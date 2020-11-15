#include "GameObject.h"

GameObject::GameObject() : gef::MeshInstance()
{
    velocity = gef::Vector4::kZero;
    position = gef::Vector4::kZero;
    rotation = gef::Vector4::kZero;
}

GameObject::~GameObject()
{
}

void GameObject::setRotation(gef::Vector4 rotation)
{
    this->rotation = rotation;
    updateTransform();
}

gef::Vector4 GameObject::getRotation()
{
    return rotation;
}

void GameObject::setPosition(gef::Vector4 position)
{
    this->position = position;
    updateTransform();
}

gef::Vector4 GameObject::getPosition()
{
    return position;
}

gef::Quaternion GameObject::createQuaternion()
{
    float yaw = rotation.z();
    float pitch = rotation.y();
    float roll = rotation.x();

    //Source from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    double cy = cos(yaw * 0.5);
    double sy = sin(yaw * 0.5);
    double cp = cos(pitch * 0.5);
    double sp = sin(pitch * 0.5);
    double cr = cos(roll * 0.5);
    double sr = sin(roll * 0.5);

    gef::Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

void GameObject::updateTransform()
{
    gef::Matrix44 transform;
	gef::Matrix44 translation;
	gef::Matrix44 rotation;

    transform.SetIdentity(); translation.SetIdentity(); rotation.SetIdentity();

	translation.SetTranslation(position);
    rotation.Rotation(createQuaternion());
	
	transform = rotation * translation;

	set_transform(transform);
}
