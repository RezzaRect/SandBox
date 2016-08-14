#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

enum class ProjectionType
{
	Perspective,
	InfinitePerspective,
	Orthographic,
};
// Could add view direction to camera struct
struct Camera
{
    float fov;
    float aspect;
    float zNear;
    float zFar;

    glm::mat4 projection;
	glm::vec3 pos;
	glm::vec3 forward;
	glm::vec3 up;
};

glm::mat4 CreateProjection(Camera& cam);//, glm::vec3& position

Camera InitCamera(float fov, float aspect, float zNear, float zFar,
                  glm::vec3 pos, glm::vec3 forward, glm::vec3 up);

glm::mat4 GetView(Camera& cam);
glm::mat4 GetViewProjection(Camera& cam);

void MoveForward(glm::vec3* p, float amount);
void MoveBackward(glm::vec3* p, float amount);
void MoveRight(glm::vec3* p, float amount);
void MoveLeft(glm::vec3* p, float amount);
void UpdateCam(Camera** cam, int WIDTH, int HEIGHT);
#endif // CAMERA_H_INCLUDED
