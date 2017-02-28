#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
#include <glm/glm.hpp>
//#include "obj_loader.h"

// Reference: https://github.com/BennyQBD/ModernOpenGLTutorial

class IndexedModel
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<unsigned int> indices;

    void CalcNormals();
    void ComputeTangetBasis();
};

class Vertex
{
public:
    Vertex(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& normal = glm::vec3(0, 0, 0),
           const glm::vec3& tangent = glm::vec3(0, 0, 0), const glm::vec3& bitangent = glm::vec3(0, 0, 0))
    {
        this->pos = pos;
        this->texCoord = texCoord;
        this->normal = normal;
        this->tangent = tangent;
        this->bitangent = bitangent;
    }

    inline glm::vec3* GetPos(){ return &pos; }
    inline glm::vec2* GetTexCoord(){ return &texCoord; }
    inline glm::vec3* GetNormal() { return &normal; }
    inline glm::vec3* GetTangent() { return &tangent; }
    inline glm::vec3* GetBiTangent() { return &bitangent; }

protected:
private:
    glm::vec3 pos;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};
//inline glm::vec3* GetPos(){ return &pos; }
class Mesh
{
public:
    Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
    Mesh(const std::string& fileName);
    Mesh(IndexedModel& model);

    void Draw();

    Mesh(const Mesh& other);
    virtual ~Mesh();
protected:
private:

    void operator=(const Mesh& other);

    void InitMesh(const IndexedModel& model);

    enum
    {
        POSITION_VB,
        TEXCOORD_VB,
        NORMAL_VB,
        INDEX_VB,
        TANGENT_VB,
        BITANGENT_VB,

        NUM_BUFFERS
    };
    GLuint m_vertexArrayObject;
    GLuint m_vertexArrayBuffers[NUM_BUFFERS];
    unsigned int m_drawCount;
};

std::vector<IndexedModel> loadModelAssimp(const std::string& fileName);

//std::vector<Mesh> loadModelAssimp(const std::string& fileName);


#endif // MESH_H_INCLUDED
