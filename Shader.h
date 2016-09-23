#ifndef SHADER_H
#define SHADER_H

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include "Camera.h"
#include "Transform.h"

// Reference: https://github.com/BennyQBD/ModernOpenGLTutorial

class Shader
{
    public:
        Shader(const std::string& fileName);

        void Bind();
        void Update(const Transform& transform, Camera& camera);

        virtual ~Shader();
    protected:

    private:
        static const unsigned int NUM_SHADERS = 2;
        static const unsigned int NUM_UNIFORMS = 10;
        //Shader(const Shader& other){}
        //void Shader& operator=(const Shader& other){}

        GLuint m_program;
        GLuint m_shaders[NUM_SHADERS];
        GLuint m_uniforms[NUM_UNIFORMS];
};

#endif // SHADER_H
