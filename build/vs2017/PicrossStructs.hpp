#pragma once
#include <vector>


namespace Picross
{
	struct CubeCoords
	{
		CubeCoords()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		CubeCoords(int x, int y, int z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		int x, y, z;

		bool operator==(CubeCoords const& c2) const
		{
			if (x == c2.x && y == c2.y && z == c2.z)
			{
				return true;
			}

			return false;
		}
	};

	static std::vector<std::vector<std::vector<bool>>> getLayout1(std::vector<CubeCoords> shape = std::vector<CubeCoords>(), int scale = 5, bool customDimensions = false, CubeCoords dimensions = CubeCoords(5, 5, 5))
	{
		CubeCoords size;

		if (customDimensions)
		{
			//Check dimensions are valid
			size = dimensions;
			if (size.x < 1)	{ size.x = 1; }
			if (size.y < 1) { size.y = 1; }
			if (size.z < 1) { size.z = 1; }
		}
		else
		{
			//Check scale is valid
			if (scale < 1)
			{
				scale = 1;
			}
			size = CubeCoords(scale, scale, scale);
		}
		
		if (shape.empty())
		{
			//Default shape
			shape = { CubeCoords(1,1,1),  CubeCoords(2,2,2) };
		}
		else
		{
			//Check given shape is valid
			for (auto& s : shape)
			{
				if (s.x > size.x - 1) { s.x = size.x - 1; }
				if (s.y > size.y - 1) { s.y = size.y - 1; }
				if (s.z > size.z - 1) { s.z = size.z - 1; }
			}
		}

		std::vector<std::vector<std::vector<bool>>> ret;

		//Fill vector to size with false
		for (int i = 0; i < size.x; ++i)
		{
			ret.push_back(std::vector<std::vector<bool>>());
			for (int j = 0; j < size.y; ++j)
			{
				ret[i].push_back(std::vector<bool>());
				{
					for (int k = 0; k < size.z; ++k)
					{
						bool fillValue = false;
						for (auto& c : shape)
						{
							CubeCoords temp = CubeCoords(i, j, k);
							if (temp == c)
							{
								fillValue = true;
								break;
							}
						}
						ret[i][j].push_back(fillValue);
					}
				}
			}
		}

		return ret;
	}
	

}