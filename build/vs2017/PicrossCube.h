#pragma once
#include "GameObject.h"
#include "PicrossStructs.hpp"

//Class for each individual cube that makes up the level

class PicrossCube : public GameObject
{
public:
	PicrossCube(Picross::CubeCoords coords);
	~PicrossCube();

	bool getFinalObject() { return finalObject; };
	void setFinalObject(bool finalObject) { this->finalObject = finalObject; };
	Picross::CubeCoords getCoords() { return coords; };

private:

	bool finalObject;	//True if part of the "final shape" of the level
	Picross::CubeCoords coords;
};

