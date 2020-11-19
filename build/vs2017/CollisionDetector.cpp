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

bool CollisionDetector::rayCube(gef::Vector4 rayDirection, gef::Vector4 rayOrigin, gef::Vector4 leftBottom, gef::Vector4 topRight)
{
	//Code from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms

	gef::Vector4 dirfrac;
	dirfrac.set_x(1.0f / rayDirection.x());
	dirfrac.set_y(1.0f / rayDirection.y());
	dirfrac.set_z(1.0f / rayDirection.z());
	
	float t1 = (leftBottom.x() - rayOrigin.x()) * dirfrac.x();
	float t2 = (topRight.x() - rayOrigin.x()) * dirfrac.x();
	float t3 = (leftBottom.y() - rayOrigin.y()) * dirfrac.y();
	float t4 = (topRight.y() - rayOrigin.y()) * dirfrac.y();
	float t5 = (leftBottom.z() - rayOrigin.z()) * dirfrac.z();
	float t6 = (topRight.z() - rayOrigin.z()) * dirfrac.z();

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	float t;

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0)
	{
		t = tmax;
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		t = tmax;
		return false;
	}

	t = tmin;
	return true;
}

bool CollisionDetector::rayCube2(gef::Vector4 rayDirection, gef::Vector4 rayOrigin, gef::Vector4 bottomLeft, gef::Vector4 topRight)
{
	//Code from https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection



	float tmin = (bottomLeft.x() - rayOrigin.x()) / rayDirection.x();
	float tmax = (topRight.x() - rayOrigin.x()) / rayDirection.x();

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (bottomLeft.y() - rayOrigin.y()) / rayDirection.y();
	float tymax = (topRight.y() - rayOrigin.y()) / rayDirection.y();

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (bottomLeft.z() - rayOrigin.z()) / rayDirection.z();
	float tzmax = (topRight.z() - rayOrigin.z()) / rayDirection.z();

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}