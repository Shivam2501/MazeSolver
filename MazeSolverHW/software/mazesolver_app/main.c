/*
 * led_test.c
 *
 *  Created on: Nov 24, 2015
 *      Author: Srujun
 */

#include "stdlib.h"
#include "system.h"
#include "stdio.h"

#include "lodepng.h"

void displayAsciiArt(const unsigned char * image, unsigned w, unsigned h)
{
  if(w > 0 && h > 0)
  {
    printf("\nASCII Art Preview: \n");
    unsigned w2 = 48;
    if(w < w2) w2 = w;
    unsigned h2 = h * w2 / w;
    h2 = (h2 * 2) / 3; //compensate for non-square characters in terminal
    if(h2 > (w2 * 2)) h2 = w2 * 2; //avoid too large output

    printf("+");
    unsigned x;
    for(x = 0; x < w2; x++) printf("-");
    printf("+\n");
    unsigned y;
    for(y = 0; y < h2; y++)
    {
      printf("|");
      for(x = 0; x < w2; x++)
      {
        unsigned x2 = x * w / w2;
        unsigned y2 = y * h / h2;
        int r = image[y2 * w * 4 + x2 * 4 + 0];
        int g = image[y2 * w * 4 + x2 * 4 + 1];
        int b = image[y2 * w * 4 + x2 * 4 + 2];
        int a = image[y2 * w * 4 + x2 * 4 + 3];
        int lightness = ((r + g + b) / 3) * a / 255;
        int min = (r < g && r < b) ? r : (g < b ? g : b);
        int max = (r > g && r > b) ? r : (g > b ? g : b);
        int saturation = max - min;
        int letter = 'i'; //i for grey, or r,y,g,c,b,m for colors
        if(saturation > 32)
        {
          int h = lightness >= (min + max) / 2;
          if(h) letter = (min == r ? 'c' : (min == g ? 'm' : 'y'));
          else letter = (max == r ? 'r' : (max == g ? 'g' : 'b'));
        }
        int symbol = ' ';
        if(lightness > 224) symbol = '@';
        else if(lightness > 128) symbol = letter - 32;
        else if(lightness > 32) symbol = letter;
        else if(lightness > 16) symbol = '.';
        printf((char)symbol);
      }
      printf("|\n");
    }
    printf("+");
    for(x = 0; x < w2; x++) printf("-");
    printf("+\n");
  }
}

int main()
{
	int i = 0;
	volatile unsigned int *LED_PIO   = (unsigned int*)0x2030;
	volatile unsigned int *KEY_1_PIO = (unsigned int*)0x2010;

	printf("Waiting for user to press KEY_1\n");
	while(*KEY_1_PIO == 1);

	FILE * fp;
	fp = fopen("/mnt/host/test.txt", "r");
	char buffer[100];
	fgets(buffer, 100, fp);
	printf("File read done!\n");
	printf(buffer);
	fflush(stdout);
	fclose(fp);

	const char* filename = "/mnt/host/cvPNG.png";

	unsigned error;
	unsigned char* image;
	unsigned width, height;

	error = lodepng_decode32_file(&image, &width, &height, filename);
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

	printf("Width: %d, Height: %d", width, height);
	//displayAsciiArt(image, width, height);

	free(image);

	*LED_PIO = 0; //clear all LEDs
	while ( (1+1) != 3) //infinite loop
	{
		for (i = 0; i < 100000; i++); //software delay
		*LED_PIO |= 0x1; //set LSB
		for (i = 0; i < 100000; i++); //software delay
		*LED_PIO &= ~0x1; //clear LSB
	}
	return 1; //never gets here
}
