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
	bool getProtected() { return markedProtected; };
	void setFinalObject(bool finalObject) { this->finalObject = finalObject; };
	void setProtected(bool markedProtected) { this->markedProtected = markedProtected; };
	Picross::CubeCoords getCoords() { return coords; };


private:

	bool finalObject;	//True if part of the "final shape" of the level
	bool markedProtected;	//True if marked by the player or was attempted to be destroyed when part of the finalObject, prevents attemped destruction
	Picross::CubeCoords coords;
};

