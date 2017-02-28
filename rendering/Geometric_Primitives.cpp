#include <stdlib.h>
//#include "stb_perlin.h"

#include "Geometric_Primitives.h"
// \/ \/ was defined in main... M_PI at least...
#define M_PI 3.14159265358979323846 /* pi */
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#define M_PI_4 0.78539816339744830962 /* pi/4 */

// https://gist.github.com/davidbitton/1094320
Mesh setupSphere(float radius, float latitudeBands, float longitudeBands){
    IndexedModel model;

    for (float latNumber = 0; latNumber <= latitudeBands; latNumber++) {
        float theta = latNumber * M_PI / latitudeBands;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (float longNumber = 0; longNumber <= longitudeBands; longNumber++) {
            float phi = longNumber * 2 * M_PI / longitudeBands;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            glm::vec3 normal(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
            glm::vec2 texCoord(1 - (longNumber / longitudeBands), 1 - (latNumber / latitudeBands));
            glm::vec3 positions(radius * normal.x, radius * normal.y, radius * normal.z);

            model.normals.push_back(normal);
            model.texCoords.push_back(texCoord);
            model.positions.push_back(positions);
        }

        for (int latNumber = 0; latNumber < latitudeBands; latNumber++) {
            for (int longNumber = 0; longNumber < longitudeBands; longNumber++) {
                int first = (latNumber * (longitudeBands + 1)) + longNumber;
                int second = first + longitudeBands + 1;

                model.indices.push_back(first);
                model.indices.push_back(second);
                model.indices.push_back(first + 1);

                model.indices.push_back(second);
                model.indices.push_back(second + 1);
                model.indices.push_back(first + 1);

            }
        }
        //size_t verticesCount = verticesVector.size();
        //vertexStruct vertices[verticesCount];
        //size_t indicesCount = indicesVector.size();
        //int indices[indicesCount];

        //copy(verticesVector.begin(), verticesVector.end(), vertices);
        //copy(indicesVector.begin(), indicesVector.end(), indices);
    }
    return Mesh(model);
}

Mesh setupCube(float cubeSize){
    //Mesh mesh;
    //float cbs
    IndexedModel model;

    Vertex cubeVertices[] = {
        // FRONT SIDE
        Vertex(glm::vec3(-cubeSize, -cubeSize, cubeSize), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
        Vertex(glm::vec3(cubeSize, -cubeSize, cubeSize), glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
        Vertex(glm::vec3(cubeSize, cubeSize, cubeSize), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, 1.0)),
        Vertex(glm::vec3(-cubeSize, cubeSize, cubeSize), glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)),
        // RIGHT SIDE
        Vertex(glm::vec3(cubeSize, cubeSize, cubeSize), glm::vec2(0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)),
        Vertex(glm::vec3(cubeSize, cubeSize, -cubeSize), glm::vec2(1.0, 0.0), glm::vec3(1.0, 0.0, 0.0)),
        Vertex(glm::vec3(cubeSize, -cubeSize, -cubeSize), glm::vec2(1.0, 1.0), glm::vec3(1.0, 0.0, 0.0)),
        Vertex(glm::vec3(cubeSize, -cubeSize, cubeSize), glm::vec2(0.0, 1.0), glm::vec3(1.0, 0.0, 0.0)),
        // BACK SIDE
        Vertex(glm::vec3(-cubeSize, -cubeSize, -cubeSize), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
        Vertex(glm::vec3(cubeSize, -cubeSize, -cubeSize), glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
        Vertex(glm::vec3(cubeSize, cubeSize, -cubeSize), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, -1.0)),
        Vertex(glm::vec3(-cubeSize, cubeSize, -cubeSize), glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)),
        // LEFT SIDE
        Vertex(glm::vec3(-cubeSize, -cubeSize, -cubeSize), glm::vec2(0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0)),
        Vertex(glm::vec3(-cubeSize, -cubeSize, cubeSize), glm::vec2(1.0, 0.0), glm::vec3(-1.0, 0.0, 0.0)),
        Vertex(glm::vec3(-cubeSize, cubeSize, cubeSize), glm::vec2(1.0, 1.0), glm::vec3(-1.0, 0.0, 0.0)),
        Vertex(glm::vec3(-cubeSize, cubeSize, -cubeSize), glm::vec2(0.0, 1.0), glm::vec3(-1.0, 0.0, 0.0)),
        // UPPER SIDE
        Vertex(glm::vec3(cubeSize, cubeSize, cubeSize), glm::vec2(0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)),
        Vertex(glm::vec3(-cubeSize, cubeSize, cubeSize), glm::vec2(1.0, 0.0), glm::vec3(0.0, 1.0, 0.0)),
        Vertex(glm::vec3(-cubeSize, cubeSize, -cubeSize), glm::vec2(1.0, 1.0), glm::vec3(0.0, 1.0, 0.0)),
        Vertex(glm::vec3(cubeSize, cubeSize, -cubeSize), glm::vec2(0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)),
        // BOTTOM SIDE
        Vertex(glm::vec3(-cubeSize, -cubeSize, -cubeSize), glm::vec2(0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
        Vertex(glm::vec3(cubeSize, -cubeSize, -cubeSize), glm::vec2(1.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
        Vertex(glm::vec3(cubeSize, -cubeSize, cubeSize), glm::vec2(1.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
        Vertex(glm::vec3(-cubeSize, -cubeSize, cubeSize), glm::vec2(0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
    };
    unsigned int indices[] = { 0,  1,  2,  0,  2,  3,   //front
                               4,  5,  6,  4,  6,  7,   //right
                               8,  9,  10, 8,  10, 11,  //back
                               12, 13, 14, 12, 14, 15,  //left
                               16, 17, 18, 16, 18, 19,  //upper
                               20, 21, 22, 20, 22, 23}; //bottom

    int vertSize = sizeof(cubeVertices) / sizeof(cubeVertices[0]);
    int indicesSize = sizeof(indices) / sizeof(indices[0]);

    for(int i = 0; i < vertSize; i++){
        model.normals.push_back(*cubeVertices[i].GetNormal());
        model.texCoords.push_back(*cubeVertices[i].GetTexCoord());
        model.positions.push_back(*cubeVertices[i].GetPos());
    }

    for(int i = 0; i < indicesSize; i++)
        model.indices.push_back(indices[i]);

    //return Mesh mesh(cubeVertices, sizeof(cubeVertices)/sizeof(cubeVertices[0]), indices, sizeof(indices)/sizeof(indices[0]));

    //return mesh;
    return Mesh(model);
}

Mesh setupLowDensityPlane(float planeSize){
    IndexedModel model;

    Vertex planeVertices[] = {
        // BOTTOM SIDE
        // reset y value to 0
        Vertex(glm::vec3(-planeSize, 0.0f, -planeSize), glm::vec2(0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)),
        Vertex(glm::vec3(planeSize, 0.0f, -planeSize), glm::vec2(1.0, 0.0), glm::vec3(0.0, 1.0, 0.0)),
        Vertex(glm::vec3(planeSize, 0.0f, planeSize), glm::vec2(1.0, 1.0), glm::vec3(0.0, 1.0, 0.0)),
        Vertex(glm::vec3(-planeSize, 0.0f, planeSize), glm::vec2(0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)),
    };
    unsigned int indices[] = { 0,  1,  2,  0,  2,  3 };

    int vertSize = sizeof(planeVertices) / sizeof(planeVertices[0]);
    int indicesSize = sizeof(indices) / sizeof(indices[0]);

    for(int i = 0; i < vertSize; i++){
        model.normals.push_back(*planeVertices[i].GetNormal());
        model.texCoords.push_back(*planeVertices[i].GetTexCoord());
        model.positions.push_back(*planeVertices[i].GetPos());
    }

    for(int i = 0; i < indicesSize; i++)
        model.indices.push_back(indices[i]);

    return Mesh(model);
}
// add density(the x-z value in positions) - was hard coded to .5
// add noise or fractal bumping for the y value then recalculate the normals
Mesh setupHighDensityPlane(int mapSize, float density, bool bump){
    IndexedModel model;
    float yBumpAmount = 0.0f;
    int iter = 0;

    for(int i = -mapSize; i <= mapSize; i++){

        for(int j = -mapSize; j <= mapSize; j++){
            int index = model.positions.size();//model.normals.size()+model.positions.size();//+model.texCoords.size(); //.size(); //(model.positions.size() + model.normals.size() + model.texCoords.size());


            model.positions.push_back(glm::vec3(-density + i, yBumpAmount, -density + j));
            model.positions.push_back(glm::vec3(+density + i, yBumpAmount, -density + j));
            model.positions.push_back(glm::vec3(+density + i, yBumpAmount, +density + j));
            model.positions.push_back(glm::vec3(-density + i, yBumpAmount, +density + j));

            //if(bump == true){
                //yBumpAmount = stb_perlin_noise3(model.positions[iter].x, model.positions[iter].y, model.positions[iter].z);
            //}
            //iter++;

            model.texCoords.push_back(glm::vec2(0.0f, 0.0f));
            model.texCoords.push_back(glm::vec2(1.0f, 0.0f));
            model.texCoords.push_back(glm::vec2(1.0f, 1.0f));
            model.texCoords.push_back(glm::vec2(0.0f, 1.0f));

            //if(!bump){
            model.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
            model.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
            model.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
            model.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
            //}

            model.indices.push_back(index+0);
            model.indices.push_back(index+3);
            model.indices.push_back(index+2);
            model.indices.push_back(index+2);
            model.indices.push_back(index+1);
            model.indices.push_back(index+0);
        }
    }

    //if(bump == true)
       //model.CalcNormals();

    return Mesh(model);
}