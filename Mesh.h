#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "obj_loader.h"

// Reference: https://github.com/BennyQBD/ModernOpenGLTutorial

class Vertex
{
public:
    Vertex(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& normal = glm::vec3(0, 0, 0))
    {
        this->pos = pos;
        this->texCoord = texCoord;
        this->normal = normal;
    }

    inline glm::vec3* GetPos(){ return &pos; }
    inline glm::vec2* GetTexCoord(){ return &texCoord; }
    inline glm::vec3* GetNormal() { return &normal; }

protected:
private:
    glm::vec3 pos;
    glm::vec2 texCoord;
    glm::vec3 normal;
};
//inline glm::vec3* GetPos(){ return &pos; }
class Mesh
{
public:
    Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
    Mesh(const std::string& fileName);
    Mesh(IndexedModel& model);

    void Draw();

    virtual ~Mesh();
protected:
private:
    Mesh(const Mesh& other);
    void operator=(const Mesh& other);

    void InitMesh(const IndexedModel& model);

    enum
    {
        POSITION_VB,
        TEXCOORD_VB,
        NORMAL_VB,

        INDEX_VB,

        NUM_BUFFERS
    };
    GLuint m_vertexArrayObject;
    GLuint m_vertexArrayBuffers[NUM_BUFFERS];
    unsigned int m_drawCount;
};

std::vector<IndexedModel> loadModelAssimp(const std::string& fileName);

//std::vector<Mesh> loadModelAssimp(const std::string& fileName);


#endif // MESH_H_INCLUDED
