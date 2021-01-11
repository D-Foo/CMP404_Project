#include "PicrossLevel.h"

PicrossLevel::PicrossLevel(PrimitiveBuilder* pBuilder, gef::Platform& platform, std::vector<std::vector<std::vector<bool>>> shape)
{
	this->shape = shape;
	rowSize = this->shape.size();
	columnSize = this->shape[0].size();
	depthSize = this->shape[0][0].size();
	maxSizes[0] = rowSize;
	maxSizes[1] = columnSize;
	maxSizes[2] = depthSize;
	minMaxMembersShown[0] = std::pair<int, int>(0, 0);
	minMaxMembersShown[1] = std::pair<int, int>(0, 0);
	minMaxMembersShown[2] = std::pair<int, int>(0, 0);


	cubeSideSize = 25.0f;
	levelScale = 1.0f;
	spacing = 0.0f;

	cubeSize = gef::Vector4(cubeSideSize, cubeSideSize, cubeSideSize);
	currentlySelectedCube[0] = { 0 };
	currentlySelectedCube[1] = { 0 };
	currentlySelectedCube[2] = { 0 };

	cameraPos = nullptr;

	//Create redMaterial
	redMat = new gef::Material*[6]();
	blueMat = new gef::Material*[6]();
	for (int i = 0; i < 6; i++)
	{
		redMat[i] = new gef::Material();
		redMat[i]->set_colour(0xff0000ff);
		blueMat[i] = new gef::Material();
		blueMat[i]->set_colour(0xffff0000);
	}
	

	levelCenter = gef::Vector4((static_cast<float>(rowSize) * cubeSideSize) / 2.0f, (static_cast<float>(columnSize) * cubeSideSize) / 2.0f, (static_cast<float>(depthSize) * cubeSideSize) / 2.0f);

	initCubes(pBuilder);
	//changeSelectedCube(0, 0, 0);	
	lives = 5;
	gameOver = false;
}

PicrossLevel::~PicrossLevel()
{
}

void PicrossLevel::renderLevel(gef::Renderer3D* renderer)
{
	/*for (auto& c : cubeRenderOrder)
	{
		if (cubes[c.first[0]][c.first[1]][c.first[2]] != nullptr)
		{
			renderer->DrawMesh(*cubes[c.first[0]][c.first[1]][c.first[2]]);
		}
	}*/
	for (auto& m : renderOrder)
	{
		renderer->DrawMesh(*m.first);
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

void PicrossLevel::updateNumbers(int numNumbers, std::pair<gef::Scene*, gef::MeshInstance*>* numberMeshes, gef::Vector4 cameraPos)
{
	for (auto& n : this->numbers)
	{
		delete(n.first);
	}
	this->numbers.clear();

	//Track number of final object cubes in each ...
	std::vector<std::vector<int>> rowSums;
	std::vector<std::vector<int>> columnSums;
	std::vector<std::vector<int>> depthSums;

	//TODO: REPLACE XYZ = 0 & COLUMNSIZE ETC. WITH THESE
	int xStart = minMaxMembersShown[0].first;
	int xEnd = minMaxMembersShown[0].second;

	int yStart = minMaxMembersShown[1].first;
	int yEnd = minMaxMembersShown[1].second;

	int zStart = minMaxMembersShown[2].first;
	int zEnd = minMaxMembersShown[2].second;

	for (int y = yStart; y < yEnd; ++y)
	{
		rowSums.push_back(std::vector<int>());
		for(int z = 0; z < depthSize; ++z)
		{
			rowSums[y].push_back(0);
		}
	}
	for (int x = xStart; x < xEnd; ++x)
	{
		columnSums.push_back(std::vector<int>());
		for (int z = zStart; z < zEnd; ++z)
		{
			columnSums[x].push_back(0);
		}
	}
	for (int x = xStart; x < xEnd; ++x)
	{
		depthSums.push_back(std::vector<int>());
		for (int y = yStart; y < yEnd; ++y)
		{
			depthSums[x].push_back(0);
		}
	}

	//Get finalObject cubes and sum
	for (size_t i = xStart; i < xEnd; ++i)
	{
		for (size_t j = yStart; j < yEnd; ++j)
		{
			for (size_t k = zStart; k < zEnd; ++k)
			{
				if (cubes[i][j][k] != nullptr)
				{
					if (cubes[i][j][k]->getFinalObject())
					{
						++rowSums[j][k];
						++columnSums[i][k];
						++depthSums[i][j];
					}
				}
			}
		}
	}

	//Render on side facing toward camera
	bool left = false;
	bool bottom = false;
	bool front = false;

	cameraPos.x() < levelCenter.x() ? left = true : left = false;
	cameraPos.y() < levelCenter.y() ? bottom = true : bottom = false;
	cameraPos.z() < levelCenter.z() ? front = true : front = false;

	//TODO: SOOOO MUCH DUPLICATE CODE COME UP WITH TIDIER SOLUTION 

	//Get closest cube in each row/column/depth to camera and render number at it's pos
	PicrossCube* closestCube = nullptr;
	PicrossCube* lowestCube = nullptr;
	int lowestCubeNum = -1;
	PicrossCube* highestCube = nullptr;
	float closestDist = -1.0f;

	//ROWS
	for (int y = yStart; y < yEnd; ++y)
	{
		for (int z = zStart; z < zEnd; ++z)
		{
			//If row contains a cube that's part of the final object
			if (rowSums[y][z] != 0)
			{
				//Get end cubes on this row 
				closestCube = nullptr;
				lowestCube = nullptr;
				highestCube = nullptr;

				//Get low cube end
				for (int x = xStart; x < xEnd; ++x)
				{
					if (cubes[x][y][z] != nullptr)
					{
						/*if (closestCube == nullptr)
						{
							closestCube = cubes[x][y][z];
						
						}
						else
						{
							if ((cameraPos - closestCube->getPosition()).Length() < closestDist)
							{
								closestDist = (cameraPos - closestCube->getPosition()).Length();
								closestCube = cubes[x][y][z];
							}
						}*/
						lowestCube = cubes[x][y][z];
						lowestCubeNum = x;
						break;
					}
				}

				//Get high cube end
				for (int x = xEnd - 1; x >= xStart; --x)
				{
					if (cubes[x][y][z] != nullptr)
					{
						//Check if different to lowestCube
						if (x > lowestCubeNum)
						{
							highestCube = cubes[x][y][z];
							float lowDist = (cameraPos - lowestCube->getPosition()).Length();
							float highDist = (cameraPos - highestCube->getPosition()).Length();
							if (lowDist < highDist)
							{
								closestCube = lowestCube; 
								closestDist = lowDist;

							}
							else
							{
								closestCube = highestCube;
								closestDist = highDist;
							}
							break;
						}
						else
						{
							closestCube = lowestCube;
							closestDist = (cameraPos - lowestCube->getPosition()).Length();
							break;
						}
					}
				}

				if (closestCube != nullptr)
				{
					addNumber(closestCube, numberMeshes, rowSums[y][z] - 1, true, false, false, left, closestDist);
				}
			}
		}
	}

	//COLUMNS
	for (int x = xStart; x < xEnd; ++x)
	{
		for (int z = zStart; z < zEnd; ++z)
		{
			//If column contains a cube that's part of the final object
			if (columnSums[x][z] != 0)
			{
				//Get end cubes on this row 
				closestCube = nullptr;
				lowestCube = nullptr;
				highestCube = nullptr;

				//Get low cube end
				for (int y = yStart; y < yEnd; ++y)
				{
					if (cubes[x][y][z] != nullptr)
					{
						lowestCube = cubes[x][y][z];
						lowestCubeNum = y;
						break;
					}
				}

				//Get high cube end
				for (int y = yEnd - 1; y >= yStart; --y)
				{
					if (cubes[x][y][z] != nullptr)
					{
						//Check if different to lowestCube
						if (y > lowestCubeNum)
						{
							highestCube = cubes[x][y][z];
							float lowDist = (cameraPos - lowestCube->getPosition()).Length();
							float highDist = (cameraPos - highestCube->getPosition()).Length();
							if (lowDist < highDist)
							{
								closestCube = lowestCube;
								closestDist = lowDist;

							}
							else
							{
								closestCube = highestCube;
								closestDist = highDist;
							}
							break;
						}
						else
						{
							closestCube = lowestCube;
							closestDist = (cameraPos - lowestCube->getPosition()).Length();
							break;
						}
					}
				}

				if (closestCube != nullptr)
				{
					addNumber(closestCube, numberMeshes, columnSums[x][z] - 1, false, true, false, bottom, closestDist);
				}
			}
		}
	}

	//DEPTH
	for (int x = xStart; x < xEnd; ++x)
	{
		for (int y = yStart; y < yEnd; ++y)
		{
			//If depth contains a cube that's part of the final object
			if (depthSums[x][y] != 0)
			{
				//Get end cubes on this row 
				closestCube = nullptr;
				lowestCube = nullptr;
				highestCube = nullptr;

				//Get low cube end
				for (int z = zStart; z < zEnd; ++z)
				{
					if (cubes[x][y][z] != nullptr)
					{
						lowestCube = cubes[x][y][z];
						lowestCubeNum = z;
						break;
					}
				}

				//Get high cube end
				for (int z = zEnd - 1; z >= zStart; --z)
				{
					if (cubes[x][y][z] != nullptr)
					{
						//Check if different to lowestCube
						if (z > lowestCubeNum)
						{
							highestCube = cubes[x][y][z];
							float lowDist = (cameraPos - lowestCube->getPosition()).Length();
							float highDist = (cameraPos - highestCube->getPosition()).Length();
							if (lowDist < highDist)
							{
								closestCube = lowestCube;
								closestDist = lowDist;

							}
							else
							{
								closestCube = highestCube;
								closestDist = highDist;
							}
							break;
						}
						else
						{
							closestCube = lowestCube;
							closestDist = (cameraPos - lowestCube->getPosition()).Length();
							break;
						}
					}
				}

				if (closestCube != nullptr)
				{
					addNumber(closestCube, numberMeshes, depthSums[x][y] - 1, false, false, true, front, closestDist);
				}
			}
		}
	}

	updateRenderOrder();
}

void PicrossLevel::setSpacing(float spacing)
{
	for (int x = 0; x < rowSize; ++x)
	{
		for (int y = 0; y < columnSize; ++y)
		{
			for (int z = 0; z < depthSize; ++z)
			{
				if (cubes[x][y][z] != nullptr)
				{
					cubes[x][y][z]->setPosition(gef::Vector4((static_cast<float>(x) * cubeSideSize) + x * spacing, (static_cast<float>(y) * cubeSideSize) + y * spacing, (static_cast<float>(z) * cubeSideSize) + z * spacing) - levelCenter);
				}
			}
		}
	}

	updateRenderOrder();
}

void PicrossLevel::setCameraPosPtr(gef::Vector4* cameraPos)
{
	this->cameraPos = cameraPos;
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
	cubes[currentlySelectedCube[0]][currentlySelectedCube[1]][currentlySelectedCube[2]]->set_mesh(redCubeMesh);
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


bool PicrossLevel::selectCubeByTouch(gef::Vector2 screenSize, gef::Vector2 touchPos, gef::Matrix44 projectionMatrix, gef::Matrix44 viewMatrix, gef::Vector4& rayDirValues, bool mark, Picross::CubeCoords& coords)
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

	rayDirection.Normalise();

	rayDirection.set_x(-rayDirection.x());
	rayDirection.set_y(-rayDirection.y());

	rayDirValues = rayDirection;

	//Ray to Cube Check
	//Cubes sorted by closest to camera
	for (size_t i = cubeRenderOrder.size() - 1; i > 0 ; --i) 
	{
		PicrossCube* cube = cubes[cubeRenderOrder[i].first[0]][cubeRenderOrder[i].first[1]][cubeRenderOrder[i].first[2]];

		if (cube != nullptr)
		{
			//Get inverse worldMatrix
			gef::Matrix44 worldMatrix;// = cube->transform();
			worldMatrix.SetIdentity();
			gef::Matrix44 inverseWorldMatrix = worldMatrix;
			inverseWorldMatrix.Inverse(inverseWorldMatrix);

			//Transform ray origin and direction from view space to world space
			gef::Vector4 rayOrigin = *cameraPos;
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
				if (mark)
				{
					cube->set_mesh(redCubeMesh);
				}
				coords = cube->getCoords();
				return true;
			}
		}
	}

	return false;
	
}

bool PicrossLevel::selectCubeByTouch2(gef::Vector2 screenSize, gef::Vector2 touchPos, gef::Matrix44 projectionMatrix, gef::Matrix44 viewMatrix, gef::Vector4& rayDirValues, bool mark, Picross::CubeCoords& coords, float ndczmin)
{
	gef::Vector4 startPoint, direction;
	GetScreenPosRay(touchPos, projectionMatrix, viewMatrix, startPoint, direction, screenSize.x, screenSize.y, ndczmin);
	return RayCubeIntersect(startPoint, direction, coords);
}

void PicrossLevel::resetCubeColours()
{
	for (auto& x : cubes)
	{
		for (auto& y : x)
		{
			for (auto& z : y)
			{
				if (z != nullptr)
				{
					z->set_mesh(defaultCubeMesh);
				}
			}
		}
	}
}

void PicrossLevel::pushIntoLevel(int axis, bool reverseDirection, int amount)	//Todo replcae with (int axis, bool reverseDirection, int amount)
{
	//Make sure valid function call
	
	if (axis < 0 || axis > 2)
	{
		return;
	}

	int index = axis;

	PushVars* pVars = &pushVars[index];
	//Check if already pushed
	if (pVars->pushed)
	{
		//Update push
		pVars->pushAmount = amount;
		//If push amount == 0 
		if (pVars->pushAmount == 0)
		{
			//Toggle pushed
			pVars->pushed = false;
			pVars->reversedPushDir = false;
		}
		
	}	
	//If new push
	else
	{
		//Set pushed
		pVars->pushed = true;
		pVars->pushAmount = amount;
		pVars->reversedPushDir = reverseDirection;
	}

	if (pVars->pushed)
	{
		//Clamp pushed to between 0 and maxSizes
		if (pVars->pushAmount < 0)
		{
			pVars->pushAmount = 0;
		}
		else if (pVars->pushAmount >= maxSizes[index])
		{
			pVars->pushAmount = maxSizes[index] - 1;
		}
	}
	updateRenderOrder();
}

bool PicrossLevel::destroyCube(Picross::CubeCoords coords)
{
	int x = coords.x;
	int y = coords.y;
	int z = coords.z;

	if (cubes[x][y][z] != nullptr)
	{
		
		if (!cubes[x][y][z]->getProtected())
		{
			//Check if cube is part of the final shape of the level
			if (cubes[x][y][z]->getFinalObject())
			{
				//Lose a life
				--lives;
				if (lives == 0)
				{
					gameOver = true;
					return false;
				}
			}
			else
			{
				//Delete cube
				//cubes[x][y].erase(cubes[x][y].begin() + z);
				delete(cubes[x][y][z]);
				cubes[x][y][z] = nullptr;

				bool levelClear = true;

				//Check if only final objects left
				for (auto& v1 : cubes)
				{
					for (auto& v2 : v1)
					{
						for (auto& c : v2)
						{
							if (c != nullptr)
							{


								if (!c->getFinalObject())
								{
									levelClear = false;
									break;
								}
							}
						}
					}
				}
				if (levelClear)
				{
					//TODO: return enum?
				}
			}
		}
	}
	return true;
	
}

void PicrossLevel::GetScreenPosRay(const gef::Vector2& screen_position, const gef::Matrix44& projection, const gef::Matrix44& view, gef::Vector4& start_point, gef::Vector4& direction, float screen_width, float screen_height, float ndc_z_min)
{
	gef::Vector2 ndc;

	float hw = screen_width * 0.5f;
	float hh = screen_height * 0.5f;

	ndc.x = (static_cast<float>(screen_position.x) - hw) / hw;
	ndc.y = (hh - static_cast<float>(screen_position.y)) / hh;

	gef::Matrix44 projectionInverse;
	projectionInverse.Inverse(view * projection);

	gef::Vector4 nearPoint, farPoint;

	nearPoint = gef::Vector4(ndc.x, ndc.y, ndc_z_min, 1.0f).TransformW(projectionInverse);
	farPoint = gef::Vector4(ndc.x, ndc.y, 1.0f, 1.0f).TransformW(projectionInverse);

	nearPoint /= nearPoint.w();
	farPoint /= farPoint.w();

	start_point = gef::Vector4(nearPoint.x(), nearPoint.y(), nearPoint.z());
	direction = farPoint - nearPoint;
	direction.Normalise();
}

bool PicrossLevel::RayCubeIntersect(const gef::Vector4& start_point, gef::Vector4 rayDirection, Picross::CubeCoords& cubeCoords)
{
	bool mark = false;
	for (size_t i = cubeRenderOrder.size() - 1; i > 0; --i)
	{
		PicrossCube* cube = cubes[cubeRenderOrder[i].first[0]][cubeRenderOrder[i].first[1]][cubeRenderOrder[i].first[2]];

		if (cube != nullptr)
		{
			//Get inverse worldMatrix
			gef::Matrix44 worldMatrix;// = cube->transform();
			worldMatrix.SetIdentity();
			gef::Matrix44 inverseWorldMatrix = worldMatrix;
			inverseWorldMatrix.Inverse(inverseWorldMatrix);

			//Transform ray origin and direction from view space to world space
			gef::Vector4 rayOrigin = *cameraPos;
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
				if (mark)
				{
					cube->set_mesh(redCubeMesh);
				}
				cubeCoords = cube->getCoords();
				return true;
			}
		}
	}

	return false;
}

void PicrossLevel::updateRenderOrder()
{
	//Setup
	cubeRenderOrder.clear();
	renderOrder.clear();
	
	minMaxMembersShown[0] = std::pair<int, int>(0, 0);
	minMaxMembersShown[1] = std::pair<int, int>(0, 0);	
	minMaxMembersShown[2] = std::pair<int, int>(0, 0);	

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
				if (cubes[x][y][z] != nullptr)
				{
					float xDiff, yDiff, zDiff;

					xDiff = cubes[x][y][z]->getPosition().x() - cameraPos->x();
					yDiff = cubes[x][y][z]->getPosition().y() - cameraPos->y();
					zDiff = cubes[x][y][z]->getPosition().z() - cameraPos->z();

					float distance = abs(xDiff) + abs(yDiff) + abs(zDiff);
					//if renderOrder not empty
					if (!cubeRenderOrder.empty())
					{
						//Iterate through and find a distance smaller than the current distance and insert it before it
						for (std::vector<std::pair<std::array<int, 3>, float>>::iterator it = cubeRenderOrder.begin(); it != cubeRenderOrder.end(); ++it)
						{
							if (it->second < distance)
							{
								cubeRenderOrder.insert(it, std::pair<std::array<int, 3>, float>(std::array<int, 3>{x, y, z}, distance));
								break;
							}
							else if (it == cubeRenderOrder.end() - 1)
							{
								cubeRenderOrder.push_back(std::pair<std::array<int, 3>, float>(std::array<int, 3>{x, y, z}, distance));
								break;
							}
						}
					}
					else
					{
						cubeRenderOrder.push_back(std::pair<std::array<int, 3>, float>(std::array<int, 3>{x, y, z}, distance));
					}
				}
			}
		}
	}

	//Create a combination of cubes and numbers to sort through and save as combined renderorder
	std::vector<std::pair<gef::MeshInstance*, float>> unsortedOrder;
	for (auto& c : cubeRenderOrder)
	{
		unsortedOrder.push_back(std::pair<gef::MeshInstance*, float>(cubes[c.first[0]][c.first[1]][c.first[2]], c.second));
	}
	for (auto& n : numbers)
	{
		unsortedOrder.push_back(std::pair<gef::MeshInstance*, float>(n.first, n.second));
	}

	//Add each element from unsorted order
	for (size_t i = 0; i < unsortedOrder.size(); ++i)
	{
		if (!renderOrder.empty())
		{
			//Iterate through and find a distance smaller than the current distance and insert it before it
			for (std::vector<std::pair<gef::MeshInstance*, float>>::iterator it = renderOrder.begin(); it != renderOrder.end(); ++it)
			{
				if (it->second < unsortedOrder[i].second)
				{
					renderOrder.insert(it, unsortedOrder[i]);
					break;
				}
				else if (it == renderOrder.end() - 1)
				{
					renderOrder.push_back(unsortedOrder[i]);
					break;
				}
			}
		}
		else
		{
			renderOrder.push_back(unsortedOrder[i]);
		}
	}

}

void PicrossLevel::toggleCubeProtected(Picross::CubeCoords coords)
{
	cubes[coords.x][coords.y][coords.z]->setProtected(!cubes[coords.x][coords.y][coords.z]->getProtected());
	if (cubes[coords.x][coords.y][coords.z]->getProtected())
	{
		cubes[coords.x][coords.y][coords.z]->set_mesh(blueCubeMesh);
	}
	else
	{
		cubes[coords.x][coords.y][coords.z]->set_mesh(defaultCubeMesh);
	}
}

void PicrossLevel::initCubes(PrimitiveBuilder* pBuilder)
{

	redCubeMesh = pBuilder->CreateBoxMesh(gef::Vector4(cubeSideSize / 2.0f, cubeSideSize / 2.0f, cubeSideSize / 2.0f), gef::Vector4(0.0f, 0.0f, 0.0f), redMat);
	blueCubeMesh = pBuilder->CreateBoxMesh(gef::Vector4(cubeSideSize / 2.0f, cubeSideSize / 2.0f, cubeSideSize / 2.0f), gef::Vector4(0.0f, 0.0f, 0.0f), blueMat);

	bool spacingEnabled = true;

	for (int x = 0; x < rowSize; ++x)
	{
		//Try emplace front
		cubes.push_back(std::vector<std::vector<PicrossCube*>>());
		for (int y = 0; y < columnSize; ++y)
		{
			cubes[x].push_back(std::vector<PicrossCube*>());
			for (int z = 0; z < depthSize; ++z)
			{
				//Create cube
				PicrossCube* temp = new PicrossCube(Picross::CubeCoords(x, y, z));
				if (!spacingEnabled)
				{
					spacing = 0.0f;
				}

				//Set postiion and mesh
				temp->setPosition(gef::Vector4((static_cast<float>(x) * cubeSideSize) + x * spacing, (static_cast<float>(y) * cubeSideSize) + y * spacing, (static_cast<float>(z) * cubeSideSize) + z * spacing) - levelCenter);
				defaultCubeMesh = pBuilder->CreateBoxMesh(gef::Vector4(cubeSideSize / 2.0f, cubeSideSize / 2.0f, cubeSideSize / 2.0f));

				temp->set_mesh(defaultCubeMesh);

				//Store
				cubes[x][y].push_back(temp);
				cubes[x][y][z]->setFinalObject(shape[x][y][z]);
				if (cubes[x][y][z]->getFinalObject())
				{
					cubes[x][y][z]->set_mesh(redCubeMesh);
				}
			}
		}
	}
}

void PicrossLevel::addNumber(PicrossCube* closestCube, std::pair<gef::Scene*, gef::MeshInstance*>* numberMeshes, int numberNum, bool row, bool column, bool depth, bool towardCamera, float distanceFromCamera)
{
	//Setup number transform
	gef::Matrix44 finalTransform = gef::Matrix44::kIdentity;
	gef::Matrix44 rotMatrix1 = gef::Matrix44::kIdentity;
	gef::Matrix44 rotMatrix2 = gef::Matrix44::kIdentity;
	gef::Matrix44 scaleMatrix = gef::Matrix44::kIdentity;
	gef::Matrix44 transformMatrix = gef::Matrix44::kIdentity;
	float scaleF = 0.1f;
	scaleMatrix.Scale(gef::Vector4(scaleF, scaleF, scaleF, 1.0f));

	//Rotate number
	float rotF = 90.0f;
	rotMatrix1.RotationZ(gef::DegToRad(rotF));
	//if (left)
	//{
	//	rotMatrix2.RotationY(gef::DegToRad(180.0f));
	//}


	//Place on appropriate side of cubes
	gef::Vector4 translation = closestCube->getPosition();
	if (row)
	{
		if (towardCamera)
		{
			translation -= gef::Vector4(cubeSideSize * 0.5f, 0.0f, 0.0f);
			rotMatrix2.RotationY(gef::DegToRad(180.0f));
		}
		else
		{
			translation += gef::Vector4(cubeSideSize * 0.5f, 0.0f, 0.0f);
		}
	}
	else if (column)
	{
		if (towardCamera)
		{
			rotMatrix1.RotationZ(gef::DegToRad(180.0f));
			translation -= gef::Vector4(0.0f, cubeSideSize * 0.5f, 0.0f);
		}
		else
		{
			rotMatrix1 = gef::Matrix44::kIdentity;
			translation += gef::Vector4(0.0f, cubeSideSize * 0.5f, 0.0f);
		}
	}
	else if (depth)
	{
		if (towardCamera)
		{
			translation -= gef::Vector4(0.0f, 0.0f, cubeSideSize * 0.5f);
			rotMatrix2.RotationY(gef::DegToRad(90.0f));
		}
		else
		{
			translation += gef::Vector4(0.0f, 0.0f, cubeSideSize * 0.5f);
			rotMatrix2.RotationY(gef::DegToRad(270.0f));
		}
	}
	rotMatrix1 = rotMatrix1 * rotMatrix2;
	transformMatrix.SetTranslation(translation);
	finalTransform = scaleMatrix * rotMatrix1 * transformMatrix;
	//numberMeshes[numberNum].second->set_transform(finalTransform);

	//Render appropriate number
	//TODO: REPLACE WITH ADD TO RENDER ORDER
	//renderer->DrawMesh(*numbers[rowSums[y][z] - 1].second);
	gef::MeshInstance* temp = new gef::MeshInstance();
	temp->set_mesh(numberMeshes[numberNum].second->mesh());
	temp->set_transform(finalTransform);
	numbers.push_back(std::pair<gef::MeshInstance*, float>(temp, distanceFromCamera));
}
