#pragma once
#include "graphics/mesh_instance.h"
#include "maths/quaternion.h"

class GameObject : public gef::MeshInstance
{
public:
	GameObject();
	~GameObject();

	void setRotation(gef::Vector4 rotation);
	gef::Vector4 getRotation();
	void setPosition(gef::Vector4 position);
	gef::Vector4 getPostition();
private:
	gef::Vector4 velocity;
	gef::Vector4 position;
	gef::Vector4 rotation;	

	gef::Quaternion createQuaternion();
	void updateTransform();
};

