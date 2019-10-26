#pragma once

#include <vector>
#include <fstream>

#pragma pack(push, 1)
struct BMPFileHeader
{
	unsigned __int16 file_type{ 0x4D42 };				// file type always BM which is 0x4D42
	unsigned __int32 file_size{ 0 };					// size of the file in bytes
	unsigned __int16 reserved1{ 0 };					// always 0
	unsigned __int16 reserved2{ 0 };					// always 0
	unsigned __int32 offset_data{ 0 };					// start position of pixel data (bytes from the begining of the data)
};
struct BMPInfoHeader
{
	unsigned __int32 size{ 0 };							// header size in bytes
	__int32 width{ 0 };									// width  of bitmap in pixels
	__int32 height{ 0 };								// height of bitmap in pixels
	unsigned __int16 planes{ 1 };						// always 1
	unsigned __int16 bit_count{ 0 };					// bits per pixel
	unsigned __int32 compression{ 0 };					// 0 or 3 - uncompressed
	unsigned __int32 size_image{ 0 };					// 0 - for uncompressed images
	__int32 x_pixel_density{ 0 };						// 
	__int32 y_pixel_density{ 0 };						// 
	unsigned __int32 colors_used{ 0 };					// color indexes of the color table 
	unsigned __int32 colors_important{ 0 };				// number of colors used for displaying an image
};
struct BMPColorHeader
{
	unsigned __int32 red_mask{ 0x00ff0000 };			// bit mask for the red channel
	unsigned __int32 green_mask{ 0x0000ff00 };			// bit mask for the green channel
	unsigned __int32 blue_mask{ 0x000000ff };			// bit mask for the blue channel
	unsigned __int32 alpha_mask{ 0xff000000 };			// bit mask for the alpha channel
	unsigned __int32 color_space_type{ 0x73524742 };	// default "sRGB"
	unsigned __int32 unused[16]{ 0 };					// unused data for the sRGB color space
};
#pragma pack(pop)

struct BMP
{
	BMPFileHeader file_header;
	BMPInfoHeader info_header;
	BMPColorHeader color_header;
	std::vector<unsigned __int8> data;
	
	// creating the BMP by reading a picture from the file
	BMP(const char* filename) { Read(filename); }
	// read an image from the file
	void Read(const char* filename);
	// creating the BMP by creating with special parameters
	BMP(__int32 width, __int32 height, bool has_alpha = true);
	// write an image to the disk
	void Write(const char* filename);
	// changes RGBA values of the chosen part of the image
	void Change(
		uint32_t x0, uint32_t y0,						// coordinates of the region left corner
		uint32_t w, uint32_t h,							// length and width of the region
		uint8_t R, uint8_t G, uint8_t B, uint8_t A);	// RGBA values for theese pixels
	BMP& operator = (BMP another);

private:
	// Check if the pixel data is stored as BGRA and if the color space type is sRGB
	void Check_Color(BMPColorHeader& header);
	uint32_t row_stride{ 0 };
	// Writes only headers of the bitmap into the file
	void Write_Headers(std::ofstream& out);
	// Writes headers and data of the bitmap into the file
	void Write_Headers_And_Data(std::ofstream& out);
	// returns a value that is divided by 4
	uint32_t Make_Stride_Alligned(uint32_t allign);
};