#include <iostream>
#include <stdio.h>
#include <time.h>
 // http://stackoverflow.com/questions/5259714/undefined-reference-to-winmain16
#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "project_includes.h"

#define PHYSICS_TEST
//#define DEBUG_RENDERER
//#define USE_DEFERRED_RENDERER

#define WIDTH 1366
#define HEIGHT 768

const float TIME_STEP = 1.0f / 60.0f;
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
    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec;

    Transform* bulletTransform = new Transform;
    Display* glDisplay = new Display;
    Camera* cam = new Camera;

	InitDisplay(&glDisplay, WIDTH, HEIGHT, true);
    SDL_ShowCursor(SDL_DISABLE);

    glm::vec3 UP(0.0f, 1.0f, 0.0f);
    glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);
    *cam = InitCamera(70.f, (float)WIDTH/(float)HEIGHT, 0.01f, 1000.0f, //256.0f
                         glm::vec3(0.0f, -5.0f, -15.0f), FORWARD, UP, 0.25f, 2.0f, 0.005f);

    glm::vec3 sp = glm::vec3(0.0, 10.0, 0.0);
    glm::ivec3 white = glm::vec3(255.0);
    //sp.x += -200.0f;
    int width, height, bytesPerPixel;
	unsigned char* hfData = stbi_load("./res/textures/Heightmap.png", &width, &height, &bytesPerPixel, 3);
    int dataLn = width * height * bytesPerPixel; //strlen((const char*)hfData);//*sizeof(unsigned char);  //sizeof(hfData)

    unsigned char* tmpchr = hfData;

    std::cout << "Sizeof const char: " << dataLn << std::endl;

    sp.x = -width/2;
    sp.z = -height/2;
    ParticleEmitter* particles = new ParticleEmitter(3500, sp, white, 10);
    ParticleEmitter* particles1= new ParticleEmitter(3500, sp, white, 10);

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	// Initialize The physics world.
	btDiscreteDynamicsWorld* dynamicsWorld = intitBullet(-9.81f);
    // vector array to store all bullet physics objects
    std::vector<btRigidBody*> rigidbodies;

    glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
    //std::vector<Camera*> cams;
	//Loading textures and shaders from file...
	Shader shader("./res/shaders/basicShader");
	Shader particleShader("./res/shaders/particleShader");
	Shader screenShader("./res/shaders/screenShader");
	Shader skyboxShader("./res/shaders/skybox");
#ifdef USE_DEFERRED_RENDERER
	Shader deferredShader("./res/shaders/deferredShading");

    deferredShader.Bind();

    glUniform1i(glGetUniformLocation(deferredShader.m_program, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(deferredShader.m_program, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(deferredShader.m_program, "gAlbedoSpec"), 2);
#endif // USE_DEFERRED_RENDERER

    Texture texture("./res/textures/texture_4.png"); //lvv.jpg
    Texture texture2("./res/textures/normal_4.png"); // bricks.jpg
    Texture texture3("./res/textures/soccer3.jpg");
    Texture bamboo("./res/textures/bamboo.jpg");
    Texture particleTexture("./res/textures/Cloud-particle.png");
    Texture particleTexture2("./res/textures/pss.png");

    // Cubemap (Skybox)
    std::vector<std::string> faces;
    faces.push_back("./res/textures/skybox/right.jpg");
    faces.push_back("./res/textures/skybox/left.jpg");
    faces.push_back("./res/textures/skybox/top.jpg");
    faces.push_back("./res/textures/skybox/bottom.jpg");
    faces.push_back("./res/textures/skybox/back.jpg");
    faces.push_back("./res/textures/skybox/front.jpg");
    GLuint skyboxTexture = loadCubemap(faces);

    Texture* g_tempTarget = NULL;
    int dataSize = WIDTH * HEIGHT * 4;
    unsigned char* data = new unsigned char[dataSize];
    g_tempTarget = new Texture(WIDTH, HEIGHT, data, GL_TEXTURE_2D, GL_NEAREST, GL_COLOR_ATTACHMENT0);

    Transform planeTransform;
    Transform modelTransform;

    Mesh* sphere = new Mesh(setupSphere(1.0f, 15.f, 15.f));
    Mesh* cube = new Mesh(setupCube(1.0f));
    //Mesh* plane = new Mesh(setupLowDensityPlane(30.0f));  // new Mesh(setupHighDensityPlane(12, 0.5f, false));
    Mesh* fsQuad = new Mesh(setupFullScreenQuad(1.0f));
    Mesh* cubeM = new Mesh(setupCube(6.0f));//Mesh(setupSphere(2.5f, 10.0f, 10.0f));
    Mesh* terrain = new Mesh(Terrain(0.15, &hfData, width, height, bytesPerPixel)); //(new Model("./res/mountain/nanosuit.obj"));
    glm::vec3 orientation(1.0f);
    glm::vec3 position(0.0f, -20.0f, 0.0f);
    //addPlane(orientation, position, &dynamicsWorld);

    addTerrain(orientation, position, &dynamicsWorld, 0.15, &hfData, width, height, bytesPerPixel);
    stbi_image_free(hfData);

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


#ifdef USE_DEFERRED_RENDERER
    setupDeferredRendering(gBuffer, gPosition, gNormal, gAlbedoSpec, WIDTH, HEIGHT);
#endif // USE_DEFERRED_RENDERER


	while(!glDisplay->isClosed){

        g_tempTarget->BindAsRenderTarget();
        Clear(0.0f, 0.0f, 0.0f, 1.0f);


        //Nessecary..?
        //glDisable(GL_BLEND);
#ifdef USE_DEFERRED_RENDERER
        glPolygonMode(GL_FRONT_AND_BACK, false ? GL_LINE : GL_FILL);
		// 1. Geometry Pass: render scene's geometry/color data into gbuffer

		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.Bind();
#endif // USE_DEFERRED_RENDERER
        glEnable(GL_DEPTH_TEST);

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
            //fsQuad->Draw();
        }

        glm::vec3 planePosition(0.0f, -19.9f, 0.0f);
        planeTransform.SetPos(planePosition);

        shader.Update(planeTransform, *cam);
        bamboo.Bind(0);
        //plane->Draw();

        shader.Update(planeTransform, *cam);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);

        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);
        //glFrontFace(GL_CW);

        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        terrain->Draw();
        //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glDisable(GL_CULL_FACE);

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
            rigidbodies[i]->applyCentralImpulse(btVector3(push.x, push.y, push.z));
        } else if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
            if(sphereLimiter >= 10){
                addSphere(1.0, cam->pos, push, 1.0, &dynamicsWorld, rigidbodies, sphereAdded);
                sphereLimiter = 0;
                sphereAdded++;
            }
        }

//#ifndef USE_DEFERRED_RENDERER
        // Cubes
        Transform tempBlank;
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        // Draw skybox as last
        glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Bind();
        glm::vec3 hakky = cam->pos;
        cam->pos = glm::vec3(0.0f);
        skyboxShader.Update(tempBlank, *cam);
        cam->pos = hakky;
        // skybox cube
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        cubeM->Draw();
        glDepthFunc(GL_LESS); // Set depth function back to default
//#endif


#ifndef USE_DEFERRED_RENDERER





        Transform tmptr;
        particleShader.Bind();
        particleTexture.Bind(0);
        particleShader.Update(tmptr, *cam);
        particleShader.Bind();

        particles->RenderParticles();
        particleTexture2.Bind(0);
        particles1->RenderParticles();

        /////////////////////////////////////////////////////
        // Bind to default framebuffer again and draw the
        // quad plane with attched screen texture.
        // //////////////////////////////////////////////////

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Clear all relevant buffers
        Clear(0.0f, 0.15f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad

        // Draw Screen
        g_tempTarget->Bind(0);
        screenShader.Bind();
        fsQuad->Draw();
#endif // USE_DEFERRED_RENDERER

#ifdef USE_DEFERRED_RENDERER
        glClearColor(0.0f, 0.15f, 0.3f, 1.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Transform tr;
        deferredShader.Bind();
        deferredShader.Update(tr, *cam);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

        fsQuad->Draw();

        // 2.5. Copy content of geometry's depth buffer to default framebuffer's depth buffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
		// depth buffer in another stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

        UpdateDisplay(&glDisplay, "Updated");
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



        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        //nano.Draw();
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
