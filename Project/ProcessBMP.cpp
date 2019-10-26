#include "ProcessBMP.h"


void BMP::Read(const char* filename)
{
	std::ifstream in{ filename, std::ios_base::binary };
	if (in)
	{
		in.read((char*)& file_header, sizeof(file_header));
		if (file_header.file_type != 0x4D42)
			throw std::runtime_error("Error! Unrecognized file format.");
		in.read((char*)& info_header, sizeof(info_header));
		if (info_header.bit_count == 32)
		{
			if (info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader)))
			{
				in.read((char*)& color_header, sizeof(color_header));
				Check_Color(color_header);
			}
			else throw std::runtime_error("Error! File does not contain a bit mask.");
		}
		// jump to the pixel data location
		in.seekg(file_header.offset_data, in.beg);
		// adjust the header fields for output
		if (info_header.bit_count == 32)
		{
			info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
			file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
		}
		else
		{
			info_header.size = sizeof(BMPInfoHeader);
			file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
		}
		file_header.file_size = file_header.offset_data;
		if (info_header.height < 0)
			throw std::runtime_error("Only bmp files with the origin int he left corner can be treated!");
		data.resize(info_header.width * info_header.height * info_header.bit_count / 8);
		// checking for row padding
		if (info_header.width % 4 == 0)
		{
			in.read((char*)data.data(), data.size());
			file_header.file_size += data.size();
		}
		else
		{
			row_stride = info_header.width * info_header.bit_count / 8;
			unsigned __int32 new_stride = Make_Stride_Alligned(4);
			std::vector<unsigned __int8> padding_row(new_stride - row_stride);
			for (unsigned int y = 0; y < info_header.height; ++y)
			{
				in.read((char*)(data.data() + row_stride * y), row_stride);
				in.read((char*)padding_row.data(), padding_row.size());
			}
			file_header.file_size += data.size() + info_header.height * padding_row.size();
		}
	}
	else throw std::runtime_error("Unable to open the input image file!");
}

void BMP::Check_Color(BMPColorHeader& header)
{
	BMPColorHeader expected;
	if (expected.red_mask != header.red_mask ||
		expected.blue_mask != header.blue_mask ||
		expected.green_mask != header.green_mask ||
		expected.alpha_mask != header.alpha_mask)
	{
		throw std::runtime_error("Unexpected color mask format! Expected BGRA format.");
	}
	if (expected.color_space_type != header.color_space_type)
		throw std::runtime_error("Unexpected color space type! Expected sRGB values.");
}

void BMP::Write(const char* filename)
{
	std::ofstream out(filename, std::ios_base::binary);
	if (out)
	{
		if (info_header.bit_count == 32) Write_Headers_And_Data(out);
		else if (info_header.bit_count == 24)
		{
			if (info_header.width % 4 == 0) Write_Headers_And_Data(out);
			else
			{
				unsigned __int32 new_stride = Make_Stride_Alligned(4);
				std::vector<unsigned __int8> padding_row(new_stride - row_stride);
				Write_Headers(out);
				for (unsigned int y = 0; y < info_header.height; ++y)
				{
					out.write((const char*)(data.data() + row_stride * y), row_stride);
					out.write((const char*)padding_row.data(), padding_row.size());
				}
			}
		}
		else throw std::runtime_error("It can be treated only with 32 or 24 bits per pixel!");
	}
	else throw std::runtime_error("Unable to open the output image file!");
}

void BMP::Write_Headers(std::ofstream& out)
{
	out.write((const char*)& file_header, sizeof(file_header));
	out.write((const char*)& info_header, sizeof(info_header));
	if (info_header.bit_count == 32)
	{
		out.write((const char*)& color_header, sizeof(color_header));
	}
}

void BMP::Write_Headers_And_Data(std::ofstream& out)
{
	Write_Headers(out);
	out.write((const char*)data.data(), data.size());
}

uint32_t BMP::Make_Stride_Alligned(uint32_t allign)
{
	uint32_t new_stride = row_stride;
	while (new_stride % allign != 0) new_stride++;
	return new_stride;
}

BMP::BMP(int32_t width, int32_t height, bool has_alpha)
{
	if (width <= 0 || height <= 0)
		throw std::runtime_error("The image width and height should be positive values!");
	info_header.width = width;
	info_header.height = height;
	if (has_alpha)
	{
		info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
		file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
		info_header.bit_count = 32;
		info_header.compression = 3;
		row_stride = width * 4;
		data.resize(row_stride * height);
		file_header.file_size = file_header.offset_data + data.size();
	}
	else
	{
		info_header.size = sizeof(BMPInfoHeader);
		file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
		info_header.bit_count = 24;
		info_header.compression = 0;
		row_stride = width * 3;
		data.resize(row_stride * height);
		uint32_t new_stride = Make_Stride_Alligned(4);
		file_header.file_size = file_header.offset_data + data.size() + info_header.height * (new_stride - row_stride);
	}
}

void BMP::Change(
	uint32_t x0, uint32_t y0,
	uint32_t w, uint32_t h,
	uint8_t R, uint8_t G, uint8_t B, uint8_t A)
{
	if (x0 + w > (uint32_t)info_header.width || y0 + h > (uint32_t)info_header.height)
		throw std::runtime_error("The region does not fit in the image!");
	uint32_t channels = info_header.bit_count / 8;
	uint8_t values[] = {B, G, R, A};
	for (uint32_t y = y0; y < y0 + h; ++y)
	{
		for (uint32_t x = x0; x < x0 + w; ++x)
		{
			for (uint32_t i = 0; i < channels; ++i)
			{
				data[channels * (y * info_header.width + x) + i] = values[i];
			}
		}
	}
}

BMP& BMP::operator = (BMP that)
{
	this->row_stride = that.row_stride;
	std::copy(that.data.begin(), that.data.end(), this->data.begin());
	this->color_header = that.color_header;
	this->file_header = that.file_header;
	this->info_header = that.info_header;
	return *this;
}

