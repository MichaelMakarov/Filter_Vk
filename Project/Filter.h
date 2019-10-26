#pragma once

#include <vector>
#include <fstream>

#define N_MAX 100 // a maximum number of the pixels in certain direction

struct BLF
{
	float_t X;								// parameter of the averaging by horizontal
	float_t Y;								// parameter of the averaging by vertical
	uint32_t NX;							// a number of the filtered pixels in horizontal direction
	uint32_t NY;							// a number of the filtered pixels in vertical direction						

	// public constructor for creting an instance
	BLF(float_t tx = 0.5f, float_t ty = 0.5f,
		uint32_t nx = 2, uint32_t ny = 2);
	// treating a data of the pixels
	std::vector<uint8_t> Treat(
		std::vector<uint8_t> d,				// a data of the pixels
		uint32_t aw, uint32_t ah,				// width and height of the area will be filtered
		uint32_t ch,						// a number of the channels in the pixels
		uint32_t imw, uint32_t imh,
		uint32_t x0 = 0, uint32_t y0 = 0);	// initial positions of the filtered area

private:
};