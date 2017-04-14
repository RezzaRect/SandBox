#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

// only needed linking against glew3s"S" not just glew32
#define GLEW_STATIC  //static or dynamic ehh
#include <GL/glew.h>

#include <SDL.h>
#include <string>

struct Display
{
    SDL_Window* window;
    SDL_GLContext* glContext;
    bool isClosed;
};

void InitDisplay(Display** disp, int width, int height, bool fullScreen);
void Clear(float r, float g, float b, float a);
void SwapBuffers(Display** disp);
void CloseWindow(Display** disp);
void UpdateDisplay(Display** disp, char* title);
void BindAsRenderTarget(int width, int height);

#endif // DISPLAY_H_INCLUDED

