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

#include <glm/glm.hpp>

#define WIDTH 1024
#define HEIGHT 600

static Display glDisplay;
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
    Texture texture("./res/soccer3.jpg");


    Mesh mesh1("res/monkey3.obj");
    //Mesh cube(cubeVertices, sizeof(cubeVertices)/sizeof(cubeVertices[0]), cubeIndices, sizeof(cubeIndices)/sizeof(cubeIndices[0]));
    Transform transform;
    //double time = SDL_GetTicks();
    IndexedModel sph = setupSphere(1.0f, 40.f, 40.f);
    Mesh sphere(sph);
    //Mesh cube = setupCube(1.0f);
    IndexedModel cc = setupCube(1.0f);
    Mesh cube(cc);

    double lastTime = SDL_GetTicks()/1000;
    int nbFrames = 0;

    float counter = 0.0f;
	while(!glDisplay.isClosed){
        Clear(0.0f, 0.15f, 0.3f, 1.0f);

        //transform.GetPos()->x = sinf(counter);

        shader.Bind();
        //*cam =
        UpdateCam(&cam, WIDTH, HEIGHT);
        glm::vec3 translate(0.0f,0.0f,0.0f);
        // Draw 20 triangles
        texture.Bind(0);
        //glPolygonMode(GL_FRONT, GL_LINE);
        //glPolygonMode(GL_BACK, GL_LINE);
        for(int i = 0; i<4; i++){
            shader.Update(transform ,*cam);
            sphere.Draw();
            //mesh1.Draw();
            translate.x+=4.6f;

            glm::vec3 rr(counter*0.3f, counter*0.5f, counter*0.75f);
            transform.SetRot(rr);
            transform.SetPos(translate);
        }

        UpdateDisplay(&glDisplay.window, glDisplay.glContext, glDisplay);
        counter += 0.0051f;
        // Measure Speed
        double currentTime = SDL_GetTicks()/1000;
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            printf("\n\t*********\n");
            printf("%.2f Milliseconds per frame\n", 1000.0/double(nbFrames));
            printf("%.2f Frames per Second\n", 1000.0 / (1000.0/double(nbFrames)));
            printf("\t*********\n");
            nbFrames = 0;
            lastTime += 1.0;
        }

	}
	CloseWindow(glDisplay.window, glDisplay.glContext);
	delete cam;

    return 0;
}
