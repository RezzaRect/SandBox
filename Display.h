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
    SDL_GLContext glContext;
    bool isClosed;
};

void InitDisplay(SDL_Window** window, SDL_GLContext& context, int width, int height);
void Clear(float r, float g, float b, float a);
void SwapBuffers(SDL_Window* w, SDL_GLContext c);
void CloseWindow(SDL_Window* w, SDL_GLContext c);
void UpdateDisplay(SDL_Window** w, SDL_GLContext& c, Display& disp);

#endif // DISPLAY_H_INCLUDED

