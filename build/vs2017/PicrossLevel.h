#include <vector>
#include "PicrossCube.h"
#include "maths/vector4.h"
#include "graphics/renderer_3d.h"
#include "primitive_builder.h"
#include "assets/png_loader.h"
#include "graphics/image_data.h"
#include "graphics/texture.h"
#include <array>
#include "maths/vector2.h"
#include "maths/vector4.h"
#include "maths/matrix44.h"
#include "graphics/mesh.h"
#include "CollisionDetector.h"
#include <deque>
#include "PicrossStructs.hpp"

#pragma once
class PicrossLevel
{
public:
	PicrossLevel(PrimitiveBuilder* pBuilder, gef::Platform& platform);
	~PicrossLevel();

	void render(gef::Renderer3D* renderer);
	void setSpacing(float spacing);
	void setCameraPosPtr(gef::Vector4* cameraPos);

	void changeSelectedCube(int xDiff, int yDiff, int zDiff);
	bool selectCubeByTouch(gef::Vector2 screenSize, gef::Vector2 touchPos, gef::Matrix44 projectionMatrix, gef::Matrix44 viewMatrix, gef::Vector4& rayDirValues, bool mark, Picross::CubeCoords& coords);
	void resetCubeColours();
	void pushIntoLevel(int axis, bool reverseDirection, int amount = 1);	//Todo: Come up with better parameters
	bool destroyCube(Picross::CubeCoords coords);

private:
	
	float currentlySelectedCube[3];
	void updateRenderOrder();

	//Vars
	float levelScale;
	gef::Vector4 levelCenter;
	float cubeSideSize;
	float spacing;
	int lives;
	bool gameOver;


	//Cubes
	int rowSize;	//Width
	int columnSize;	//Height
	int depthSize;	//Depth
	int maxSizes[3];//The max sizes of each axis
	gef::Vector4 cubeSize;

	void initCubes(PrimitiveBuilder* pBuilder);

	std::vector<std::vector<std::vector<PicrossCube*>>> cubes;	//3D container of cubes. Access with XYZ, nullptr if cube was erased
	std::vector<std::pair<std::array<int, 3>, float>> renderOrder;	//Sorted vector of cubeIndexes by largest cube distance to camera to smallest 
	gef::Mesh* defaultCubeMesh;
	gef::Mesh* redCubeMesh;
	gef::Material** redMat;
	gef::Texture* tempTex;

	//Pushing Vars
	struct PushVars
	{
		PushVars() { pushed = false; pushAmount = 0; reversedPushDir = false; }
		bool pushed;
		int pushAmount;
		bool reversedPushDir;
	};
	PushVars pushVars[3]; //[x][y][z]
	gef::Vector4* cameraPos;
};

