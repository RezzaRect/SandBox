#include "Particle_Emitter.h"

#include "../stb_perlin.h"

ParticleEmitter::ParticleEmitter(int numParticles, glm::vec3& spawnPoint, glm::ivec3& color, int particlesPerFrame){
    GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    m_g_particule_position_size_data = new GLfloat[numParticles * 4];
	m_g_particule_color_data         = new GLubyte[numParticles * 4];
	m_ParticlesContainer             = new Particle[numParticles];

    m_spawnPoint = spawnPoint;
    m_total = numParticles;
    m_color = color;
    m_particlesPerFrame = particlesPerFrame;

	for(int i=0; i<numParticles; i++){
		m_ParticlesContainer[i].life = -1.0f;
		m_ParticlesContainer[i].cameradistance = -1.0f;
	}

	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const GLfloat g_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};
	//GLuint billboard_vertex_buffer;
	glGenBuffers(1, &m_billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	//GLuint particles_position_buffer;
	glGenBuffers(1, &m_particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, numParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	//GLuint particles_color_buffer;
	glGenBuffers(1, &m_particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, numParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

}

void ParticleEmitter::SortParticles(){
    std::sort(&m_ParticlesContainer[0], &m_ParticlesContainer[m_total]);
}

int ParticleEmitter::FindUnusedParticle(){

    for(int i=m_LastUsedParticle; i<m_total; i++){
		if (m_ParticlesContainer[i].life < 0){
			m_LastUsedParticle = i;
			return i;
		}
	}

	for(int i=0; i<m_LastUsedParticle; i++){
		if (m_ParticlesContainer[i].life < 0){
			m_LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void ParticleEmitter::UpdateParticles(Camera** cam, float delta){
    // Clear the screen
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //double currentTime = glfwGetTime();
    //double delta = currentTime - lastTime;
    //lastTime = currentTime;


    //computeMatricesFromInputs();
    glm::mat4 ProjectionMatrix = (*cam)->projection; //getProjectionMatrix();
    glm::mat4 ViewMatrix = GetView(**cam); //getViewMatrix();

    // We will need the camera's position in order to sort the particles
    // w.r.t the camera's distance.
    // There should be a getCameraPosition() function in common/controls.cpp,
    // but this works too.
    glm::vec3 CameraPosition = (*cam)->pos; //(glm::inverse(ViewMatrix)[3]);

    //glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

    float time = SDL_GetTicks() / 1000.0f;
    // Generate 10 new particule each millisecond,
    // but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
    // newparticles will be huge and the next frame even longer.
    int newparticles = (int)(delta*10000.0);
    if (newparticles > (int)(0.016f*10000.0))
        newparticles = (int)(0.016f*10000.0);

    for(int i = 0; i < m_particlesPerFrame; i++){
        int particleIndex = FindUnusedParticle();
        m_ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.m_spawnPointglm::vec3(0.0f ,-10.0f,0.0f);
        m_ParticlesContainer[particleIndex].pos =  glm::vec3((rand()%2000 - 1000.0f)/25.0f, 30.0f,
                                                             (rand()%2000 - 1000.0f)/25.0f);  //
        //m_ParticlesContainer[particleIndex].pos.x-=9.0f;
        float spread = 1.5f;
        glm::vec3 maindir = glm::vec3(3.0f, -1.0f, -5.0f);
        // Very bad way to generate a random direction;
        // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
        // combined with some user-controlled parameters (main direction, spread, etc)

        float noise = stb_perlin_noise3(time, time/2.0f, time*1.4f, 0, 0, 0);

        glm::vec3 randomdir = glm::vec3(
            (rand()%2000 - 1000.0f)/1000.0f+sin(noise-time)*1.5f,
            (rand()%2000 - 1000.0f)/1000.0f-sin(noise+time)*1.75f,
            (rand()%2000 - 1000.0f)/1000.0f-cos(noise*time)*2.0f
        );

        m_ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;


        // Very bad way to generate a random color
        m_ParticlesContainer[particleIndex].r = m_color.x;  //rand() % 256;
        m_ParticlesContainer[particleIndex].g = m_color.y;  //rand() % 256;
        m_ParticlesContainer[particleIndex].b = m_color.z;  //rand() % 256;
        m_ParticlesContainer[particleIndex].a = (rand() % 128) + 128;

        m_ParticlesContainer[particleIndex].size = (rand()%1000)/2000.0f + 0.1f;

    }



    // Simulate all particles
    m_ParticlesCount = 0;
    for(int i=0; i<m_total; i++){

        Particle& p = m_ParticlesContainer[i]; // shortcut

        if(p.life > 0.0f){

            // Decrease life
            p.life -= delta;
            if (p.life > 0.0f){

                // Simulate simple physics : gravity only, no collisions
                p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
                p.pos += p.speed * (float)delta;
                p.cameradistance = glm::length2( p.pos - CameraPosition );
                //ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

                // Fill the GPU buffer
                m_g_particule_position_size_data[4*m_ParticlesCount+0] = p.pos.x;
                m_g_particule_position_size_data[4*m_ParticlesCount+1] = p.pos.y;
                m_g_particule_position_size_data[4*m_ParticlesCount+2] = p.pos.z;

                m_g_particule_position_size_data[4*m_ParticlesCount+3] = p.size;

                m_g_particule_color_data[4*m_ParticlesCount+0] = p.r;
                m_g_particule_color_data[4*m_ParticlesCount+1] = p.g;
                m_g_particule_color_data[4*m_ParticlesCount+2] = p.b;
                m_g_particule_color_data[4*m_ParticlesCount+3] = p.a;

            }else{
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.cameradistance = -1.0f;
            }
            m_ParticlesCount++;
        }
    }
    SortParticles();
}

void ParticleEmitter::RenderParticles(){
    // Update the buffers that OpenGL uses for rendering.
    // There are much more sophisticated means to stream data from the CPU to the GPU,
    // but this is outside the scope of this tutorial.
    // http://www.opengl.org/wiki/Buffer_Object_Streaming


    glBindBuffer(GL_ARRAY_BUFFER, m_particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_total * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_ParticlesCount * sizeof(GLfloat) * 4, m_g_particule_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, m_particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_total * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_ParticlesCount * sizeof(GLubyte) * 4, m_g_particule_color_data);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use our shader
    //glUseProgram(programID);

    // Bind our texture in Texture Unit 0
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    //glUniform1i(TextureID, 0);

    // Same as the billboards tutorial
    //glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    //glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

    //glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_billboard_vertex_buffer);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_particles_position_buffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        4,                                // size : x + y + z + size => 4
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, m_particles_color_buffer);
    glVertexAttribPointer(
        2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        4,                                // size : r + g + b + a => 4
        GL_UNSIGNED_BYTE,                 // type
        GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

    // Draw the particules !
    // This draws many times a small triangle_strip (which looks like a quad).
    // This is equivalent to :
    // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
    // but faster.
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_ParticlesCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
