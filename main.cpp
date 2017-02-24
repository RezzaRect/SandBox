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
//#include "obj_loader.h"
#include "Geometric_Primitives.h"
#include "Clock.h"
#include "BulletDebugDeprecatedGL.h"
//#include "Raycast.h"
#include "BulletPhysics.h"
//#include "Model.h"
#include "Particle_Emitter.h"

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
const int MAX_OBJECTS = 75;
const int MAX_PHYSICS_STEPS = 7;
//const float MAX_DELTA_TIME = 1.0f;
const float DESIRED_FRAMETIME = MS_PER_FRAME / DESIRED_FPS;

//static Display glDisplay;

int main(int argc, char** argv)
{
    //glDisplay->isClosed = false;

    Transform* bulletTransform = new Transform;

    Display* glDisplay = new Display;
	InitDisplay(&glDisplay, WIDTH, HEIGHT);

    Camera* cam = new Camera;
    glm::vec3 UP(0.0f, 1.0f, 0.0f);
    glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);


    glm::vec3 sp = glm::vec3(0.0, 10.0, 0.0);
    glm::ivec3 white = glm::vec3(255.0);
    ParticleEmitter* particles = new ParticleEmitter(1000, sp, white, 10);
    sp.x += 10.0f;
    ParticleEmitter* particles1= new ParticleEmitter(1000, sp, white, 10);


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


    *cam = InitCamera(70.f, (float)WIDTH/(float)HEIGHT, 0.01f, 1000.0f, //256.0f
                     glm::vec3(0.0f, -5.0f, -15.0f), FORWARD, UP);

	//Loading textures from file...
	Shader shader("./res/basicShader");
	Shader particleShader("./res/particleShader");
    Texture texture("./res/container2.png"); //lvv.jpg
    Texture texture2("./res/container2_specular.png"); // bricks.jpg
    Texture texture3("./res/soccer3.jpg");
    Texture bamboo("./res/bamboo.jpg");
    Texture gunTex("./res/models/normal_up.jpg");
    Texture particleTexture("./res/Cloud-particle.png");
    //Load models from file...
    //Mesh mesh1("res/monkey3.obj");
    //Model nano("./res/models/nanosuit.obj");

    //Mesh cube(cubeVertices, sizeof(cubeVertices)/sizeof(cubeVertices[0]), cubeIndices, sizeof(cubeIndices)/sizeof(cubeIndices[0]));
    Transform transform;
    Transform planeTransform;
    Transform modelTransform;

    //IndexedModel sph = setupSphere(1.0f, 15.f, 15.f);
    Mesh* sphere = new Mesh(setupSphere(1.0f, 15.f, 15.f));
    Mesh* cube = new Mesh(setupCube(1.0f));
    Mesh* plane = new Mesh(setupLowDensityPlane(30.0f));


    glm::vec3 orientation(1.0f);
    glm::vec3 position(0.0f, -20.0f, 0.0f);
    addPlane(orientation, position, &dynamicsWorld);

    setupBoxPositions(rigidbodies, &dynamicsWorld, MAX_OBJECTS);

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

	while(!glDisplay->isClosed){
        Clear(0.0f, 0.15f, 0.3f, 1.0f);
        glDisable(GL_BLEND);

        modelSpread += 0.01f;
        sphereLimiter++;
        float newTicks = SDL_GetTicks();
		float delta = newTicks - previousTicks;
		previousTicks = newTicks;
        float totalDeltaTime = delta / DESIRED_FRAMETIME;

        const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);
        SDL_Event event;
        SDL_PollEvent(&event);

        if (keyboard_state_array[SDL_SCANCODE_R])
            resetBoxPositions(rigidbodies, &dynamicsWorld, MAX_OBJECTS, sphereAdded);


        particles->UpdateParticles(&cam, .016f);
        particles1->UpdateParticles(&cam, .016f);

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
        for(int i = 0; i < MAX_OBJECTS; i++){
            //Transform bulletTransform = physicsTransforms(rigidbodies, i);
            physicsTransforms(&bulletTransform, rigidbodies, i);
            shader.Update(*bulletTransform, *cam);
            cube->Draw();
        }

        texture3.Bind(0);

        for(unsigned int i = MAX_OBJECTS; i < rigidbodies.size(); i++){
            //Transform bulletTransform = physicsTransforms(rigidbodies, i);
            physicsTransforms(&bulletTransform, rigidbodies, i);
            shader.Update(*bulletTransform, *cam);
            sphere->Draw();
        }

        glm::vec3 planePosition(0.0f, -19.9f, 0.0f);
        planeTransform.SetPos(planePosition);
        //glm::vec3 sc(5.0f);
        //planeTransform.SetScale(sc);
        glm::mat4 rectrot = glm::mat4(1.0f);
        planeTransform.SetRot(rectrot);

        shader.Update(planeTransform, *cam);
        bamboo.Bind(0);
        //texture.Bind(0);
        plane->Draw();

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
		glm::vec3 push = glm::normalize(cam->viewDir);
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
            int i = getPhysicsObjIndex(&dynamicsWorld, &cam);
            rigidbodies[i]->setActivationState(1.0f);
            rigidbodies[i]->applyCentralImpulse(btVector3(-push.x, -push.y, -push.z));
        } else if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
            if(sphereLimiter >= 20){
                //btRigidBody* sphere=addSphere(1.0, cam->pos.x, cam->pos.y, cam->pos.z, 1.0, &dynamicsWorld, rigidbodies);
                //sphere->setLinearVelocity(btVector3(push.x,push.y,push.z)*75.0f);
                //addSphere(1.0f, cam->pos, push, 1.0f, &btDynamicsWorld, rigidbodies, sphereAdded);
                addSphere(1.0, cam->pos, push, 1.0, &dynamicsWorld, rigidbodies, sphereAdded);
                sphereLimiter = 0;
                sphereAdded++;
            }
        }
        Transform tmptr;
        particleShader.Bind();
        particleTexture.Bind(0);
        particleShader.Update(tmptr, *cam);
        particleShader.Bind();

        particles->RenderParticles();
        //particleTexture2.Bind(0);
        particles1->RenderParticles();



        UpdateDisplay(&glDisplay, "Updated");
        //glDisable(GL_BLEND);
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

	CloseWindow(&glDisplay);
	delete cam;
	delete glDisplay;
	delete cube;
	delete sphere;
	delete bulletTransform;

    return 0;
}

