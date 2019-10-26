#include <iostream>
#include <omp.h>
#include <chrono>
#include "main.h"


int main()
{
	std::cout << "Project" << std::endl;
	std::cout << "Reading" << std::endl;
	BMP bmp_proc("E:/Users/Desktop/graphics/vulkan/pictures/bmp files/Bathroom_LDR_0009.bmp");
	BMP bmp_proc2 = bmp_proc;
	BMP bmp_proc3 = bmp_proc2;
	Timer timer;
	BLF bilin;

	std::cout << "Changing first" << std::endl;
	timer.Start();
	bmp_proc.data = bilin.Treat(bmp_proc.data, bmp_proc.info_header.width, bmp_proc.info_header.height, 
		bmp_proc.info_header.bit_count, bmp_proc.info_header.width, bmp_proc.info_header.height);
	timer.Finish();
	std::cout << "Saving first" << std::endl;
	bmp_proc.Write("E:/Users/Desktop/graphics/vulkan/pictures/Bathroom_LDR_00091.bmp");

	std::cout << "Changing second" << std::endl;
	omp_set_dynamic(0);
	omp_set_num_threads(4);
	timer.Start();
#pragma omp parallel for
	bmp_proc2.data = bilin.Treat(bmp_proc2.data, bmp_proc2.info_header.width, bmp_proc2.info_header.height,
		bmp_proc2.info_header.bit_count, bmp_proc2.info_header.width, bmp_proc2.info_header.height);
	timer.Finish();

	std::cout << "Saving second" << std::endl;
	bmp_proc2.Write("E:/Users/Desktop/graphics/vulkan/pictures/Bathroom_LDR_00092.bmp");
	std::cout << "End of the project" << std::endl;
	getchar();
	return 0;
}
