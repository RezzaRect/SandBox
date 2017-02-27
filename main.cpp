#include <iostream>
#include <stdio.h>
#include <time.h>
 // http://stackoverflow.com/questions/5259714/undefined-reference-to-winmain16
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "project_includes.h"

// Include GLM
#include <glm/glm.hpp>

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


int main(int argc, char** argv)
{

    Transform* bulletTransform = new Transform;
    Display* glDisplay = new Display;
    Camera* cam = new Camera;

	InitDisplay(&glDisplay, WIDTH, HEIGHT);

    glm::vec3 UP(0.0f, 1.0f, 0.0f);
    glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);
    *cam = InitCamera(70.f, (float)WIDTH/(float)HEIGHT, 0.01f, 1000.0f, //256.0f
                         glm::vec3(0.0f, -5.0f, -15.0f), FORWARD, UP);

    glm::vec3 sp = glm::vec3(0.0, 10.0, 0.0);
    glm::ivec3 white = glm::vec3(255.0);
    ParticleEmitter* particles = new ParticleEmitter(3000, sp, white, 10);
    sp.x += 10.0f;
    ParticleEmitter* particles1= new ParticleEmitter(3000, sp, white, 10);


	// Initialize The physics world.
	btDiscreteDynamicsWorld* dynamicsWorld = intitBullet(-9.81f);
    // vector array to store all bullet physics objects
    std::vector<btRigidBody*> rigidbodies;
    //std::vector<Camera*> cams;
	//Loading textures and shaders from file...
	Shader shader("./res/shaders/basicShader");
	Shader particleShader("./res/shaders/particleShader");
    Texture texture("./res/textures/container2.png"); //lvv.jpg
    //Texture texture("./res/textures/brickwall.jpg"); //lvv.jpg

    Texture texture2("./res/textures/container2_specular.png"); // bricks.jpg
    //Texture texture2("./res/textures/brickwall_normal.jpg"); // bricks.jpg

    Texture texture3("./res/textures/soccer3.jpg");
    Texture bamboo("./res/textures/bamboo.jpg");
    //Texture gunTex("./res/models/normal_up.jpg");
    Texture particleTexture("./res/textures/Cloud-particle.png");
    Texture particleTexture2("./res/textures/pss.png");

    Transform planeTransform;
    Transform modelTransform;

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

#ifdef DEBUG_RENDERER
    BulletDebugDrawer_DeprecatedOpenGL mydebugdrawer;
    dynamicsWorld->setDebugDrawer(&mydebugdrawer);
#endif // DEBUG_RENDERER

    int sphereLimiter = 0;
    int sphereAdded = MAX_OBJECTS;


    SDL_ShowCursor(SDL_DISABLE);

	while(!glDisplay->isClosed){
        //Clear(0.0f, 0.15f, 0.3f, 1.0f);
        Clear(0.0f, 0.0f, 0.0f, 1.0f);
        glDisable(GL_BLEND);

        sphereLimiter++;
        float newTicks = SDL_GetTicks();
		float delta = newTicks - previousTicks;
		previousTicks = newTicks;
        float totalDeltaTime = delta / DESIRED_FRAMETIME;

        const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);
        SDL_Event event;
        SDL_PollEvent(&event);

        if (keyboard_state_array[SDL_SCANCODE_R]){
            resetBoxPositions(rigidbodies, &dynamicsWorld, MAX_OBJECTS, sphereAdded);
            sphereAdded = MAX_OBJECTS;
        }



        particles->UpdateParticles(&cam, .016f);
        particles1->UpdateParticles(&cam, .016f);

#ifdef PHYSICS_TEST
        dynamicsWorld->stepSimulation((1.0f / 60.0f)*totalDeltaTime, MAX_PHYSICS_STEPS);
#endif // PHYSICS_TEST

        shader.Bind();
        UpdateCam(&cam, WIDTH, HEIGHT, totalDeltaTime);
        glm::vec3 translate(0.0f,0.0f,0.0f);
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

        shader.Update(planeTransform, *cam);
        bamboo.Bind(0);
        plane->Draw();

        shader.Update(planeTransform, *cam);


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
        particleTexture2.Bind(0);
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

