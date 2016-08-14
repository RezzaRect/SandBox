#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

class Texture
{
public:
    Texture(const std::string& fileName);

    void Bind(unsigned int unit);

    virtual~Texture();
protected:
private:
    Texture(const Texture& texture){}
    void operator=(const Texture& texture){}

    GLuint m_texture;
};

#endif // TEXTURE_H_INCLUDED
