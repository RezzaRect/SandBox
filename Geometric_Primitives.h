#ifndef GEOMETRIC_PRIMITIVES_H_INCLUDED
#define GEOMETRIC_PRIMITIVES_H_INCLUDED

#include "obj_loader.h"
#include "Mesh.h"
//setup err create..?
IndexedModel setupSphere(float radius, float latitudeBands, float longitudeBands);
IndexedModel setupCube(float cubeSize);
IndexedModel setupLowDensityPlane(float planeSize);

#endif // GEOMETRIC_PRIMITIVES_H_INCLUDED
