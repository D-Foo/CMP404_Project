#include <vector>
#include "PicrossCube.h"
#include "maths/vector4.h"
#include "graphics/renderer_3d.h"
#include "primitive_builder.h"

#pragma once
class PicrossLevel
{
public:
	PicrossLevel(PrimitiveBuilder* pBuilder);
	~PicrossLevel();

	void render(gef::Renderer3D* renderer);

private:
	//Vars
	float levelScale;
	gef::Vector4 levelCenter;
	float cubeSideSize;


	//Cubes
	int rowSize;	//Width
	int columnSize;	//Height
	int depthSize;	//Depth
	gef::Vector4 cubeSize;

	void initCubes(PrimitiveBuilder* pBuilder);

	std::vector<std::vector<std::vector<PicrossCube>>> cubes;	//3D container of cubes. Access with XYZ
};

