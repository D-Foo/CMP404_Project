#include "CollisionDetector.h"



CollisionDetector::CollisionDetector()
{
}


CollisionDetector::~CollisionDetector()
{
}

bool CollisionDetector::sphereToSphere(gef::MeshInstance * sphere1, gef::MeshInstance * sphere2)
{
	gef::Sphere sphere1Global = sphere1->mesh()->bounding_sphere().Transform(sphere1->transform());
	gef::Sphere sphere2Global = sphere1->mesh()->bounding_sphere().Transform(sphere2->transform());

	float distance = abs(sphere1Global.position().x() - sphere2Global.position().x()) + abs(sphere1Global.position().y() - sphere2Global.position().y()) + abs(sphere1Global.position().z() - sphere2Global.position().z());
	float radiusSum = sphere1Global.radius() + sphere2Global.radius();

	if (distance <= radiusSum)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CollisionDetector::AABB(gef::MeshInstance * mesh1, gef::MeshInstance * mesh2)
{
	bool collision = false; bool XCollision = false; bool YCollision = false; bool ZCollision = false;
	gef::Aabb AABB1Global = mesh1->mesh()->aabb().Transform(mesh1->transform());
	gef::Aabb AABB2Global = mesh2->mesh()->aabb().Transform(mesh2->transform());

	if (AABB1Global.min_vtx().x() >= AABB2Global.min_vtx().x() && AABB1Global.min_vtx().x() <= AABB2Global.max_vtx().x() || AABB1Global.max_vtx().x() >= AABB2Global.min_vtx().x() && AABB1Global.min_vtx().x() <= AABB2Global.max_vtx().x())
	{
		XCollision = true;
	}

	if (AABB1Global.min_vtx().y() >= AABB2Global.min_vtx().y() && AABB1Global.min_vtx().y() <= AABB2Global.max_vtx().y() || AABB1Global.max_vtx().y() >= AABB2Global.min_vtx().y() && AABB1Global.min_vtx().y() <= AABB2Global.max_vtx().y())
	{
		YCollision = true;
	}

	if (AABB1Global.min_vtx().z() >= AABB2Global.min_vtx().z() && AABB1Global.min_vtx().z() <= AABB2Global.max_vtx().z() || AABB1Global.max_vtx().z() >= AABB2Global.min_vtx().z() && AABB1Global.min_vtx().z() <= AABB2Global.max_vtx().z())
	{
		ZCollision = true;
	}

	if (XCollision && YCollision && ZCollision)
	{
		collision = true;
	}

	return collision;
}
