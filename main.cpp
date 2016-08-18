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

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <btBulletDynamicsCommon.h>

#define WIDTH 1024
#define HEIGHT 600
static Display glDisplay;

// For updating movement and simulation
const float DESIRED_FPS = 60.0f;
// Just for fram time or rendering
const float FRAME_CAP = 60.0f;
const float MS_PER_FRAME = 1000.0f;
const int MAX_OBJECTS = 50;
//const int MAX_PHYSICS_STEPS = 6;
//const float MAX_DELTA_TIME = 1.0f;
const float DESIRED_FRAMETIME = MS_PER_FRAME / DESIRED_FPS;

int main(int argc, char** argv)
{
    glDisplay.isClosed = false;
	InitDisplay(&glDisplay.window, glDisplay.glContext, WIDTH, HEIGHT);

    Camera* cam = new Camera;
    glm::vec3 UP(0.0f, 1.0f, 0.0f);
    glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);


    /* ------------- START BULLET INITIALIZATION ------------------- */
    //btBroadphaseInterface* broadphase = new btDbvtBroadphase();
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

	// In this example, all monkeys will use the same collision shape :
	// A box of 2m*2m*2m (1.0 is the half-extent !)
	btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));

    srand(time(NULL));

	// Generate positions & rotations for 100 monkeys
	std::vector<glm::vec3> positions(MAX_OBJECTS);
	std::vector<glm::quat> orientations(MAX_OBJECTS);
	for(int i=0; i<MAX_OBJECTS; i++){
		positions[i] = glm::vec3(rand()%20-10, rand()%20-10, rand()%20-10);
		orientations[i] = glm::normalize(glm::quat(glm::vec3(rand()%360, rand()%360, rand()%360)));

	}

	for(int i=0; i<MAX_OBJECTS; i++){

		btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
			btQuaternion(orientations[i].x, orientations[i].y, orientations[i].z, orientations[i].w),
			btVector3(positions[i].x, positions[i].y, positions[i].z)
		));

		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
			0,                  // mass, in kg. 0 -> Static object, will never move.
			motionstate,
			boxCollisionShape,  // collision shape of body
			btVector3(0,0,0)    // local inertia
		);
		btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

		rigidbodies.push_back(rigidBody);
		dynamicsWorld->addRigidBody(rigidBody);

		// Small hack : store the mesh's index "i" in Bullet's User Pointer.
		// Will be used to know which object is picked.
		// A real program would probably pass a "MyGameObjectPointer" instead.
		rigidBody->setUserPointer((void*)i);

	}

    *cam = InitCamera(70.f, (float)WIDTH/(float)HEIGHT, 0.01f, 1000.0f, //256.0f
                     glm::vec3(0.0f, 0.0f, -5.0f), FORWARD, UP);

	Shader shader("./res/basicShader");
    Texture texture("./res/lvv.jpg");
    Texture texture2("./res/bricks.jpg");
    Texture texture3("./res/soccer3.jpg");


    Mesh mesh1("res/monkey3.obj");
    //Mesh cube(cubeVertices, sizeof(cubeVertices)/sizeof(cubeVertices[0]), cubeIndices, sizeof(cubeIndices)/sizeof(cubeIndices[0]));
    Transform transform;
    //double time = SDL_GetTicks();
    IndexedModel sph = setupSphere(1.0f, 20.f, 20.f);
    Mesh sphere(sph);
    //Mesh cube = setupCube(1.0f);
    IndexedModel cc = setupCube(1.0f);
    Mesh cube(cc);

    double lastTime = SDL_GetTicks()/1000;
    float previousTicks = SDL_GetTicks();
    int nbFrames = 0;
    Clock clock;
    float counter = 0.0f;
	while(!glDisplay.isClosed){
        Clear(0.0f, 0.15f, 0.3f, 1.0f);

        float newTicks = SDL_GetTicks();
		float delta = newTicks - previousTicks;
		previousTicks = newTicks;
        float totalDeltaTime = delta / DESIRED_FRAMETIME;

        //transform.GetPos()->x = sinf(counter);

        shader.Bind();
        //*cam =
        UpdateCam(&cam, WIDTH, HEIGHT, totalDeltaTime);
        glm::vec3 translate(0.0f,0.0f,0.0f);
        // Draw 20 triangles
        texture.Bind(0);
        //glPolygonMode(GL_FRONT, GL_LINE);
        //glPolygonMode(GL_BACK, GL_LINE);

        for(int i = 0; i < MAX_OBJECTS; i++){
            transform.SetPos(positions[i]);
            glm::vec3 qToVec3(orientations[i].x, orientations[i].y, orientations[i].z);
            glm::vec3 rr(counter*0.3f, counter*0.5f, counter*0.75f);
            qToVec3+=rr;
            transform.SetRot(qToVec3);
            shader.Update(transform, *cam);


            //cube.Draw();
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
        }



        UpdateDisplay(&glDisplay.window, glDisplay.glContext, glDisplay);
        counter += 0.01f * totalDeltaTime; //.0051
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
