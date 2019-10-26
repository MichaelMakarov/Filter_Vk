#include "Filter.h"
#include <cmath>

BLF::BLF(float_t tx, float_t ty, uint32_t nx, uint32_t ny)
{
	if (tx > 1.0f || tx < 0.0f || ty > 1.0f || ty < 0.0f)
		throw std::runtime_error("Values of the interpolation parameters should be between 0 and 1!");
	if (nx < 2 || ny < 2 || nx > N_MAX || ny > N_MAX)
		throw std::runtime_error("The number of the pixels in horizontal and vertical direction should be between 2 and 100!");
	X = tx;
	Y = ty;
	NX = nx;
	NY = ny;
}

std::vector<uint8_t> BLF::Treat(
	std::vector<uint8_t> d,
	uint32_t aw, uint32_t ah,
	uint32_t ch,
	uint32_t imw, uint32_t imh,
	uint32_t x0, uint32_t y0)
{
	uint32_t c = ch / 8;
	float_t midx = X * NX,
		midy = Y * NY;
	//std::vector<uint8_t> data(d);

	if (imw < 1 || imh < 1 || c < 1 || c > 4 || aw < NX || ah < NY || (aw + x0) > imw || (ah + y0) > imh)
		throw std::runtime_error("Width and height should be more than zero and the number of the channels between 1 and 4!");
	if (imw * imh * c > d.size())
		throw std::runtime_error("Data does not match the size of the filtered area!");

	for (uint32_t y = y0 + NY / 2; y < y0 + ah - NY / 2; ++y)
	{
		for (uint32_t x = x0 + NX / 2; x < x0 + aw - NX / 2; ++x)
		{
			for (uint32_t i = 0; i < c; ++i)
			{
				uint8_t v11 = d[c * ((y - NY / 2) * imw + x - NX / 2) + i],
					v12 = d[c * ((y + NY / 2) * imw + x - NX / 2) + i],
					v21 = d[c * ((y - NY / 2) * imw + x + NX / 2) + i],
					v22 = d[c * ((y + NY / 2) * imw + x + NX / 2) + i];
				d[c * (y * imw + x) + i] = (uint8_t)(X * v11 * Y + (1 - X) * v21 * Y + X * v12 * (1 - Y) + (1 - X) * v22 * (1 - Y));
			}
		}
	}
	return d;
}