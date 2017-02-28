#ifndef BULLETPHYSICS_H_INCLUDED
#define BULLETPHYSICS_H_INCLUDED

#include <vector>
#include <btBulletDynamicsCommon.h>
//#include "core/Camera.h"
//#include "core/Transform.h"
#include "../project_includes.h"
// Reference: http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/
// Reference: http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Hello_World

btDiscreteDynamicsWorld* intitBullet(float gravity);

int getPhysicsObjIndex(btDiscreteDynamicsWorld** dynamicsWorld, Camera** cam);
// btRigidBody*
void addSphere(float rad, glm::vec3& position, glm::vec3& velocity, float mass,
               btDiscreteDynamicsWorld** world, std::vector<btRigidBody*> &bodies, int numAdded);

void addCube(glm::vec3& dimension, glm::vec3& position, float mass, glm::vec3& orientation,
             btDiscreteDynamicsWorld** world, std::vector<btRigidBody*> &bodies, int numAdded);

void addPlane(glm::vec3& orientation, glm::vec3& position, btDiscreteDynamicsWorld** world);
//Transform
void physicsTransforms(Transform** transform, std::vector<btRigidBody*> &bodies, int index);

void setupBoxPositions(std::vector<btRigidBody*> &bodies, btDiscreteDynamicsWorld** world, int totalBoxes);

void resetBoxPositions(std::vector<btRigidBody*> &bodies, btDiscreteDynamicsWorld** world, int totalBoxes, int spereAdded);

// Reference: http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/
void ScreenPosToWorldRay(int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
                         int screenWidth, int screenHeight,  // Window size, in pixels
                         glm::mat4 ViewMatrix,               // Camera position and orientation
                         glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
                         glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
                         glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
                         );

#endif // BULLETPHYSICS_H_INCLUDED
