#include "BulletPhysics.h"
//#include "Raycast.h"

int rgbToGS(int r, int g, int b){
	//averaging comp
	return (r + g + b) / 3;

}

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
// btRigidBody*

void addSphere(float rad, glm::vec3& position, glm::vec3& velocity, float mass,
                       btDiscreteDynamicsWorld** world, std::vector<btRigidBody*> &bodies, int numAdded)
{
	btTransform t;	//position and rotation
	t.setIdentity();
	t.setOrigin(btVector3(position.x, position.y, position.z));	//put it to x,y,z coordinates
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

	// Small hack : store the mesh's index "i" in Bullet's User Pointer.
    // Will be used to know which object is picked.
    // A real program would probably pass a "MyGameObjectPointer" instead.
    body->setUserPointer((void*)numAdded);

    // set velocity, here its the camera direction passed in
    body->setLinearVelocity(btVector3(velocity.x,velocity.y,velocity.z)*75.0f);

	//return body;
}

void addCube(glm::vec3& dimension, glm::vec3& position, float mass, glm::vec3& orientation,
                       btDiscreteDynamicsWorld** world, std::vector<btRigidBody*> &bodies, int numAdded){
    btTransform t;	//position and rotation
	t.setIdentity();
	t.setOrigin(btVector3(btVector3(position.x,position.y,position.z)));	//put it to x,y,z coordinates
	t.setRotation(btQuaternion(orientation.x,orientation.y,orientation.z,1.0f));
	btBoxShape* box=new btBoxShape(btVector3(dimension.x,dimension.y,dimension.z));	//it's a sphere, so use sphereshape
	btVector3 inertia(0,0,0);	//inertia is 0,0,0 for static object, else
	if(mass!=0.0)
		box->calculateLocalInertia(mass,inertia);	//it can be determined by this function (for all kind of shapes)

	btMotionState* motion=new btDefaultMotionState(t);	//set the position (and motion)
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,box,inertia);	//create the constructioninfo, you can create multiple bodies with the same info
	btRigidBody* body=new btRigidBody(info);	//let's create the body itself
	//body->setRestitution(1.0f);
	(*world)->addRigidBody(body);	//and let the world know about it
	bodies.push_back(body);	//to be easier to clean, I store them a vector
	//return body;

	// Small hack : store the mesh's index "i" in Bullet's User Pointer.
    // Will be used to know which object is picked.
    // A real program would probably pass a "MyGameObjectPointer" instead.
    body->setUserPointer((void*)numAdded);
}

void addPlane(glm::vec3& orientation, glm::vec3& position, btDiscreteDynamicsWorld** world){
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    //groundRigidBody->setRestitution(1.0f);
    (*world)->addRigidBody(groundRigidBody);
    //return groundRigidBody;
}
//void addTerrain(glm::vec3& orientation, glm::vec3& position, btDiscreteDynamicsWorld** world,
//                const std::string& fileName, float hScale, unsigned char** data, int w, int h, int bpp){

// http://opensimulator.org/git/opensim-libs-save/BulletPhysics/bulletTrunk/Demos/VehicleDemo/VehicleDemo.cpp
void addTerrain(glm::vec3& orientation, glm::vec3& position, btDiscreteDynamicsWorld** world,
                float hScale, unsigned char** data, int w, int h, int bpp){

    int width, height, bytesPerPixel;
    width = w;
    height = h;
    bytesPerPixel = bpp;
	//unsigned char* data2 = stbi_load(fileName.c_str(), &width, &height, &bytesPerPixel, 3);

    std::cout << "MemoryLoc: " << data << std::endl;
    int i;

    const float TRIANGLE_SIZE=1.0f;

	//create a triangle-mesh ground
	int vertStride = sizeof(btVector3);
	int indexStride = 3*sizeof(int);

	const int NUM_VERTS_X = width;
	const int NUM_VERTS_Y = height;
	const int totalVerts = NUM_VERTS_X*NUM_VERTS_Y;

	const int totalTriangles = 2*(NUM_VERTS_X-1)*(NUM_VERTS_Y-1);

	btVector3* m_vertices = new btVector3[totalVerts];
	int*	gIndices = new int[totalTriangles*3];



	for ( i=0;i<NUM_VERTS_X;i++)
	{
		for (int j=0;j<NUM_VERTS_Y;j++)
		{
			//float wl = .2f;
			//height set to zero, but can also use curved landscape, just uncomment out the code
			float height = 0.f;//20.f*sinf(float(i)*wl)*cosf(float(j)*wl);
			unsigned char r = (*data)[(i + j * width) * 3 + 0];
            unsigned char g = (*data)[(i + j * width) * 3 + 1];
            unsigned char b = (*data)[(i + j * width) * 3 + 2];
            height = rgbToGS((int)r, (int)g, (int)b);


			m_vertices[i+j*NUM_VERTS_X].setValue(
				(i-NUM_VERTS_X*0.5f)*TRIANGLE_SIZE,
				height*hScale,
				(j-NUM_VERTS_Y*0.5f)*TRIANGLE_SIZE);

		}
	}

	int index=0;
	for ( i=0;i<NUM_VERTS_X-1;i++)
	{
		for (int j=0;j<NUM_VERTS_Y-1;j++)
		{
			gIndices[index++] = j*NUM_VERTS_X+i;
			gIndices[index++] = j*NUM_VERTS_X+i+1;
			gIndices[index++] = (j+1)*NUM_VERTS_X+i+1;

			gIndices[index++] = j*NUM_VERTS_X+i;
			gIndices[index++] = (j+1)*NUM_VERTS_X+i+1;
			gIndices[index++] = (j+1)*NUM_VERTS_X+i;
		}
	}

	btTriangleIndexVertexArray* m_indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles,
		gIndices,
		indexStride,
		totalVerts,(btScalar*) &m_vertices[0].x(),vertStride);

	bool useQuantizedAabbCompression = true;
	btCollisionShape*  groundShape = new btBvhTriangleMeshShape(m_indexVertexArrays,useQuantizedAabbCompression);

	//tr.setOrigin(btVector3(0,-4.5f,0));
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(.5, -19.85, .5)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    //groundRigidBody->setRestitution(1.0f);
    (*world)->addRigidBody(groundRigidBody);
}

//Transform
void physicsTransforms(Transform** transform, std::vector<btRigidBody*> &bodies, int index){
    //Transform transform;

    btVector3 btpos = bodies[index]->getCenterOfMassPosition();
    glm::vec3 pos(btpos.x(), btpos.y(), btpos.z());
    //glm::vec3 pp(0.0, -4.0, 0.0);
    (*transform)->SetPos(pos);

    // Set Rotations
    btMatrix3x3 mrot = bodies[index]->getCenterOfMassTransform().getBasis();

    glm::mat4 qq(mrot[0][0], mrot[0][1], mrot[0][2], 0.0f,
                 mrot[1][0], mrot[1][1], mrot[1][2], 0.0f,
                 mrot[2][0], mrot[2][1], mrot[2][2], 0.0f,
                 0.0f,       0.0f,       0.0f,       1.0f);

    (*transform)->SetRot(qq);

    //return transform;
}

void setupBoxPositions(std::vector<btRigidBody*> &bodies, btDiscreteDynamicsWorld** world, int totalBoxes){
    int amountPerLevel = 10;
    float radius = 5.5f;
    float boxy = 50.0f;//-19.80f;
    //float displacement = 0.75f;
    int j = 0;
    for(int i = 0; i < totalBoxes; i++){
        float xr = rand() % 30;
        //float y = rand() % 40 + 20;
        float zr = rand() % 30;


        if(j >= amountPerLevel)
            j = 0;


        float angle = (float)j / (float)amountPerLevel * 360.0f;
        j++;
        //GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius;// + displacement;
        //displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        //GLfloat y = -2.5f + displacement * 0.4f; // Keep height of asteroid field smaller compared to width of x and z
        //displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius;// + displacement;

        if(i % amountPerLevel == 0)
            boxy += 2.05f;

        glm::vec3 position = glm::vec3(float(x), float(boxy), float(z));
        glm::vec3 boxRot(xr+x, 1.0, zr+z);
        glm::vec3 dims = glm::vec3(1.0f);
        addCube(dims, position, 1.0f, boxRot, world, bodies, i);
    }
}

void resetBoxPositions(std::vector<btRigidBody*> &bodies, btDiscreteDynamicsWorld** world, int totalBoxes, int sphereAdded){
    float boxy = 50.0f;//-19.8f;
    int amountPerLevel = 10;
    int j = 0;
    float radius = 5.5f;

    for(int i = 0; i < totalBoxes; i++){
        float xr = rand() % 30;
        float zr = rand() % 30;

        if(j >= amountPerLevel)
            j = 0;

        float angle = (float)j / (float)amountPerLevel * 360.0f;
        j++;

        float z = cos(angle) * radius;// + displacement;
        float x = sin(angle) * radius;// + displacement;

        if(i % amountPerLevel == 0)
            boxy += 2.05f;

        glm::vec3 position = glm::vec3(float(x), float(boxy), float(z));
        glm::vec3 boxRot(xr+x, 1.0, zr+z);

        bodies[i]->setWorldTransform(btTransform(btQuaternion(boxRot.x,0,boxRot.z,1), btVector3(position.x, position.y, position.z)));
        bodies[i]->setActivationState(1);
        bodies[i]->setLinearVelocity(btVector3(0.0,0.0,0.0));
        bodies[i]->setAngularVelocity(btVector3(0.0,0.0,0.0));
    }
    int totalObjects = (*world)->getNumCollisionObjects()-1;

    for (int i = totalBoxes+1; i <= totalObjects; i++){
        btCollisionObject* obj = (*world)->getCollisionObjectArray()[i];
        (*world)->removeCollisionObject(obj);
        delete obj;
    }
    bodies.resize(totalBoxes);
    sphereAdded = totalBoxes;
}

void ScreenPosToWorldRay(
	int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
	int screenWidth, int screenHeight,  // Window size, in pixels
	glm::mat4 ViewMatrix,               // Camera position and orientation
	glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
	glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
	glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
){

	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	glm::vec4 lRayStart_NDC(
		((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)mouseY/(float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);
	glm::vec4 lRayEnd_NDC(
		((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f,
		((float)mouseY/(float)screenHeight - 0.5f) * 2.0f,
		0.0,
		1.0f
	);


	// The Projection matrix goes from Camera Space to NDC.
	// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
	glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

	glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera/=lRayStart_camera.w;
	glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; lRayStart_world /=lRayStart_world .w;
	glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera  /=lRayEnd_camera  .w;
	glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world   /=lRayEnd_world   .w;


	// Faster way (just one inverse)
	//glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
	//glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
	//glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;


	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);


	out_origin = glm::vec3(lRayStart_world);
	out_direction = glm::normalize(lRayDir_world);
}
