#ifndef TEXTURE_H
#define TEXTURE_H

#include "../core/referenceCounter.h"

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <map>

/*

class TextureData : public ReferenceCounter
{
public:
	TextureData(GLenum textureTarget, int width, int height, int numTextures, unsigned char** data, GLfloat* filters, GLenum* attachments);

	void Bind(int textureNum);
	void BindAsRenderTarget();

	virtual ~TextureData();

	//inline GLenum GetTextureTarget() { return m_textureTarget; }
	//inline GLuint GetTextureID() { return m_textureID[0]; }
protected:
private:
	TextureData(TextureData& other) {}
	void operator=(TextureData& other) {}

	void InitTextures(unsigned char** data, GLfloat* filters);
    void InitRenderTargets(GLenum* attachments);

	GLuint* m_textureID;
	GLenum m_textureTarget;
	GLuint m_frameBuffer;
	int m_numTextures;
	int m_width;
	int m_height;
};

class Texture
{
public:
	Texture(const std::string& fileName, GLenum textureTarget = GL_TEXTURE_2D, GLfloat filter = GL_LINEAR, GLenum attachment = GL_NONE);
	Texture(int width = 0, int height = 0, unsigned char* data = 0, GLenum textureTarget = GL_TEXTURE_2D, GLfloat filter = GL_LINEAR, GLenum attachment = GL_NONE);
	virtual ~Texture();

	void Bind(unsigned int unit = 0) const;
	void BindAsRenderTarget();
protected:
private:
	static std::map<std::string, TextureData*> s_resourceMap;

	Texture(Texture& texture) {}
	void operator=(Texture& texture) {}

	TextureData* m_textureData;
	std::string m_fileName;

};

#endif
*/

/*
#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include "referenceCounter.h"
// Reference: https://github.com/BennyQBD/ModernOpenGLTutorial

class Texture
{
public:
    Texture(const std::string& fileName);

    void Bind(unsigned int unit);
    void BindAsRenderTarget();

    std::string retStr;

    virtual~Texture();
protected:
private:
    Texture(const Texture& texture){}
    void operator=(const Texture& texture){}

    GLuint m_texture;

};

#endif // TEXTURE_H_INCLUDED
*/

class TextureData : public ReferenceCounter
{
public:
	TextureData(GLenum textureTarget, int width, int height, int numTextures, unsigned char** data, GLfloat* filters, GLenum* attachments);

	void Bind(int texureNum);
	void BindAsRenderTarget();

	virtual ~TextureData();
protected:
private:
    friend class Gbuffer;
	TextureData(TextureData& other) {}
	void operator=(TextureData& other) {}

	void InitTextures(unsigned char** data, GLfloat* filters);
	void InitRenderTargets(GLenum* attachments);

	GLuint* m_textureID;
	GLenum m_textureTarget;
	GLuint m_frameBuffer;
	GLuint m_renderBuffer;
	int m_numTextures;
	int m_width;
	int m_height;
};

class Texture
{
public:
	Texture(const std::string& fileName, GLenum textureTarget = GL_TEXTURE_2D, GLfloat filter = GL_LINEAR, GLenum attachment = GL_NONE);
	Texture(int width = 0, int height = 0, unsigned char* data = 0, GLenum textureTarget = GL_TEXTURE_2D, GLfloat filter = GL_LINEAR, GLenum attachment = GL_NONE);
	virtual ~Texture();

	void Bind(unsigned int unit = 0) const;
	void BindAsRenderTarget();
protected:
private:
	friend class Gbuffer;
	static std::map<std::string, TextureData*> s_resourceMap;

	Texture(Texture& texture) {}
	void operator=(Texture& texture) {}

	TextureData* m_textureData;
	std::string m_fileName;

};

#endif
