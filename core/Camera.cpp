#include "Camera.h"

#include <SDL.h>
#include <glm/gtx/rotate_vector.hpp>

const glm::vec3 UP(0.0, 1.0f, 0.0);
const glm::vec3 RIGHT(0.0f, 0.0f, 1.0f);

glm::mat4 CreateProjection(Camera& cam){
    return glm::perspective(cam.fov, cam.aspect, cam.zNear, cam.zFar);
}

Camera InitCamera(float fov, float aspect, float zNear, float zFar,
                  glm::vec3 pos, glm::vec3 viewDir, glm::vec3 up)
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
    return cam;
}

glm::mat4 GetView(Camera& cam){
    return glm::lookAt(cam.pos, cam.pos + cam.viewDir, cam.up);
}

glm::mat4 GetViewProjection(Camera& cam){
    return cam.projection * glm::lookAt(cam.pos, cam.pos + cam.viewDir, cam.up);
}

// horizontal angle : toward -Z
float horizontalAngle = 0.0f; //3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
float inc = 1.0f;
// http://stackoverflow.com/questions/1252976/how-to-handle-multiple-keypresses-at-once-with-sdl
    glm::vec3 camVel = glm::vec3(0.0, 0.0, 0.0);
// Reference: https://github.com/id-Software/Quake/blob/master/WinQuake/view.c
// line: 81
float calcRoll(glm::vec3& velVec, glm::vec3& camRight){
    //float roll = 0.0f;


    //float glm::clamp()
    //camRight = glm::normalize(camRight);

    //roll = glm::dot(velVec, camRight);

    //return roll;
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

void UpdateCam(Camera** cam, int WIDTH, int HEIGHT, float deltaTime){
    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);
    SDL_Event event;
    SDL_PollEvent(&event);

    //Camera c = cam;
    //c.pos.x += 0.001f;

    int xPos, yPos;
    SDL_GetMouseState(&xPos, &yPos);
    SDL_WarpMouseInWindow(NULL, WIDTH/2, HEIGHT/2);


    float speed = 0.025; //3.0f; // 3 units / second
    float mouseSpeed = 0.005f;


    // Compute new orientation
    horizontalAngle += mouseSpeed  * float(WIDTH/2 - xPos); //float(1024/2 - xpos );
    verticalAngle   += mouseSpeed  * float(HEIGHT/2 - yPos); //float( 768/2 - ypos );

    // Represents the Direction that you are pointing in, which also is used
    // to translate the cam forward and back too
    (*cam)->viewDir = glm::vec3(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle));

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );

    //glm::mat4 rotMat(1);
    //rotMat = glm::rotate(rotMat, calcRoll(camVel), glm::vec4(0,1,0));
    //glm::mat4 rotationMat(1); // Creates a identity matrix
    //Now your can rotate the vectorspacec with

    //rotationMat = glm::rotate(rotationMat, calcRoll(camVel, right), glm::vec3(0.0, 1.0, 0.0));
    //glm::mat3 mat4Tomat3 = glm::mat3(rotationMat);
    //right = right * mat4Tomat3;

    right.y = -1.0f * calcRoll((*cam)->camVel, right);

    // "CAM.FORWARD IS THE DIRECTION HERE"
    (*cam)->up = glm::cross(right, (*cam)->viewDir);


    if(keyboard_state_array[SDL_SCANCODE_LSHIFT])
        speed *= 2.0f;
    else
        speed = 0.025f;

    if (keyboard_state_array[SDL_SCANCODE_A])
        (*cam)->camVel -= (glm::vec3(right.x, 0.0f, right.z) * speed * deltaTime);
        //(*cam)->pos -= right * speed * deltaTime; //0.005f;


    if (keyboard_state_array[SDL_SCANCODE_D])
        (*cam)->camVel += glm::vec3(right.x, 0.0f, right.z) * speed * deltaTime;
        //(*cam)->pos += right * speed * deltaTime; //0.005f;


    if (keyboard_state_array[SDL_SCANCODE_S])
        (*cam)->camVel -= ((*cam)->viewDir * speed * deltaTime);
        //(*cam)->pos -= (*cam)->viewDir * speed * deltaTime; //0.005f;


    if (keyboard_state_array[SDL_SCANCODE_W])
        (*cam)->camVel += (*cam)->viewDir * speed * deltaTime;
        //(*cam)->pos += (*cam)->viewDir * speed * deltaTime; //0.005f;

    // not cam up but absolute up
    if(keyboard_state_array[SDL_SCANCODE_Q])
        (*cam)->camVel += glm::vec3(0.0,1.0,0.0) *speed * deltaTime;
        //(*cam)->pos += glm::vec3(0.0,1.0,0.0) *speed * deltaTime;

    // not cam->up but absolute up
    //inc += 0.01;
    if(keyboard_state_array[SDL_SCANCODE_Z])
        (*cam)->camVel -= glm::vec3(0.0,1.0f,0.0) *speed * deltaTime;
        //(*cam)->pos -= glm::vec3(0.0,1.0,0.0) *speed * deltaTime;


    (*cam)->camVel *= 0.95;

    (*cam)->pos += (*cam)->camVel;

    (*cam)->projection = CreateProjection(**cam);
    //return c;
}

//Vertex(glm::vec3(-.5, -.5, .0)),
//CameraQuatBothPitchAndYaw = glm::cross(CameraQuatPitch, CameraQuatYaw); CameraQuatBothPitchAndYaw = glm::normalize(CameraQuatBothPitchAndYaw); DirectionOfWhereCameraIsFacing = glm::rotate(CameraQuatBothPitchAndYaw, DirectionOfWhereCameraIsFacing);

/*
void MoveForward(glm::vec3* p, float amount){
    *p += (RIGHT * amount); // * deltaTime..?
}

void MoveBackward(glm::vec3* p, float amount){
    *p -= (RIGHT * amount);
}

void MoveRight(glm::vec3* p, float amount){
    *p += (glm::cross(UP, RIGHT) * amount);
}

void MoveLeft(glm::vec3* p, float amount){
    *p -= (glm::cross(UP, RIGHT) * amount);
}
*/
