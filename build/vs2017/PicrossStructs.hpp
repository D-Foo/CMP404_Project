#pragma once

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
	};
}