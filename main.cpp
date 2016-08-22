#include <iostream>
#include <stdio.h>
#include <time.h>
 // http://stackoverflow.com/questions/5259714/undefined-reference-to-winmain16
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "Display.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "obj_loader.h"
#include "Geometric_Primitives.h"
#include "Clock.h"
#include "BulletDebugDeprecatedGL.h"

// Include GLM
#include <glm/glm.hpp>

#include <btBulletDynamicsCommon.h>

#define PHYSICS_TEST
#define DEBUG_RENDERER

#define WIDTH 1024
#define HEIGHT 600


// For updating movement and simulation
const float DESIRED_FPS = 60.0f;
// Just for frame time or rendering
const float FRAME_CAP = 60.0f;
const float MS_PER_FRAME = 1000.0f;
const int MAX_OBJECTS = 200;
const int MAX_PHYSICS_STEPS = 7;
//const float MAX_DELTA_TIME = 1.0f;
const float DESIRED_FRAMETIME = MS_PER_FRAME / DESIRED_FPS;

static Display glDisplay;

int main(int argc, char** argv)
{
    glDisplay.isClosed = false;
	InitDisplay(&glDisplay.window, glDisplay.glContext, WIDTH, HEIGHT);

    Camera* cam = new Camera;
    glm::vec3 UP(0.0f, 1.0f, 0.0f);
    glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);


    /* ------------- START BULLET INITIALIZATION ------------------- */
    // Build the broadphase
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	// The world.
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-9.81f,0));
    /* ------------- END BULLET INITIALIZATION ------------------- */


    std::vector<btRigidBody*> rigidbodies;

	// A box of 2m*2m*2m (1.0 is the half-extent !)
	btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

    srand(time(NULL));

	// Generate positions & rotations for MAX_OBJECTS
	std::vector<glm::vec3> positions(MAX_OBJECTS);
	std::vector<glm::quat> orientations(MAX_OBJECTS);
	for(int i=0; i<MAX_OBJECTS; i++){
		positions[i] = glm::vec3(rand()%20-10, rand()%20+30, rand()%20-10);
		orientations[i] = glm::normalize(glm::quat(glm::vec3(rand()%360, rand()%360, rand()%360)));
	}

	for(int i=0; i<MAX_OBJECTS; i++){
		btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
			btQuaternion(orientations[i].x, orientations[i].y, orientations[i].z, orientations[i].w),
			btVector3(positions[i].x, positions[i].y, positions[i].z)
		));
		btScalar mass = 1.0f;
		btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f);
		boxCollisionShape->calculateLocalInertia(mass, inertia);

		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
			mass,                  // mass, in kg. 0 -> Static object, will never move.
			motionstate,
			boxCollisionShape,      // collision shape of body
			inertia                 // local inertia
		);

		rigidBodyCI.m_linearDamping = .2f;
		rigidBodyCI.m_angularDamping = .2f;

		btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

		rigidBody->setFriction(.5f);
		rigidBody->setRollingFriction(.5f);
		rigidBody->setRestitution(1.3f);

		rigidbodies.push_back(rigidBody);
		dynamicsWorld->addRigidBody(rigidBody);

		// Small hack : store the mesh's index "i" in Bullet's User Pointer.
		// Will be used to know which object is picked.
		// A real program would probably pass a "MyGameObjectPointer" instead.
		rigidBody->setUserPointer((void*)i);

	}
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -15, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);

    *cam = InitCamera(70.f, (float)WIDTH/(float)HEIGHT, 0.01f, 1000.0f, //256.0f
                     glm::vec3(0.0f, -5.0f, -15.0f), FORWARD, UP);

	Shader shader("./res/basicShader");
    Texture texture("./res/lvv.jpg");
    Texture texture2("./res/bricks.jpg");
    Texture texture3("./res/soccer3.jpg");
    Texture bamboo("./res/bamboo.jpg");


    Mesh mesh1("res/monkey3.obj");
    //Mesh cube(cubeVertices, sizeof(cubeVertices)/sizeof(cubeVertices[0]), cubeIndices, sizeof(cubeIndices)/sizeof(cubeIndices[0]));
    Transform transform;
    Transform planeTransform;
    IndexedModel sph = setupSphere(1.0f, 15.f, 15.f);
    Mesh sphere(sph);
    IndexedModel cc = setupCube(1.0f);
    Mesh cube(cc);
    IndexedModel pl = setupLowDensityPlane(30.0f);
    Mesh plane(pl);

    double lastTime = SDL_GetTicks()/1000;
    float previousTicks = SDL_GetTicks();
    int nbFrames = 0;
    Clock clock;
    //float counter = 0.0f;

#ifdef DEBUG_RENDERER
    BulletDebugDrawer_DeprecatedOpenGL mydebugdrawer;
    dynamicsWorld->setDebugDrawer(&mydebugdrawer);
#endif // DEBUG_RENDERER
    int quatPrint = 0;
	while(!glDisplay.isClosed){
        Clear(0.0f, 0.15f, 0.3f, 1.0f);
        quatPrint++;
        float newTicks = SDL_GetTicks();
		float delta = newTicks - previousTicks;
		previousTicks = newTicks;
        float totalDeltaTime = delta / DESIRED_FRAMETIME;



        //transform.GetPos()->x = sinf(counter);
#ifdef PHYSICS_TEST
        dynamicsWorld->stepSimulation(1.0 / 60.0f, MAX_PHYSICS_STEPS);
#endif // PHYSICS_TEST

        shader.Bind();
        //*cam =
        UpdateCam(&cam, WIDTH, HEIGHT, totalDeltaTime);
        glm::vec3 translate(0.0f,0.0f,0.0f);
        texture.Bind(0);
        //glPolygonMode(GL_FRONT, GL_LINE);
        //glPolygonMode(GL_BACK, GL_LINE);

        for(int i = 0; i < MAX_OBJECTS; i++){
            // Set Positions
            btVector3 btpos = rigidbodies[i]->getCenterOfMassPosition();
            glm::vec3 pos(btpos.x(), btpos.y(), btpos.z());
            transform.SetPos(pos);

            // Set Rotations
            btMatrix3x3 mrot = rigidbodies[i]->getCenterOfMassTransform().getBasis();
            glm::mat4 qq(mrot[0][0], mrot[0][1], mrot[0][2], 0.0f,
                         mrot[1][0], mrot[1][1], mrot[1][2], 0.0f,
                         mrot[2][0], mrot[2][1], mrot[2][2], 0.0f,
                         0.0f,       0.0f,       0.0f,       1.0f);
            transform.SetRot(qq);

            // Update the model transform and camera matrices for the shader
            shader.Update(transform, *cam);
#ifdef PHYSICS_TEST
            cube.Draw();
#endif // PHYSICS_TEST

#ifndef PHYSICS_TEST
            if(i%3 == 0){
                texture2.Bind(0);
                mesh1.Draw();
            }
            else if(i%5 == 0){
                texture3.Bind(0);
                sphere.Draw();
            }
            else{
                texture.Bind(0);
                cube.Draw();
            }
#endif // PHYSICS_TEST

        }

        glm::vec3 planePosition(0.0f, 15.9f, 0.0f);
        planeTransform.SetPos(planePosition);
        shader.Update(planeTransform, *cam);
        bamboo.Bind(0);
        plane.Draw();

#ifdef DEBUG_RENDERER
        mydebugdrawer.SetMatrices(GetView(*cam), cam->projection);
        dynamicsWorld->debugDrawWorld();
#endif // DEBUG_RENDERER


        UpdateDisplay(&glDisplay.window, glDisplay.glContext, glDisplay);
        // Measure Speed
        double currentTime = SDL_GetTicks()/1000;
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            printf("\n\t*********\n");
            printf("%.2f Milliseconds per frame\n", 1000.0/double(nbFrames));
            printf("%.2f Frames per Second\n", double(nbFrames));
            printf("%.4f Is the delta between frames\n", totalDeltaTime);
            printf("\t*********\n");
            nbFrames = 0;
            lastTime += 1.0;
        }

        while(clock.getElapsedTime()/1000.0 < 1.0 / FRAME_CAP)
            ;
        clock.restart();
	}

	CloseWindow(glDisplay.window, glDisplay.glContext);
	delete cam;

    return 0;
}

