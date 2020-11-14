#pragma once
#include "maths/aabb.h"
#include "maths/sphere.h"
#include "graphics/mesh_instance.h"
#include "graphics/mesh.h"

class CollisionDetector
{
public:
	CollisionDetector();
	~CollisionDetector();
	bool sphereToSphere(gef::MeshInstance* sphere1, gef::MeshInstance* sphere2);
	bool AABB(gef::MeshInstance* mesh1, gef::MeshInstance* mesh2);
};

// Distance between 2 spheres
//If radius  of both > distance
//Collision


//GET POSITION OF SPHERES
//GET RADII
//COMPARE
//RETURN BOOL