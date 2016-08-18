#include "Display.h"
//#include <GL/gl.h>
//#include <SDL.h>
#include <iostream>
#include <stdio.h>


void InitDisplay(SDL_Window** window, SDL_GLContext& context, int width, int height){

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);


	//SDL_Window* m_window
	*window = SDL_CreateWindow("Sand-Box", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	//SDL_GLContext m_glContext
	context = SDL_GL_CreateContext(*window);

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
	//glCullFace(GL_FRONT);
}

void Clear(float r, float g, float b, float a){
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // );
}

void SwapBuffers(SDL_Window* w){
    SDL_GL_SwapWindow(w);
}

void CloseWindow(SDL_Window* w, SDL_GLContext c){
    SDL_GL_DeleteContext(c);
    SDL_DestroyWindow(w);
    SDL_Quit();
}

void UpdateDisplay(SDL_Window** w, SDL_GLContext& c, Display& disp){
    SwapBuffers(*w);

    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);

    SDL_Event event;
    SDL_PollEvent(&event);
    if (keyboard_state_array[SDL_SCANCODE_ESCAPE])
        disp.isClosed = true;

    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT) // || keyboard_state_array[SDL_SCANCODE_ESCAPE]
            disp.isClosed = true;
            //CloseWindow(w, c);
    }
}

