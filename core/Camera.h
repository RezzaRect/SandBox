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

    float horizontalAngle;
    float verticalAngle;
    float speedVal;
    float speedBoost;
    float mouseSpeed;

    glm::vec3 camVel;
    glm::mat4 projection;
	glm::vec3 pos;
	glm::vec3 viewDir;
	glm::vec3 up;
};

glm::mat4 CreateProjection(Camera& cam);//, glm::vec3& position

Camera InitCamera(float fov, float aspect, float zNear, float zFar, glm::vec3 pos,
                  glm::vec3 viewDir, glm::vec3 up, float camSpeed, float camBoost, float mouseSen);

glm::mat4 GetView(Camera& cam);
glm::mat4 GetViewProjection(Camera& cam);
float calcRoll(glm::vec3 velVec);
void UpdateCam(Camera** cam, int WIDTH, int HEIGHT, float deltaTime);

#endif // CAMERA_H_INCLUDED
