#ifndef BULLETPHYSICS_H_INCLUDED
#define BULLETPHYSICS_H_INCLUDED

#include <vector>
#include <btBulletDynamicsCommon.h>
#include "Camera.h"

// Reference: http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/
// Reference: http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Hello_World

btDiscreteDynamicsWorld* intitBullet(float gravity);

int getPhysicsObjIndex(btDiscreteDynamicsWorld** dynamicsWorld, Camera** cam);

btRigidBody* addSphere(float rad,float x,float y,float z,float mass,
                       btDiscreteDynamicsWorld** world, std::vector<btRigidBody*> &bodies);


#endif // BULLETPHYSICS_H_INCLUDED
