#include <iostream>
#include <stdio.h>
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

#include <glm/glm.hpp>

#define WIDTH 1024
#define HEIGHT 600
static Display glDisplay;

// For updating movement and simulation
const float DESIRED_FPS = 60.0f;
// Just for fram time or rendering
const float FRAME_CAP = 60.0f;
const float MS_PER_FRAME = 1000.0f;
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
        for(int i = 0; i<4; i++){
            shader.Update(transform ,*cam);

            if(i == 2){
                texture2.Bind(0);
                mesh1.Draw();
            }
            else if(i == 3){
                texture3.Bind(0);
                sphere.Draw();
            }
            else{
                texture.Bind(0);
                cube.Draw();
            }

            translate.x+=4.6f;


            transform.SetPos(translate);
        }
        glm::vec3 rr(counter*0.3f, counter*0.5f, counter*0.75f);
        transform.SetRot(rr);

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
