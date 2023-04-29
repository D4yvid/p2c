#include "image.h"
#include <stdio.h>

int main()
{
	for (int y = 0; y < image_data.height; y++)
	{
		for (int x = 0; x < image_data.width; x++)
		{
			unsigned pixel = image_data.pixels[x][y];
			unsigned char r, g, b, a;

			r = (pixel >> 24) & 0xFF;
			g = (pixel >> 16) & 0xFF;
			b = (pixel >>  8) & 0xFF;
			a = (pixel & 0xFF);

			printf("\033[48;2;%d;%d;%dm  \033[0m", r, g, b);
		}

		printf("\n");
	}
}
