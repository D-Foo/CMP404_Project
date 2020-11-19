#pragma once
#include "maths/aabb.h"
#include "maths/sphere.h"
#include "graphics/mesh_instance.h"
#include "graphics/mesh.h"
#include <algorithm>

class CollisionDetector
{
public:
	CollisionDetector();
	~CollisionDetector();
	bool sphereToSphere(gef::MeshInstance* sphere1, gef::MeshInstance* sphere2);
	bool AABB(gef::MeshInstance* mesh1, gef::MeshInstance* mesh2);
	static bool rayCube(gef::Vector4 rayDirection, gef::Vector4 rayOrigin, gef::Vector4 leftBottom, gef::Vector4 topRight);
	static bool rayCube2(gef::Vector4 rayDirection, gef::Vector4 rayOrigin, gef::Vector4 leftBottom, gef::Vector4 topRight);
};

// Distance between 2 spheres
//If radius  of both > distance
//Collision


//GET POSITION OF SPHERES
//GET RADII
//COMPARE
//RETURN BOOL