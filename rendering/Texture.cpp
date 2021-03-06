#include "Texture.h"
#include "../staticLibs/stb_image.h"

#include <iostream>
#include <cassert>

std::map<std::string, TextureData*> Texture::s_resourceMap;

TextureData::TextureData(GLenum textureTarget, int width, int height, int numTextures, unsigned char** data, GLfloat* filters, GLenum* attachments)
{
    m_textureID = new GLuint[numTextures];
	m_textureTarget = textureTarget;
	m_numTextures = numTextures;
	m_width = width;
	m_height = height;
	m_frameBuffer = 0;
	m_renderBuffer = 0;

	InitTextures(data, filters);
	InitRenderTargets(attachments);

}

TextureData::~TextureData()
{
	if(*m_textureID) glDeleteTextures(m_numTextures, m_textureID);
	if(m_frameBuffer) glDeleteFramebuffers(1, &m_frameBuffer);
	if(m_renderBuffer) glDeleteRenderbuffers(1, &m_renderBuffer);
	if(m_textureID) delete[] m_textureID;
}

void TextureData::InitTextures(unsigned char** data, GLfloat* filters){

    glGenTextures(m_numTextures, m_textureID);
    for(int i = 0; i < m_numTextures; i++){
        glBindTexture(m_textureTarget, m_textureID[i]);

        glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, filters[i]);
        glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, filters[i]);

        glTexImage2D(m_textureTarget, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
    }
}
//m_textureData = new TextureData(textureTarget, width, height, 1, &data, &filter);
void TextureData::InitRenderTargets(GLenum* attachments){
    if(attachments == 0)
        return;

    GLenum drawBuffers[m_numTextures];
    bool hasDepth = false;

    for(int i = 0; i < m_numTextures; i++){

        if(attachments[i] == GL_DEPTH_ATTACHMENT){ // || GL_STENCIL_ATTACHMENT
            drawBuffers[i] = GL_NONE;
            hasDepth = true;
        }
        else
            drawBuffers[i] = attachments[i];

        if(attachments[i] == GL_NONE)
            continue;

        if(m_frameBuffer == 0){
            glGenFramebuffers(1, &m_frameBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBuffer);
        }

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachments[i], m_textureTarget, m_textureID[i], 0);
    }

    if(m_frameBuffer == 0)
        return;

    if(!hasDepth){
        glGenRenderbuffers(1, &m_renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);
    }

    glDrawBuffers(m_numTextures, drawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "Framebuffer creation did not complete!" << std::endl;
        assert(false);
    }
}

void TextureData::Bind(int textureNum){
    glBindTexture(m_textureTarget, m_textureID[textureNum]);
}

void TextureData::BindAsRenderTarget(){
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBuffer);
    glViewport(0, 0, m_width, m_height);
}

Texture::Texture(const std::string& fileName, GLenum textureTarget, GLfloat filter, GLenum attachment)
{
 	m_fileName = fileName;

	std::map<std::string, TextureData*>::const_iterator it = s_resourceMap.find(fileName);
	if(it != s_resourceMap.end())
	{
		m_textureData = it->second;
		m_textureData->AddReference();
	}
	else
	{
		int x, y, bytesPerPixel;
		//"./res/textures/" +
		unsigned char* data = stbi_load((fileName).c_str(), &x, &y, &bytesPerPixel, 4);

		if(data == NULL)
		{
			std::cerr << "Unable to load texture: " << fileName << std::endl;
		}

		m_textureData = new TextureData(textureTarget, x, y, 1, &data, &filter, &attachment);
		stbi_image_free(data);

		s_resourceMap.insert(std::pair<std::string, TextureData*>(fileName, m_textureData));
	}
}

Texture::Texture(int width, int height, unsigned char* data, GLenum textureTarget, GLfloat filter, GLenum attachment)
{
	m_fileName = "";
	m_textureData = new TextureData(textureTarget, width, height, 1, &data, &filter, &attachment);
}

Texture::~Texture()
{
	if(m_textureData && m_textureData->RemoveReference())
	{
		if(m_fileName.length() > 0)
			s_resourceMap.erase(m_fileName);

		delete m_textureData;
	}
}

void Texture::Bind(unsigned int unit) const
{
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
	m_textureData->Bind(0);
}

void Texture::BindAsRenderTarget(){
    m_textureData->BindAsRenderTarget();
}
void setupDeferredRendering(GLuint& gBuffer, GLuint& gPosition, GLuint& gNormal, GLuint& gAlbedoSpec, int WIDTH, int HEIGHT){

    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    //GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    //glDrawBuffers(3, attachments);
    ///////////////////////////////////////////
    ///////////////////////////////////////////

    // Set up G-Buffer
	// 3 textures:
	// 1. Positions (RGB)
	// 2. Color (RGB) + Specular (A)
	// 3. Normals (RGB)
	//GLuint gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    //GLuint gPosition, gNormal, gAlbedoSpec;
    // - Position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // - Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // - Color + Specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
	// - Create and attach depth buffer (renderbuffer)
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.4f, 0.69f, 1.0f);
}

// Reference: https://learnopengl.com/#!Advanced-OpenGL/Cubemaps
GLuint loadCubemap(std::vector<std::string> faces){
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);


    int x, y, bytesPerPixel;
    unsigned char* data;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < faces.size(); i++)
    {
        data = stbi_load(faces[i].c_str(), &x, &y, &bytesPerPixel, 3);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        // if the last parameter 'int req_comp' is 3 GL_RGB must be used
        // if the last parameter 'int req_comp' is 4 GL_RGBA must be used

        //data = stbi_load(faces[i].c_str(), &x, &y, &bytesPerPixel, 4);
        //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    stbi_image_free(data);

    return textureID;
}

