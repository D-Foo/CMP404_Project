#include "PicrossLevel.h"

PicrossLevel::PicrossLevel(PrimitiveBuilder* pBuilder, gef::Platform& platform)
{
	rowSize = 3;
	columnSize = 3;
	depthSize = 3;
	cubeSideSize = 25.0f;
	levelScale = 1.0f;
	spacing = 0.0f;

	cubeSize = gef::Vector4(cubeSideSize, cubeSideSize, cubeSideSize);
	currentlySelectedCube[0] = { 0 };
	currentlySelectedCube[1] = { 0 };
	currentlySelectedCube[2] = { 0 };


	//Create redMaterial
	redMat = new gef::Material*[6]();
	for (int i = 0; i < 6; i++)
	{
		redMat[i] = new gef::Material();
		redMat[i]->set_colour(0xff0000ff);
	}
	

	levelCenter = gef::Vector4((static_cast<float>(rowSize) * cubeSideSize) / 2.0f, (static_cast<float>(columnSize) * cubeSideSize) / 2.0f, (static_cast<float>(depthSize) * cubeSideSize) / 2.0f);

	initCubes(pBuilder);
	changeSelectedCube(2, 1, 0);
}

PicrossLevel::~PicrossLevel()
{
}

void PicrossLevel::render(gef::Renderer3D* renderer)
{
	for (int x = 0; x < rowSize; ++x)
	{
		for (int y = 0; y < columnSize; ++y)
		{
			for (int z = 0; z < depthSize; ++z)
			{
				renderer->DrawMesh(cubes[x][y][z]);
			}
		}
	}
}

void PicrossLevel::setSpacing(float spacing)
{
	for (int x = 0; x < rowSize; ++x)
	{
		for (int y = 0; y < columnSize; ++y)
		{
			for (int z = 0; z < depthSize; ++z)
			{
				cubes[x][y][z].setPosition(gef::Vector4((static_cast<float>(x) * cubeSideSize) + x * spacing, (static_cast<float>(y) * cubeSideSize) + y * spacing, (static_cast<float>(z) * cubeSideSize) + z * spacing) - levelCenter);
			}
		}
	}
}

//TODO: Will have to handle missing rows/cubes
void PicrossLevel::changeSelectedCube(int xDiff, int yDiff, int zDiff)
{
	//Change cube selection
	currentlySelectedCube[0] += xDiff;
	currentlySelectedCube[1] += yDiff;
	currentlySelectedCube[2] += zDiff;


	//Clamp
	float maxCubeSides[3] = { rowSize, columnSize, depthSize };
	for (int i = 0; i < 3; ++i)
	{
		if (currentlySelectedCube[i] < 0)
		{
			currentlySelectedCube[i] = 0;
		}
		else if (currentlySelectedCube[i] > maxCubeSides[i])
		{
			currentlySelectedCube[i] = maxCubeSides[i];
		}
	}

	//Change cube material
	

	cubes[currentlySelectedCube[0]][currentlySelectedCube[0]][currentlySelectedCube[0]].set_mesh(redCubeMesh);
}

void PicrossLevel::initCubes(PrimitiveBuilder* pBuilder)
{

	redCubeMesh = pBuilder->CreateBoxMesh(gef::Vector4(cubeSideSize / 2.0f, cubeSideSize / 2.0f, cubeSideSize / 2.0f), gef::Vector4(0.0f, 0.0f, 0.0f), redMat);

	bool spacingEnabled = true;

	for (int x = 0; x < rowSize; ++x)
	{
		cubes.push_back(std::vector<std::vector<PicrossCube>>());
		for (int y = 0; y < columnSize; ++y)
		{
			cubes[x].push_back(std::vector<PicrossCube>());
			for (int z = 0; z < depthSize; ++z)
			{
				//Create cube
				PicrossCube temp;
				if (!spacingEnabled)
				{
					spacing = 0.0f;
				}

				//Set postiion and mesh
				temp.setPosition(gef::Vector4((static_cast<float>(x) * cubeSideSize) + x * spacing, (static_cast<float>(y) * cubeSideSize) + y * spacing, (static_cast<float>(z) * cubeSideSize) + z * spacing) - levelCenter);
				defaultCubeMesh = pBuilder->CreateBoxMesh(gef::Vector4(cubeSideSize / 2.0f, cubeSideSize / 2.0f, cubeSideSize / 2.0f));

				temp.set_mesh(defaultCubeMesh);

				//Store
				cubes[x][y].push_back(temp);
			}
		}
	}
}
