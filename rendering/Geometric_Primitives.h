#ifndef GEOMETRIC_PRIMITIVES_H_INCLUDED
#define GEOMETRIC_PRIMITIVES_H_INCLUDED

//#include "obj_loader.h"
#include "Mesh.h"
//setup err create..?
int rgbToGrayscale(int r, int g, int b);
Mesh setupSphere(float radius, float latitudeBands, float longitudeBands);
Mesh setupCube(float cubeSize);
Mesh setupLowDensityPlane(float planeSize);
Mesh setupHighDensityPlane(int mapSize, float density, bool bump);
Mesh setupFullScreenQuad(float quadSize);
//Mesh Terrain(const std::string& fileName, float hScale, unsigned char** data, int w, int h, int bpp);
Mesh Terrain(float hScale, unsigned char** data, int w, int h, int bpp);

#endif // GEOMETRIC_PRIMITIVES_H_INCLUDED
