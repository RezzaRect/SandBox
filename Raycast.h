#ifndef RAYCAST_H_INCLUDED
#define RAYCAST_H_INCLUDED

#include <glm/glm.hpp>

// Reference: http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/

void ScreenPosToWorldRay(int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
                         int screenWidth, int screenHeight,  // Window size, in pixels
                         glm::mat4 ViewMatrix,               // Camera position and orientation
                         glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
                         glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
                         glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
                         );
//SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)


#endif // RAYCAST_H_INCLUDED
