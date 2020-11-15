#include <vector>
#include "PicrossCube.h"
#include "maths/vector4.h"
#include "graphics/renderer_3d.h"
#include "primitive_builder.h"
#include "assets/png_loader.h"
#include "graphics/image_data.h"
#include "graphics/texture.h"

#pragma once
class PicrossLevel
{
public:
	PicrossLevel(PrimitiveBuilder* pBuilder, gef::Platform& platform);
	~PicrossLevel();

	void render(gef::Renderer3D* renderer);
	void setSpacing(float spacing);

	void changeSelectedCube(int xDiff, int yDiff, int zDiff);

private:
	
	float currentlySelectedCube[3];

	//Vars
	float levelScale;
	gef::Vector4 levelCenter;
	float cubeSideSize;
	float spacing;

	//Cubes
	int rowSize;	//Width
	int columnSize;	//Height
	int depthSize;	//Depth
	gef::Vector4 cubeSize;

	void initCubes(PrimitiveBuilder* pBuilder);

	std::vector<std::vector<std::vector<PicrossCube>>> cubes;	//3D container of cubes. Access with XYZ
	gef::Mesh* defaultCubeMesh;
	gef::Mesh* redCubeMesh;
	gef::Material** redMat;
	gef::Texture* tempTex;

	
};

