#include "Mesh.h"
#include "obj_loader.h"
#include <vector>
#include <stdio.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::vector<IndexedModel> loadModelAssimp(const std::string& fileName)
{
    //m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;
    std::vector<IndexedModel> meshes;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(("./res/models/" + fileName).c_str(),
                                             aiProcess_Triangulate |
                                             aiProcess_GenSmoothNormals |  //);
                                             //aiProcess_GenNormals |
                                             aiProcess_FlipUVs);

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    int numMeshes = scene->mNumMeshes;
    //int numIndices = scene->
    //scene->

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    if(scene->HasAnimations()){
        printf("Model contains animations\n");
        printf("Model has %d number of animations", scene->mNumAnimations);
    }
    else
        printf("Model contains NO animations\n\n");

    for(int i = 0; i < numMeshes; i++){
        const aiMesh* paiMesh = scene->mMeshes[i];
        IndexedModel tempPushBackModel;
        //int numIndices = paiMesh->mNumFaces;
        printf("Mesh %d contains %d vertices \n", i, paiMesh->mNumVertices);
        for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {

            const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
            const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
            const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

            tempPushBackModel.positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));// (),
            tempPushBackModel.texCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));  //Vector2f(),
            tempPushBackModel.normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z)); //Vector3f());

            //Vertices.push_back(v);
        }

        for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
            const aiFace& Face = paiMesh->mFaces[i];
            assert(Face.mNumIndices == 3);
            tempPushBackModel.indices.push_back(Face.mIndices[0]);
            tempPushBackModel.indices.push_back(Face.mIndices[1]);
            tempPushBackModel.indices.push_back(Face.mIndices[2]);
        }
     meshes.push_back(tempPushBackModel);
    }
    return meshes;
    //m_Entries[Index].Init(Vertices, Indices);
}
/*
std::vector<IndexedModel> loadModelAssimp(const std::string& fileName){
    //IndexedModel indexedModel;
    std::vector<IndexedModel> meshes;
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(("./res/models/" + fileName).c_str(),
                                             //aiProcess_Triangulate |
                                             aiProcess_GenSmoothNormals |
                                             //aiProcess_GenNormals |
                                             aiProcess_FlipUVs);// |
                                             //aiProcess_CalcTangentSpace);
    if(scene->HasAnimations()){
        printf("Model contains animations\n");
        printf("Model has %d number of animations", scene->mNumAnimations);
    }
    else
        printf("Model contains NO animations\n\n");



    if(!scene)
    {
        printf("Mesh load failed!: ");
        assert(0 == 0);
    }


    //printf()scene->mNumAnimations
    int incrementMesh = 0;
    printf("Model contains %d meshes\n\n", scene->mNumMeshes);

    for( unsigned int i = 0; i < scene->mNumMeshes; i++){
        const aiMesh* model = scene->mMeshes[i];
        IndexedModel indexedModel;


        printf("Mesh %d contains %d vertices \n", i, model->mNumVertices);


        const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);

        for(unsigned int i = 0; i < model->mNumVertices; i++)
        {
            const aiVector3D pos = model->mVertices[i];
            const aiVector3D normal = model->mNormals[i];
            const aiVector3D texCoord = model->HasTextureCoords(0) ? model->mTextureCoords[0][i] : aiZeroVector;
            //const aiVector3D tangent = model->mTangents[i];

            indexedModel.positions.push_back(glm::vec3(pos.x, pos.y, pos.z));
            indexedModel.texCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
            indexedModel.normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
            //indexedModel.tangents.push_back(glm::vec3(tangent.x, tangent.y, tangent.z));
        }


        for(unsigned int i = 0; i < model->mNumFaces; i++)
        {
            const aiFace& face = model->mFaces[i];
            assert(face.mNumIndices == 3);
            indexedModel.indices.push_back(face.mIndices[0]);
            indexedModel.indices.push_back(face.mIndices[1]);
            indexedModel.indices.push_back(face.mIndices[2]);
        }
        //Mesh tmpmesh(indexedModel);
        meshes.push_back(indexedModel);
        //incrementMesh++;
        //model = scene->mMeshes[incrementMesh];
    }
    //indexedModel.CalcNormals();

    return meshes; //indexedModel;
}
*/
Mesh::Mesh(const std::string& fileName){
    IndexedModel model = OBJModel(fileName).ToIndexedModel();
    InitMesh(model);
}

Mesh::Mesh(IndexedModel& model){
    InitMesh(model);
}

Mesh::Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices){
    //m_drawCount = numVertices;

    IndexedModel model;

    for(unsigned int i = 0; i < numVertices; i++){
        model.positions.push_back(*vertices[i].GetPos());
        model.texCoords.push_back(*vertices[i].GetTexCoord());
        model.normals.push_back(*vertices[i].GetNormal());
    }

    for(unsigned int i = 0; i < numIndices; i++)
        model.indices.push_back(indices[i]);

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
    glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


    //////////////////////////////
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STREAM_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(model.normals[0]), &model.normals[0], GL_STREAM_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STREAM_DRAW);

    glBindVertexArray(0);
}

void Mesh::Draw(){
    glBindVertexArray(m_vertexArrayObject);

    glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, m_drawCount);

    glBindVertexArray(0);
}


