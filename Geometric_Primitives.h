#ifndef GEOMETRIC_PRIMITIVES_H_INCLUDED
#define GEOMETRIC_PRIMITIVES_H_INCLUDED

//#include "obj_loader.h"
#include "Mesh.h"
//setup err create..?
Mesh setupSphere(float radius, float latitudeBands, float longitudeBands);
Mesh setupCube(float cubeSize);
Mesh setupLowDensityPlane(float planeSize);
Mesh setupHighDensityPlane(int mapSize, float density, bool bump);

#endif // GEOMETRIC_PRIMITIVES_H_INCLUDED
