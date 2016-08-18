#ifndef PARTICLES_H_INCLUDED
#define PARTICLES_H_INCLUDED

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "Camera.h"

#define MAX_PARTICLES 5000

// CPU representation of a particle
struct Particle{
	glm::vec3 pos, speed;
	unsigned char r,g,b,a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

void InitParticles(float life, float camDistance);
int FindUnusedParticle();
void SortParticles();
void UpdateParicles(float deltaTime, Camera** cam);

#endif // PARTICLES_H_INCLUDED
