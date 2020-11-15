#include "PicrossLevel.h"

PicrossLevel::PicrossLevel(PrimitiveBuilder* pBuilder)
{
	rowSize = 3;
	columnSize = 3;
	depthSize = 3;
	cubeSideSize = 25.0f;
	levelScale = 1.0f;
	spacing = 0.0f;

	cubeSize = gef::Vector4(cubeSideSize, cubeSideSize, cubeSideSize);



	levelCenter = gef::Vector4((static_cast<float>(rowSize) * cubeSideSize) / 2.0f, (static_cast<float>(columnSize) * cubeSideSize) / 2.0f, (static_cast<float>(depthSize) * cubeSideSize) / 2.0f);

	initCubes(pBuilder);
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

void PicrossLevel::initCubes(PrimitiveBuilder* pBuilder)
{
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
				temp.set_mesh(pBuilder->CreateBoxMesh(gef::Vector4(cubeSideSize / 2.0f, cubeSideSize / 2.0f, cubeSideSize / 2.0f)));

				//Store
				cubes[x][y].push_back(temp);
			}
		}
	}
}
