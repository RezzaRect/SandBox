#ifndef TRANSFORM_H_INCLUDED
#define TRANSFORM_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"

// Reference: https://github.com/BennyQBD/ModernOpenGLTutorial

struct Transform
{
public:
	Transform(const glm::vec3& pos = glm::vec3(), const glm::mat4& rot = glm::mat4(), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		this->pos = pos;
		this->rot = rot;
		this->scale = scale;
	}

	inline glm::mat4 GetModel() const
	{
		glm::mat4 posMat = glm::translate(pos);
		glm::mat4 scaleMat = glm::scale(scale);
		// not sure if the inverse or transpose is better to use here
		glm::mat4 rotMat = glm::inverse(rot);

		return posMat * rotMat * scaleMat;
	}
    // was GetMVP(const Camera& camera)
	inline glm::mat4 GetMVP(Camera& camera) //const
	{
		glm::mat4 VP = GetViewProjection(camera); //camera.GetViewProjection();
		glm::mat4 M = GetModel();

		return VP * M;//camera.GetViewProjection() * GetModel();
	}

	inline glm::vec3* GetPos() { return &pos; }
	inline glm::mat4* GetRot() { return &rot; }
	inline glm::vec3* GetScale() { return &scale; }

	inline void SetPos(glm::vec3& pos) { this->pos = pos; }
	inline void SetRot(glm::mat4& rot) { this->rot = rot; }
	inline void SetScale(glm::vec3& scale) { this->scale = scale; }
protected:
private:
	glm::vec3 pos;
	glm::mat4 rot;
	glm::vec3 scale;
};

#endif // TRANSFORM_H_INCLUDED
