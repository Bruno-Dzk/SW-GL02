#include "VideoColorMonitor.hpp"
#include <string>
#include <iostream>
#include <cstdio>


int VideoColorMonitor::getColor()
{
	// For better performance check every N'th pixels row
	int N = 10;

	// Imagemagick is required for screenshot
	system("import -window root screenshot.bmp");
	
	FILE* image = fopen("screenshot.bmp", "rb");

	if(image == NULL)
		throw "Image loading exception";

	unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, image); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int row_padded = (width*3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];
    unsigned char tmp;

    unsigned int r = 0;
    unsigned int g = 0;
    unsigned int b = 0;
    unsigned int counter = 0;

    int loop_limit = height/N;

    if(N > height)
    	throw "N must be lower than image height";

   	int step = row_padded*N;
	for(int i = 0; i < loop_limit; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, image);
        for(int j = 0; j < width*3; j += 3)
        {
            // Colors are stored in (B,G,R) format
            b += (unsigned int)data[j];
            g += (unsigned int)data[j+1];
            r += (unsigned int)data[j+2];
            counter++;
        }
        fseek(image, step, 1);
    }

    b /= counter;
    g /= counter;
    r /= counter;

    unsigned int average_color = ((r)<<16)|((g)<<8)|(b);
    // std::cout << "R: "<< r << " G: " << g << " B: " << b << std::endl;

    // Color decoding:
    // int red   = (average_color>>16) & 0xFF;
	// int green = (average_color>>8) & 0xFF;
	// int blue  = (average_color) & 0xFF;

    free(data);
    fclose(image);
    system("rm screenshot.bmp");

	return average_color;
}
