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
	//changeSelectedCube(0, 0, 0);	
}

PicrossLevel::~PicrossLevel()
{
}

void PicrossLevel::render(gef::Renderer3D* renderer)
{
	

	for (auto& c : renderOrder)
	{
		renderer->DrawMesh(cubes[c.first[0]][c.first[1]][c.first[2]]);
	}


	/*for (int x = minRowShown; x < maxRowShown; ++x)
	{
		for (int y = 0; y < columnSize; ++y)
		{
			for (int z = 0; z < depthSize; ++z)
			{
				renderer->DrawMesh(cubes[x][y][z]);
			}
		}
	}*/
}

void PicrossLevel::setSpacing(float spacing, gef::Vector4 cameraPos)
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
	updateRenderOrder(cameraPos);
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
	

	cubes[currentlySelectedCube[0]][currentlySelectedCube[1]][currentlySelectedCube[2]].set_mesh(redCubeMesh);
}

gef::Vector4 vec4Mat44Mult(gef::Vector4 v, gef::Matrix44 m)
{
	gef::Vector4 ret;
	ret.set_x((v.x() * m.GetRow(0).x()) + (v.y() * m.GetRow(0).y()) + (v.z() * m.GetRow(0).z()) + (v.w() * m.GetRow(0).w()));
	ret.set_y((v.x() * m.GetRow(1).x()) + (v.y() * m.GetRow(1).y()) + (v.z() * m.GetRow(1).z()) + (v.w() * m.GetRow(1).w()));
	ret.set_z((v.x() * m.GetRow(2).x()) + (v.y() * m.GetRow(2).y()) + (v.z() * m.GetRow(2).z()) + (v.w() * m.GetRow(2).w()));
	ret.set_w((v.x() * m.GetRow(3).x()) + (v.y() * m.GetRow(3).y()) + (v.z() * m.GetRow(3).z()) + (v.w() * m.GetRow(3).w()));
	return ret;
}


void PicrossLevel::selectCubeByTouch(gef::Vector2 screenSize, gef::Vector2 touchPos, gef::Matrix44 projectionMatrix, gef::Matrix44 viewMatrix, gef::Vector4 cameraPos, gef::Vector4& rayDirValues)
{
	//Reference
	//https://www.rastertek.com/dx11tut47.html
	//https://antongerdelan.net/opengl/raycasting.html
	//https://stackoverflow.com/questions/19150215/3d-screenspace-raycasting-picking-directx9


	//Can check for collision with reverse render order

	//Create Ray from Touch

	//Convert from touch pos to device space -1.0f, 1.0f
	gef::Vector4 rayPos;
	rayPos.set_x(((2 * touchPos.x) / screenSize.x) - 1.0f);
	rayPos.set_y(((2 * touchPos.y) / screenSize.y - 1.0f) * -1.0f);
	rayPos.set_z(-1.0f);
	rayPos.set_w(1.0f);

	//Adjust using projection matrix
	rayPos.set_x(rayPos.x() / projectionMatrix.GetRow(0).x());
	rayPos.set_y(rayPos.y() / projectionMatrix.GetRow(1).y());
	gef::Matrix44 inverseProjectionMatrix = projectionMatrix;
	inverseProjectionMatrix.Inverse(inverseProjectionMatrix);

	//Create Inverse View Matrix
	gef::Matrix44 inverseViewMatrix = viewMatrix;
	inverseViewMatrix.Inverse(inverseViewMatrix);

	//Get Ray Direction
	gef::Vector4 rayDirection;
	rayDirection.set_x((rayPos.x() * inverseViewMatrix.GetRow(0).x()) + (rayPos.y() * inverseViewMatrix.GetRow(1).x()) + inverseViewMatrix.GetRow(2).x());
	rayDirection.set_y((rayPos.x() * inverseViewMatrix.GetRow(0).y()) + (rayPos.y() * inverseViewMatrix.GetRow(1).y()) + inverseViewMatrix.GetRow(2).y());
	rayDirection.set_z((rayPos.x() * inverseViewMatrix.GetRow(0).z()) + (rayPos.y() * inverseViewMatrix.GetRow(1).z()) + inverseViewMatrix.GetRow(2).z());
	rayDirection.set_w(1.0f);

	rayDirection.set_x(-rayDirection.x());
	rayDirection.set_y(-rayDirection.y());

	rayDirValues = rayDirection;

	//Ray to Cube Check
	//Cubes sorted by closest to camera
	for (size_t i = renderOrder.size() - 1; i > 0 ; --i) 
	{
		PicrossCube* cube = &cubes[renderOrder[i].first[0]][renderOrder[i].first[1]][renderOrder[i].first[2]];

		//Get inverse worldMatrix
		gef::Matrix44 worldMatrix;// = cube->transform();
		worldMatrix.SetIdentity();
		gef::Matrix44 inverseWorldMatrix = worldMatrix;
		inverseWorldMatrix.Inverse(inverseWorldMatrix);

		//Transform ray origin and direction from view space to world space
		gef::Vector4 rayOrigin = cameraPos;
		rayOrigin = rayOrigin.Transform(inverseWorldMatrix);
		rayDirection = rayDirection.TransformW(inverseWorldMatrix);
		rayDirection.Normalise();
		
		//Check for ray + cube intersection
		gef::Vector4 bottomLeft;
		bottomLeft.set_x(cube->transform().GetRow(3).x() + cube->mesh()->aabb().min_vtx().x());
		bottomLeft.set_y(cube->transform().GetRow(3).y() + cube->mesh()->aabb().min_vtx().y());
		bottomLeft.set_z(cube->transform().GetRow(3).z() + cube->mesh()->aabb().min_vtx().z());	
		/*bottomLeft.set_x(cube->mesh()->aabb().min_vtx().x() - cube->transform().GetRow(3).x());
		bottomLeft.set_y(cube->mesh()->aabb().min_vtx().y() - cube->transform().GetRow(3).y());
		bottomLeft.set_z(cube->mesh()->aabb().min_vtx().z() - cube->transform().GetRow(3).z());*/

		bottomLeft.set_w(1.0f);

		gef::Vector4 topRight;
		topRight.set_x(cube->transform().GetRow(3).x() + cube->mesh()->aabb().max_vtx().x());
		topRight.set_y(cube->transform().GetRow(3).y() + cube->mesh()->aabb().max_vtx().y());
		topRight.set_z(cube->transform().GetRow(3).z() + cube->mesh()->aabb().max_vtx().z());
	/*	topRight.set_x(cube->mesh()->aabb().max_vtx().x() - cube->transform().GetRow(3).x());
		topRight.set_y(cube->mesh()->aabb().max_vtx().y() - cube->transform().GetRow(3).y());
		topRight.set_z(cube->mesh()->aabb().max_vtx().z() - cube->transform().GetRow(3).z());*/
		topRight.set_w(1.0f);

		bool intersection = CollisionDetector::rayCube2(rayDirection, rayOrigin, bottomLeft, topRight);
		if (intersection)
		{
			cube->set_mesh(redCubeMesh);
			break;
		}
	}

	
}

void PicrossLevel::resetCubeColours()
{
	for (auto& x : cubes)
	{
		for (auto& y : x)
		{
			for (auto& z : y)
			{
				z.set_mesh(defaultCubeMesh);
			}
		}
	}
}

void PicrossLevel::updateRenderOrder(gef::Vector4 cameraPos)
{
	//Setup
	renderOrder.clear();
	int maxSizes[3] = { rowSize, columnSize, depthSize };						//The max sizes of each axis
	std::pair<int, int> minMaxMembersShown[3] = { std::pair<int, int>(0, 0) };	//Where to start and stop rendering each axis

	//Check for pushing and adjust what cubes are rendered
	for (int i = 0; i < 3; ++i)
	{
		if (pushVars[i].pushed)
		{
			if (pushVars[i].reversedPushDir)
			{
				minMaxMembersShown[i].first = 0 + pushVars[i].pushAmount;
				minMaxMembersShown[i].second = maxSizes[i];
			}
			else
			{
				minMaxMembersShown[i].first = 0;
				minMaxMembersShown[i].second = maxSizes[i] - pushVars[i].pushAmount;
			}
		}
		else
		{
			minMaxMembersShown[i].first = 0;
			minMaxMembersShown[i].second = maxSizes[i];
		}
	}

	//Create render order
	for (int x = minMaxMembersShown[0].first; x < minMaxMembersShown[0].second; ++x)
	{
		for (int y = minMaxMembersShown[1].first; y < minMaxMembersShown[1].second; ++y)
		{
			for (int z = minMaxMembersShown[2].first; z < minMaxMembersShown[2].second; ++z)
			{
				float xDiff, yDiff, zDiff;

				xDiff = cubes[x][y][z].getPosition().x() - cameraPos.x();
				yDiff = cubes[x][y][z].getPosition().y() - cameraPos.y();
				zDiff = cubes[x][y][z].getPosition().z() - cameraPos.z();

				float distance = abs(xDiff) + abs(yDiff) + abs(zDiff);
				//if renderOrder not empty
				if (!renderOrder.empty())
				{
					//Iterate through and find a distance smaller than the current distance and insert it before it
					for (std::vector<std::pair<std::array<int, 3>, float>>::iterator it = renderOrder.begin(); it != renderOrder.end(); ++it)
					{
						if (it->second < distance)
						{
							renderOrder.insert(it, std::pair<std::array<int, 3>, float>(std::array<int, 3>{x, y, z}, distance));
							break;
						}
						else if (it == renderOrder.end() - 1)
						{
							renderOrder.push_back(std::pair<std::array<int, 3>, float>(std::array<int, 3>{x, y, z}, distance));
							break;
						}
					}
				}
				else
				{
					renderOrder.push_back(std::pair<std::array<int, 3>, float>(std::array<int, 3>{x, y, z}, distance));
				}
			}
		}
	}
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
