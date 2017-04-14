#ifndef SHADER_H
#define SHADER_H

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include "../core/Camera.h"
#include "../core/Transform.h"
// Reference: https://github.com/BennyQBD/ModernOpenGLTutorial

class Shader
{
    public:
        Shader(const std::string& fileName);

        void Bind();
        void Update(const Transform& transform, Camera& camera);

        virtual ~Shader();
        GLuint m_program;
    protected:

    private:
        static const unsigned int NUM_SHADERS = 2;
        static const unsigned int NUM_UNIFORMS = 16;
        //Shader(const Shader& other){}
        //void Shader& operator=(const Shader& other){}


        GLuint m_shaders[NUM_SHADERS];
        GLuint m_uniforms[NUM_UNIFORMS];
};

#endif // SHADER_H
