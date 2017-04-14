#include "Shader.h"

#include <SDL.h>
#include <fstream>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>
static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
static std::string LoadShader(const std::string& fileName);
static GLuint CreateShader(const std::string& text, GLenum shaderType);
glm::vec4 dLight = glm::normalize(glm::vec4(0.6,-0.3,0.4,1.0));
Shader::Shader(const std::string& fileName)
{
    //ctor
    m_program = glCreateProgram();
    m_shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);
    m_shaders[1] = CreateShader(LoadShader(fileName + ".fs"), GL_FRAGMENT_SHADER);

    for(unsigned int i = 0; i< NUM_SHADERS; i++)
        glAttachShader(m_program, m_shaders[i]);

    glBindAttribLocation(m_program, 0, "position");
    glBindAttribLocation(m_program, 1, "texCoord");
    glBindAttribLocation(m_program, 2, "normal");
    glBindAttribLocation(m_program, 3, "tangent");
    glBindAttribLocation(m_program, 4, "bitangent");

    glLinkProgram(m_program);
    CheckShaderError(m_program, GL_LINK_STATUS, true, "Error: Program failed to link");

    glValidateProgram(m_program);
    CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "Error: Program is invalid");

	m_uniforms[0] = glGetUniformLocation(m_program, "model");
	m_uniforms[1] = glGetUniformLocation(m_program, "view");
	m_uniforms[2] = glGetUniformLocation(m_program, "projection");
	m_uniforms[3] = glGetUniformLocation(m_program, "viewPos");
	m_uniforms[4] = glGetUniformLocation(m_program, "CameraRight_worldspace");
	m_uniforms[5] = glGetUniformLocation(m_program, "CameraUp_worldspace");
	m_uniforms[6] = glGetUniformLocation(m_program, "diffuse");
	//m_uniforms[7] = glGetUniformLocation(m_program, "specularTexture");
	m_uniforms[8] = glGetUniformLocation(m_program, "time");
	m_uniforms[9] = glGetUniformLocation(m_program, "normalMap");
    m_uniforms[10] = glGetUniformLocation(m_program, "camDir");
    m_uniforms[11] = glGetUniformLocation(m_program, "lightPos");
}

Shader::~Shader()
{
    for(unsigned int i = 0; i< NUM_SHADERS; i++){
        glDetachShader(m_program, m_shaders[i]);
        glDeleteShader(m_shaders[i]);
    }

    glDeleteProgram(m_program);
}

void Shader::Bind(){
    glUseProgram(m_program);
}

void Shader::Update(const Transform& transform, Camera& camera){

    glm::mat4 viewProj = GetView(camera);   //GetViewProjection
    glm::mat4 model = transform.GetModel();
    float t = SDL_GetTicks()/1000.0;
    //rotate normal vector glm opengl.org
    dLight = normalize(glm::rotate(glm::mat4(),t/2,glm::vec3(1.0f+sin(t/3),1.0f-cos(t/2),0))*glm::vec4 (0,0,1,0));

    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);
    SDL_Event event;
    SDL_PollEvent(&event);
    if (keyboard_state_array[SDL_SCANCODE_L])
        camera.viewDir = glm::vec3(dLight);

    glUniformMatrix4fv(m_uniforms[0], 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(m_uniforms[1], 1, GL_FALSE, &viewProj[0][0]);
    glUniformMatrix4fv(m_uniforms[2], 1, GL_FALSE, &camera.projection[0][0]);
    glUniform3f(m_uniforms[3], camera.pos.x, camera.pos.y, camera.pos.z);
    glUniform3f(m_uniforms[4], viewProj[0][0], viewProj[1][0], viewProj[2][0]);
    glUniform3f(m_uniforms[5], viewProj[0][1], viewProj[1][1], viewProj[2][1]);
    glUniform1i(m_uniforms[6], 0);

    glUniform1f(m_uniforms[8], t);
    glUniform1i(m_uniforms[9], 1);
    glUniform3f(m_uniforms[10], camera.viewDir.x, camera.viewDir.y, camera.viewDir.z);
    glUniform3f(m_uniforms[11], dLight.x, dLight.y, dLight.z);
    //Release 16.01  rev 10702 (2016-01-25 19:50:14)   gcc 4.9.2 Windows/unicode - 32 bit
}

static GLuint CreateShader(const std::string& text, GLenum shaderType){
    GLuint shader = glCreateShader(shaderType);

    if(shader == 0)
        std::cerr << "Error: Shader creation failed!" << std::endl;

    const GLchar* shaderSourceStrings[1];
    GLint shaderSourceStringsLengths[1];

    shaderSourceStrings[0] = text.c_str();
    shaderSourceStringsLengths[0] = text.length();

    glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringsLengths);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation failed: ");


    return shader;
}

static std::string LoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
			output.append(line + "\n");
        }
    }
    else
    {
		std::cerr << "Unable to load shader: " << fileName << std::endl;
    }

    return output;
}

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if(isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if(success == GL_FALSE)
    {
        if(isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}
