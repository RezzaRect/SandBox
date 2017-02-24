#include <iostream>
#include <stdio.h>
#include <time.h>
 // http://stackoverflow.com/questions/5259714/undefined-reference-to-winmain16
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "Display.h"
//#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "obj_loader.h"
#include "Geometric_Primitives.h"
#include "Clock.h"
#include "BulletDebugDeprecatedGL.h"
#include "Raycast.h"
#include "BulletPhysics.h"
#include "Model.h"
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
const int MAX_OBJECTS = 175;
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

	// Initialize The physics world.
	btDiscreteDynamicsWorld* dynamicsWorld = intitBullet(-9.81f);


    std::vector<btRigidBody*> rigidbodies;

	// A box of 2m*2m*2m (1.0 is the half-extent !)  width
	//btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

    //srand(time(NULL));

	// Generate positions & rotations for MAX_OBJECTS
	std::vector<glm::vec3> positions(MAX_OBJECTS);
	std::vector<glm::quat> orientations(MAX_OBJECTS);
	std::vector<glm::vec3> rectDimensions(MAX_OBJECTS);
	std::vector<Mesh*> planks; //(MAX_OBJECTS);
	float wallX = -34;
	float wallY = -13;
    float wallZ = 20;
    float stuffAdded = 234.234f; // ...

    for(int i = 0; i < MAX_OBJECTS; i++){

    }

	//*
	for(int i=0; i<MAX_OBJECTS; i++){
		positions[i] = glm::vec3(wallX, wallY, wallZ);
		orientations[i] = glm::normalize(glm::quat(glm::vec3(0, 0, 0)));
		wallX++;
		if(i % 10 == 0){
            wallX -= 10;
            wallY+=1.1;
		}
		if(i%55 == 0){
            //wallZ+=12.0;
            wallY = -13;
            wallX +=25;
		}
	}
    /*
    srand(time(NULL));
    for(int i = 0; i < MAX_OBJECTS; i++){
        float x = rand() % 30;
        float y = rand() % 40 + 20;
        float z = rand() % 30;
        positions[i] = glm::vec3(float(x), float(y), float(z));
        orientations[i] = glm::normalize(glm::quat(glm::vec3(rand()%360, rand()%360, rand()%360)));
        float rx = 0.1f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.6f-0.1f)));
        float ry = 0.5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(2.2f-0.5f)));
        float rz = 3.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(6.0f-3.0f)));
        if(i % 5 == 0){
            rx = ry;
            ry = ry;
            rz = ry;
        }

        rectDimensions[i] = glm::vec3(rx, ry, rz);
    }
*/


	for(int i=0; i<MAX_OBJECTS; i++){
		btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
			btQuaternion(orientations[i].x, orientations[i].y, orientations[i].z, orientations[i].w),
			btVector3(positions[i].x, positions[i].y, positions[i].z)
		));
		btScalar mass = 1.0f;
		btVector3 inertia = btVector3(0.0f, 0.0f, 0.0f); //btVector3(rectDimensions[i].x, rectDimensions[i].y, rectDimensions[i].z)
		btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
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
		//rigidBody->setRestitution(1.1f);

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
    //groundRigidBody->setRestitution(1.0f);
    dynamicsWorld->addRigidBody(groundRigidBody);

    *cam = InitCamera(70.f, (float)WIDTH/(float)HEIGHT, 0.01f, 1000.0f, //256.0f
                     glm::vec3(0.0f, -5.0f, -15.0f), FORWARD, UP);

	//Loading textures from file...
	Shader shader("./res/basicShader");
    Texture texture("./res/container2.png"); //lvv.jpg
    Texture texture2("./res/container2_specular.png"); // bricks.jpg
    Texture texture3("./res/soccer3.jpg");
    Texture bamboo("./res/bamboo.jpg");
    Texture gunTex("./res/models/normal_up.jpg");
    //Load models from file...
    //Mesh mesh1("res/monkey3.obj");
    //Model nano("./res/models/nanosuit.obj");

    //Mesh cube(cubeVertices, sizeof(cubeVertices)/sizeof(cubeVertices[0]), cubeIndices, sizeof(cubeIndices)/sizeof(cubeIndices[0]));
    Transform transform;
    Transform planeTransform;
    Transform modelTransform;

    IndexedModel sph = setupSphere(1.0f, 15.f, 15.f);
    Mesh sphere(sph);

    IndexedModel cc = setupCube(1.0f);
    Mesh cube(cc);

    IndexedModel pl = setupLowDensityPlane(30.0f);  //;setupHighDensityPlane(10.0f)
    Mesh plane(pl);

    //IndexedModel rct = setupRectangularCube(0.5f, 1.0f, 5.0f);
    //Mesh rect(rct);

    //for(int i = 0; i < MAX_OBJECTS; i++){
        //IndexedModel tempRect = setupRectangularCube(rectDimensions[i].x, rectDimensions[i].y, rectDimensions[i].z);
        //planks.push_back(new Mesh(tempRect));
    //}

    //IndexedModel nano =
    //Mesh dummieVar(pl);
    //std::vector<IndexedModel> nanosuit = loadModelAssimp("nanosuit.obj"); //nanosuit(nano);
    //std::vector<Mesh*> nanoMeshes;//[nanosuit.size()] ;
    //for(unsigned int i = 0; i < nanosuit.size(); i++)
        //nanoMeshes.push_back(new Mesh(nanosuit[i]));

    double lastTime = SDL_GetTicks()/1000;
    float previousTicks = SDL_GetTicks();
    int nbFrames = 0;
    Clock clock;
    float counter = 0.0f;


#ifdef DEBUG_RENDERER
    BulletDebugDrawer_DeprecatedOpenGL mydebugdrawer;
    dynamicsWorld->setDebugDrawer(&mydebugdrawer);
#endif // DEBUG_RENDERER

    int sphereLimiter = 0;
    int sphereAdded = 0;
    float modelSpread = 0.0f;
    //printf("Model contains %d textures\n", nano.textures.size());
    printf("Names of textures...\n\n");

    // Print out the all the model's texture strings...
    //for(unsigned int i = 0; i < nano.textures.size(); i++){
        //string name = nano.textures[i].type;
        //printf("%d Type: %s\n", i, name.c_str());
    //}

	while(!glDisplay.isClosed){
        Clear(0.0f, 0.15f, 0.3f, 1.0f);
        modelSpread += 0.01f;
        sphereLimiter++;
        float newTicks = SDL_GetTicks();
		float delta = newTicks - previousTicks;
		previousTicks = newTicks;
        float totalDeltaTime = delta / DESIRED_FRAMETIME;



        //transform.GetPos()->x = sinf(counter);
#ifdef PHYSICS_TEST
        dynamicsWorld->stepSimulation((1.0f / 60.0f)*totalDeltaTime, MAX_PHYSICS_STEPS);
#endif // PHYSICS_TEST

        shader.Bind();
        //*cam =
        UpdateCam(&cam, WIDTH, HEIGHT, totalDeltaTime);
        glm::vec3 translate(0.0f,0.0f,0.0f);
        //texture.Bind(0);
        texture.Bind(0);
        texture2.Bind(1);
        //glPolygonMode(GL_FRONT, GL_LINE);
        //glPolygonMode(GL_BACK, GL_LINE);

        for(int i = 0; i < MAX_OBJECTS + sphereAdded; i++){
            // Set Positions
            btVector3 btpos = rigidbodies[i]->getCenterOfMassPosition();
            glm::vec3 pos(btpos.x(), btpos.y(), btpos.z());
            //glm::vec3 pp(0.0, -4.0, 0.0);
            transform.SetPos(pos);

            // Set Rotations
            btMatrix3x3 mrot = rigidbodies[i]->getCenterOfMassTransform().getBasis();
            glm::mat4 qq(mrot[0][0], mrot[0][1], mrot[0][2], 0.0f,
                         mrot[1][0], mrot[1][1], mrot[1][2], 0.0f,
                         mrot[2][0], mrot[2][1], mrot[2][2], 0.0f,
                         0.0f,       0.0f,       0.0f,       1.0f);
            transform.SetRot(qq);
            //glm::vec3 ss(0.25);
            //transform.SetScale(ss);

            // Update the model transform and camera matrices for the shader
            shader.Update(transform, *cam);
#ifdef PHYSICS_TEST
            //float distCull = glm::length(pos - cam->pos);
            //float cosTheta = glm::dot(cam->forward, glm::normalize(pos - cam->pos));
            //float theta = glm::acos(cosTheta);
            //glm::abs(theta) <= cam->fov ||
            //if(distCull < 60.0f){

                if(i < MAX_OBJECTS)
                    cube.Draw(); //planks[i]->Draw();
                else{
                    texture3.Bind(0);
                    sphere.Draw();
                }
            //}
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
        //glm::vec3 sc(5.0f);
        //planeTransform.SetScale(sc);
        glm::mat4 rectrot = glm::mat4(1.0f);
        planeTransform.SetRot(rectrot);

        shader.Update(planeTransform, *cam);
        bamboo.Bind(0);
        //texture.Bind(0);
        plane.Draw();

        planePosition.y += 2.0f;
        planeTransform.SetPos(planePosition);

        rectrot *= glm::rotate(rectrot, SDL_GetTicks()/1000.0f, glm::vec3(0.75f, 1.5f, 0.25f));
        planeTransform.SetRot(rectrot);

        shader.Update(planeTransform, *cam);
        //texture.Bind(0);
        //rect.Draw();

        //planePosition.y -= 30.;
        //planePosition.x += 20.;
        //planeTransform.SetPos(planePosition);
        //shader.Update(planeTransform, *cam);
        counter+=0.01f;
        shader.Update(modelTransform, *cam);
        glm::vec3 modelMeshPositions(0.0, 8.0, 0.0);
        glm::mat4 rot3d(1.0f);
        rot3d*=glm::rotate(counter, glm::vec3(0.0, 1.0, 0.0) );
        modelTransform.SetRot(rot3d);

        shader.Update(modelTransform, *cam);

        modelMeshPositions=glm::vec3(8.0f, -8.0f, 0.0);
        modelTransform.SetPos(modelMeshPositions);
        shader.Update(modelTransform, *cam);

        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        //nano.Draw();
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#ifdef DEBUG_RENDERER
        mydebugdrawer.SetMatrices(GetView(*cam), cam->projection);
        dynamicsWorld->debugDrawWorld();
#endif // DEBUG_RENDERER

        // PICKING IS DONE HERE
		SDL_PumpEvents();
		glm::vec3 push = glm::normalize(cam->forward);
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
            int i = getPhysicsObjIndex(&dynamicsWorld, &cam);
            rigidbodies[i]->setActivationState(1.0f);
            rigidbodies[i]->applyCentralImpulse(btVector3(-push.x, -push.y, -push.z));
        } else if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
            if(sphereLimiter >= 20){
                btRigidBody* sphere=addSphere(1.0, cam->pos.x, cam->pos.y, cam->pos.z, 1.0, &dynamicsWorld, rigidbodies);
                sphere->setLinearVelocity(btVector3(push.x,push.y,push.z)*75.0f);
                sphereLimiter = 0;
                sphereAdded++;
            }
        }


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

