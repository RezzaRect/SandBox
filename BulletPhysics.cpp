#include "BulletPhysics.h"
#include "Raycast.h"


btDiscreteDynamicsWorld* intitBullet(float gravity){
    /* ------------- START BULLET INITIALIZATION ------------------- */
    // Build the broadphase
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, gravity, 0));
	return dynamicsWorld;
}

int getPhysicsObjIndex(btDiscreteDynamicsWorld** dynamicsWorld, Camera** cam){
    glm::vec3 out_origin;
    glm::vec3 out_direction;
    ScreenPosToWorldRay(
        1024/2, 600/2,
        1024, 600,
        GetView(**cam),
        (*cam)->projection,
        out_origin,
        out_direction
    );

    glm::vec3 out_end = out_origin + out_direction*1000.0f;

    btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z));
    (*dynamicsWorld)->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z), RayCallback);
    //int i;
    if(RayCallback.hasHit()) {
        return (size_t)RayCallback.m_collisionObject->getUserPointer();

    }
    return -1;
}

btRigidBody* addSphere(float rad,float x,float y,float z,float mass,
                       btDiscreteDynamicsWorld** world, std::vector<btRigidBody*> &bodies)
{
	btTransform t;	//position and rotation
	t.setIdentity();
	t.setOrigin(btVector3(x,y,z));	//put it to x,y,z coordinates
	btSphereShape* sphere=new btSphereShape(rad);	//it's a sphere, so use sphereshape
	btVector3 inertia(0,0,0);	//inertia is 0,0,0 for static object, else
	if(mass!=0.0)
		sphere->calculateLocalInertia(mass,inertia);	//it can be determined by this function (for all kind of shapes)

	btMotionState* motion=new btDefaultMotionState(t);	//set the position (and motion)
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);	//create the constructioninfo, you can create multiple bodies with the same info
	btRigidBody* body=new btRigidBody(info);	//let's create the body itself
	//body->setRestitution(1.0f);
	(*world)->addRigidBody(body);	//and let the world know about it
	bodies.push_back(body);	//to be easier to clean, I store them a vector
	return body;
}
