#include "Display.h"
//#include <GL/gl.h>
//#include <SDL.h>
#include <iostream>
#include <stdio.h>


void InitDisplay(Display** disp, int width, int height){

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);


	//SDL_Window* m_window
	(*disp)->window = SDL_CreateWindow("Sand-Box", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL); // | SDL_WINDOW_FULLSCREEN_DESKTOP
	//SDL_GLContext m_glContext =
	(*disp)->glContext = (SDL_GLContext*)SDL_GL_CreateContext((*disp)->window);

	glewExperimental=GL_TRUE;
	GLenum res = glewInit();
    if(res != GLEW_OK)
    {
		std::cerr << "Glew failed to initialize!" << std::endl;
    }

    // stackoverflow.com/questions/12184506/why-does-glgetstringgl-
    // version-return-null-zero-instead-of-the-opengl-versio
    printf("OpenGL version: '%s' \nHardware: '%s' \n",
           glGetString(GL_VERSION), glGetString(GL_RENDERER));

	glEnable(GL_DEPTH_TEST);


	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
}

void Clear(float r, float g, float b, float a){
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // );
}

void SwapBuffers(Display** disp){
    SDL_GL_SwapWindow((*disp)->window);
}

void CloseWindow(Display** disp){
    SDL_GL_DeleteContext((*disp)->glContext);
    SDL_DestroyWindow((*disp)->window);
    SDL_Quit();
}

void UpdateDisplay(Display** disp, char* title){
    SwapBuffers(disp);

    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);

    SDL_Event event;
    SDL_PollEvent(&event);
    if (keyboard_state_array[SDL_SCANCODE_ESCAPE])
        (*disp)->isClosed = true;

    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT) // || keyboard_state_array[SDL_SCANCODE_ESCAPE]
            (*disp)->isClosed = true;
            //CloseWindow(w, c);
    }
    SDL_SetWindowTitle((*disp)->window, title);
}

void BindAsRenderTarget(int width, int height){
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
}
