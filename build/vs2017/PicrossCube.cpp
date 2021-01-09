#include "PicrossCube.h"

PicrossCube::PicrossCube(Picross::CubeCoords coords)
{
	this->coords = coords;
	finalObject = false;
}

PicrossCube::~PicrossCube()
{

}