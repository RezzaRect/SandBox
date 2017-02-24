#ifndef PARTICLE_EMITTER_H_INCLUDED
#define PARTICLE_EMITTER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include <SDL.h>

#include "Camera.h"

//#define MaxParticles 15000


class ParticleEmitter
{
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
public:
    ParticleEmitter(int numParticles, glm::vec3& spawnPoint, glm::ivec3& color, int particlesPerFrame);

    void SortParticles();

    int FindUnusedParticle();

    void UpdateParticles(Camera** cam, float delta1);

    void RenderParticles();
protected:

private:
    GLuint m_billboard_vertex_buffer;
    GLuint m_particles_position_buffer;
    GLuint m_particles_color_buffer;

    //const int MaxParticles = 10000;
    Particle* m_ParticlesContainer; //[MaxParticles];

    int m_LastUsedParticle = 0;
    int m_ParticlesCount = 0;
    int m_total;
    int m_particlesPerFrame;

    GLfloat* m_g_particule_position_size_data; // = new GLfloat[MaxParticles * 4];
	GLubyte* m_g_particule_color_data;         //  = new GLubyte[MaxParticles * 4];

    glm::vec3 m_spawnPoint;
    glm::vec3 m_spawnDir;
    glm::vec3 m_color;
};

#endif // PARTICLE_EMITTER_H_INCLUDED
