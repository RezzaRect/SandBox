#include "Camera.h"

#include <SDL.h>
#include <glm/gtx/rotate_vector.hpp>

const glm::vec3 UP(0.0, 1.0f, 0.0);
const glm::vec3 RIGHT(0.0f, 0.0f, 1.0f);
glm::vec3 camVel = glm::vec3(0.0, 0.0, 0.0);

glm::mat4 CreateProjection(Camera& cam){
    return glm::perspective(cam.fov, cam.aspect, cam.zNear, cam.zFar);
}

Camera InitCamera(float fov, float aspect, float zNear, float zFar, glm::vec3 pos,
                  glm::vec3 viewDir, glm::vec3 up, float camSpeed, float camBoost, float mouseSen)
{
    Camera cam;
    cam.fov = fov;
    cam.aspect = aspect;
    cam.zNear = zNear;
    cam.zFar = zFar;
    cam.pos = pos;
    cam.viewDir = viewDir;
    cam.up = up;
    cam.projection = CreateProjection(cam);
    cam.camVel = glm::vec3(0.0f, 0.0f, 0.0f);
    cam.horizontalAngle = 0.0f;
    cam.verticalAngle = 0.0f;
    cam.speedVal = camSpeed;
    cam.speedBoost = camBoost;
    cam.mouseSpeed = mouseSen;
    return cam;
}

glm::mat4 GetView(Camera& cam){
    return glm::lookAt(cam.pos, cam.pos + cam.viewDir, cam.up);
}

glm::mat4 GetViewProjection(Camera& cam){
    return cam.projection * glm::lookAt(cam.pos, cam.pos + cam.viewDir, cam.up);
}

// Reference: https://github.com/id-Software/Quake/blob/master/WinQuake/view.c
// line: 81
float calcRoll(glm::vec3& velVec, glm::vec3& camRight){
    float	sign;
	float	side;
	float	value;

	//AngleVectors (angles, forward, right, up);
	side = glm::dot(velVec, camRight);
	sign = side < 0 ? -1 : 1;
	side = glm::abs(side);

	value = 25.0f;
//	if (cl.inwater)
//		value *= 6;

	if (side < 50.0f)
		side = side * value / 50.0f;
	else
		side = value;

	return side*sign;
}
// http://stackoverflow.com/questions/1252976/how-to-handle-multiple-keypresses-at-once-with-sdl

void UpdateCam(Camera** cam, int WIDTH, int HEIGHT, float deltaTime){
    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);
    SDL_Event event;
    SDL_PollEvent(&event);


    int xPos, yPos;
    SDL_GetMouseState(&xPos, &yPos);
    SDL_WarpMouseInWindow(NULL, WIDTH/2, HEIGHT/2);

    (*cam)->speedVal = 0.025;
    //float speed = 0.025; //3.0f; // 3 units / second
    //float mouseSpeed = 0.005f;

    // Compute new orientation
    (*cam)->horizontalAngle += (*cam)->mouseSpeed  * float(WIDTH/2 - xPos); //float(1024/2 - xpos );
    (*cam)->verticalAngle   += (*cam)->mouseSpeed  * float(HEIGHT/2 - yPos); //float( 768/2 - ypos );

    // Represents the Direction that you are pointing in, which also is used
    // to translate the cam forward and back too
    (*cam)->viewDir = glm::vec3(
                cos((*cam)->verticalAngle) * sin((*cam)->horizontalAngle),
                sin((*cam)->verticalAngle),
                cos((*cam)->verticalAngle) * cos((*cam)->horizontalAngle));

    // Right vector
    glm::vec3 right = glm::vec3(
        sin((*cam)->horizontalAngle - 3.14f/2.0f),
        0,
        cos((*cam)->horizontalAngle - 3.14f/2.0f)
    );

    right.y = -1.0f * calcRoll((*cam)->camVel, right);
    (*cam)->up = glm::cross(right, (*cam)->viewDir);

    if(keyboard_state_array[SDL_SCANCODE_LSHIFT])
        (*cam)->speedVal *= (*cam)->speedBoost;
    else
        (*cam)->speedVal = 0.025f;

    if (keyboard_state_array[SDL_SCANCODE_A])
        (*cam)->camVel -= (glm::vec3(right.x, 0.0f, right.z) * (*cam)->speedVal * deltaTime);

    if (keyboard_state_array[SDL_SCANCODE_D])
        (*cam)->camVel += glm::vec3(right.x, 0.0f, right.z) * (*cam)->speedVal * deltaTime;

    if (keyboard_state_array[SDL_SCANCODE_S])
        (*cam)->camVel -= ((*cam)->viewDir * (*cam)->speedVal * deltaTime);

    if (keyboard_state_array[SDL_SCANCODE_W])
        (*cam)->camVel += (*cam)->viewDir * (*cam)->speedVal * deltaTime;

    if(keyboard_state_array[SDL_SCANCODE_Q])
        (*cam)->camVel += glm::vec3(0.0,1.0,0.0) *(*cam)->speedVal * deltaTime;

    // not cam->up but absolute up
    if(keyboard_state_array[SDL_SCANCODE_Z])
        (*cam)->camVel -= glm::vec3(0.0,1.0f,0.0) *(*cam)->speedVal * deltaTime;

    (*cam)->camVel *= 0.95;
    (*cam)->pos += (*cam)->camVel;
    (*cam)->projection = CreateProjection(**cam);
}

    //return c;
