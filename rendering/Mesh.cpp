#include "Mesh.h"
//#include "obj_loader.h"
#include <vector>
#include <stdio.h>

//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

void IndexedModel::CalcNormals()
{
    normals.clear();
    normals.reserve(positions.size());

    for(unsigned int i = 0; i < positions.size(); i++)
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

    for(unsigned int i = 0; i < indices.size(); i += 3)
    {
        int i0 = indices[i];
        int i1 = indices[i + 1];
        int i2 = indices[i + 2];

        glm::vec3 v1 = positions[i1] - positions[i0];
        glm::vec3 v2 = positions[i2] - positions[i0];

        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }

    for(unsigned int i = 0; i < positions.size(); i++)
        normals[i] = glm::normalize(normals[i]);
}

void IndexedModel::ComputeTangetBasis()
{
	tangents.clear();
	tangents.reserve(positions.size());

	for(unsigned int i = 0; i < positions.size(); i++)
		tangents.push_back(glm::vec3(0,0,0));

	for(unsigned int i = 0; i < indices.size(); i += 3)
    {
		int i0 = indices[i];
		int i1 = indices[i + 1];
		int i2 = indices[i + 2];

        glm::vec3 edge1 = positions[i1] - positions[i0];
        glm::vec3 edge2 = positions[i2] - positions[i0];

        float deltaU1 = texCoords[i1].x - texCoords[i0].x;
        float deltaU2 = texCoords[i2].x - texCoords[i0].x;
        float deltaV1 = texCoords[i1].y - texCoords[i0].y;
        float deltaV2 = texCoords[i2].y - texCoords[i0].y;

        float dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
        float f = dividend == 0.0f ? 0.0f : 1.0f/dividend;

        glm::vec3 tangent = glm::vec3(0,0,0);

        tangent.x = (f * (deltaV2 * edge1.x - deltaV1 * edge2.x));
        tangent.y = (f * (deltaV2 * edge1.y - deltaV1 * edge2.y));
        tangent.z = (f * (deltaV2 * edge1.z - deltaV1 * edge2.z));

//Bitangent example, in Java
//		Vector3f bitangent = new Vector3f(0,0,0);
//
//		bitangent.setX(f * (-deltaU2 * edge1.getX() - deltaU1 * edge2.getX()));
//		bitangent.setX(f * (-deltaU2 * edge1.getY() - deltaU1 * edge2.getY()));
//		bitangent.setX(f * (-deltaU2 * edge1.getZ() - deltaU1 * edge2.getZ()));

		tangents[i0] += tangent;
		tangents[i1] += tangent;
		tangents[i2] += tangent;
		//tangents.push_back(tangent);
    }

    for(unsigned int i = 0; i < tangents.size(); i++)
		tangents[i] = glm::normalize(tangents[i]);
}


Mesh::Mesh(IndexedModel& model){
    model.ComputeTangetBasis();
    InitMesh(model);
}

Mesh::Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices){
    //m_drawCount = numVertices;

    IndexedModel model;

    for(unsigned int i = 0; i < numVertices; i++){
        model.positions.push_back(*vertices[i].GetPos());
        model.texCoords.push_back(*vertices[i].GetTexCoord());
        model.normals.push_back(*vertices[i].GetNormal());
        //model.tangents.push_back(*vertices[i].GetTangent());
        //model.bitangents.push_back(*vertices[i].GetBiTangent());
    }

    for(unsigned int i = 0; i < numIndices; i++)
        model.indices.push_back(indices[i]);

    //model.ComputeTangetBasis();

    InitMesh(model);
}

Mesh::~Mesh(){
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void Mesh::InitMesh(const IndexedModel& model){

    m_drawCount = model.indices.size();

    glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(model.normals[0]), &model.normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TANGENT_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.tangents.size() * sizeof(model.tangents[0]), &model.tangents[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW);

}

void Mesh::Draw(){
    glBindVertexArray(m_vertexArrayObject);

    glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, m_drawCount);

    glBindVertexArray(0);
}

