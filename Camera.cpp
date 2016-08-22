#include "Camera.h"

#include <SDL.h>

const glm::vec3 UP(0.0, 1.0f, 0.0);
const glm::vec3 FORWARD(0.0f, 0.0f, 1.0f);

glm::mat4 CreateProjection(Camera& cam){
    return glm::perspective(cam.fov, cam.aspect, cam.zNear, cam.zFar);
}

Camera InitCamera(float fov, float aspect, float zNear, float zFar,
                  glm::vec3 pos, glm::vec3 forward, glm::vec3 up)
{
    Camera cam;
    cam.fov = fov;
    cam.aspect = aspect;
    cam.zNear = zNear;
    cam.zFar = zFar;
    cam.pos = pos;
    cam.forward = forward;
    cam.up = up;
    cam.projection = CreateProjection(cam);
    return cam;
}

glm::mat4 GetView(Camera& cam){
    return glm::lookAt(cam.pos, cam.pos + cam.forward, cam.up);
}

glm::mat4 GetViewProjection(Camera& cam){
    return cam.projection * glm::lookAt(cam.pos, cam.pos + cam.forward, cam.up);
}

void MoveForward(glm::vec3* p, float amount){
    *p += (FORWARD * amount); // * deltaTime..?
}

void MoveBackward(glm::vec3* p, float amount){
    *p -= (FORWARD * amount);
}

void MoveRight(glm::vec3* p, float amount){
    *p += (glm::cross(UP, FORWARD) * amount);
}

void MoveLeft(glm::vec3* p, float amount){
    *p -= (glm::cross(UP, FORWARD) * amount);
}

// horizontal angle : toward -Z
float horizontalAngle = 0.0f; //3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;

// http://stackoverflow.com/questions/1252976/how-to-handle-multiple-keypresses-at-once-with-sdl

void UpdateCam(Camera** cam, int WIDTH, int HEIGHT, float deltaTime){
    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);
    SDL_Event event;
    SDL_PollEvent(&event);

    //Camera c = cam;
    //c.pos.x += 0.001f;

    int xPos, yPos;
    SDL_GetMouseState(&xPos, &yPos);
    SDL_WarpMouseInWindow(NULL, WIDTH/2, HEIGHT/2);


    float speed = 0.1; //3.0f; // 3 units / second
    float mouseSpeed = 0.005f;


    // Compute new orientation
    horizontalAngle += mouseSpeed  * float(WIDTH/2 - xPos); //float(1024/2 - xpos );
    verticalAngle   += mouseSpeed  * float(HEIGHT/2 - yPos); //float( 768/2 - ypos );

    // Represents the Direction that you are pointing in, which also is used
    // to translate the cam forward and back too
    (*cam)->forward = glm::vec3(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle));

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );

    // "CAM.FORWARD IS THE DIRECTION HERE"
    (*cam)->up = glm::cross(right, (*cam)->forward);

    if(keyboard_state_array[SDL_SCANCODE_LSHIFT])
        speed *= 2.0f;
    else
        speed = 0.1f;

    if (keyboard_state_array[SDL_SCANCODE_A])
        (*cam)->pos -= right * speed * deltaTime; //0.005f;

    if (keyboard_state_array[SDL_SCANCODE_D])
        (*cam)->pos += right * speed * deltaTime; //0.005f;

    if (keyboard_state_array[SDL_SCANCODE_S])
        (*cam)->pos -= (*cam)->forward * speed * deltaTime; //0.005f;

    if (keyboard_state_array[SDL_SCANCODE_W])
        (*cam)->pos += (*cam)->forward * speed * deltaTime; //0.005f;
    // not cam up but absolute up
    if(keyboard_state_array[SDL_SCANCODE_Q])
        (*cam)->pos += glm::vec3(0.0,1.0,0.0) *speed * deltaTime;
    // not cam->up but absolute up
    if(keyboard_state_array[SDL_SCANCODE_Z])
        (*cam)->pos -= glm::vec3(0.0,1.0,0.0) *speed * deltaTime;

    (*cam)->projection = CreateProjection(**cam);
    //return c;
}

//Vertex(glm::vec3(-.5, -.5, .0)),
