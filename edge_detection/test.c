#include "lodepng.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#define MAX_BRIGHTNESS 255

size_t coords(int x, int y, int width, int height)
{
	//return x*height + y;
    return y*width + x;
}

void gaussian_blur(int width, int height, unsigned char * inimg, unsigned char * outimg)
{
    int kernel[3][3] = {{  1,  2,  1 },
                        {  2,  4,  2 },
                        {  1,  2,  1 }};
    //double kernel[3][3] = {{ 0.4, 0.4, 0.4 },
    //                       { 0.4, 0.4, 0.4 },
    //                       { 0.4, 0.4, 0.4 }};
    int kernel_size = 16;
    double pixval;
    int x, y, i, j;
    printf("Applying Gaussian Blur\n\n");
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if(y == 0 || x == 0 || y == height-1 || x == width-1)
            {
                outimg[coords(x,y,width,height)] = inimg[coords(x,y,width,height)];
                continue;
            }
          
            pixval = 0.0;
            for (j = -1; j <= 1; j++) {
                for (i = -1; i <= 1; i++) {
                    pixval += kernel[i+1][j+1] * inimg[coords(x+i,y+j,width,height)];
                }
            }
            
            pixval = pixval / kernel_size;

            outimg[coords(x,y,width,height)] = (unsigned char) pixval;
        }
    }
}

void sobel_filtering(int width, int height, unsigned char * inimg, unsigned char * outimg)
{
    /* Definition of Sobel filter in horizontal direction */
    int weight_x[3][3] = {{ -1,  0,  1 },
                          { -2,  0,  2 },
                          { -1,  0,  1 }};
    int weight_y[3][3] = {{ -1, -2, -1 },
                          {  0,  0,  0 },
                          {  1,  2,  1 }};
    double G_x, G_y, G;
    int x, y, i, j;  /* Loop variable */

    /* Maximum values calculation after filtering*/
    printf("Applying Sobel Filter\n\n");
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if(y == 0 || x == 0 || y == height-1 || x == width-1)
            {
                outimg[coords(x,y,width,height)] = inimg[coords(x,y,width,height)];
                continue;
            }
          
            G_x = 0.0;
            G_y = 0.0;
            G = 0.0;
            for (j = -1; j <= 1; j++) {
                for (i = -1; i <= 1; i++) {
                    G_x += weight_x[i+1][j+1] * inimg[coords(x+i,y+j,width,height)];
                    G_y += weight_y[i+1][j+1] * inimg[coords(x+i,y+j,width,height)];
                }
            }
            
            G = sqrt(G_x*G_x + G_y*G_y);

            outimg[coords(x,y,width,height)] = (unsigned char) G;
        }
    }
}

int blocksize(int width, int height, unsigned char *inimg, int * start_x, int * start_y, int maze_size, int * right_cd, int index1, int index2)
{
    int i, j;
    int counter_w, counter_tot;
    int left_coord, right_coord, top_coord;
    int block_size;

    //check left boundary
    counter_w = 0;
    counter_tot = 10;
    for(i = 0; i < width; i++)
    {
        if(inimg[coords(i,height/2,width,height)] > (unsigned char)150 && counter_tot > 0)
        {
            if(counter_w==0){
                    counter_tot=10;
                }
            counter_w++;
            if(counter_w==3)
                break;
        }
        else if(inimg[coords(i,height/2,width,height)] > (unsigned char)150 && counter_tot <= 0)
        {
            counter_tot = 10;
            counter_w = 0;
            counter_w++;
        }
        counter_tot--;
    }   
    left_coord = i-counter_tot;

    //check right boundary
    counter_w = 0;
    counter_tot = 10;
    for(i = width-1; i >= 0; i--)
    {
        if(inimg[coords(i,height/2,width,height)] > (unsigned char)150 && counter_tot > 0)
        {
            if(counter_w==0){
                    counter_tot=10;
                }
            counter_w++;
            if(counter_w==3)
                break;
        }
        else if(inimg[coords(i,height/2,width,height)] > (unsigned char)150 && counter_tot <= 0)
        {
            counter_tot = 10;
            counter_w = 0;
            counter_w++;
        }
        counter_tot--;
    }   
    right_coord = i + counter_tot;

    //check top boundary
    counter_w = 0;
    counter_tot = 10;
    for(i = 0; i <height; i++)
    {
        if(inimg[coords(width/2,i,width,height)] > (unsigned char)150 && counter_tot > 0)
        {
            if(counter_w==0){
                    counter_tot=10;
                }
            counter_w++;
            if(counter_w==3)
                break;
        }
        else if(inimg[coords(width/2,i,width,height)] > (unsigned char)150 && counter_tot <= 0)
        {
            counter_tot = 10;
            counter_w = 0;
            counter_w++;
        }
        counter_tot--;
    }   
    top_coord = i - counter_tot;
    printf("%d\n", top_coord);
    *right_cd = right_coord;
    block_size = (right_coord-left_coord) / maze_size;
    printf("%d\n", block_size);
    
    *start_y = ((block_size*index1)+top_coord)-(block_size/2);
    *start_x = ((block_size*index2)+left_coord)-(block_size/2);
    printf("%d %d\n", *start_x, *start_y);
    return block_size;
}

int dfs(int width, int height, unsigned char * inimg, int x_new, int y_new,
        int x_prev, int y_prev, int block_size, unsigned char * output, int * size)
{

    //check if it reaches x boundary on the right
    if(x_new>=width-1){
        int i;
        int counter_w, counter_tot;

        counter_w = 0;
        counter_tot = 15;
        for(i=x_prev;i<width;i++){
            if(inimg[coords(i,y_new,width,height)]>(unsigned char)150 && counter_tot>0){
                if(counter_w==0){
                    counter_tot=15;
                }
                counter_w++;
            }
            else if(inimg[coords(i,y_new,width,height)]>(unsigned char)150 && counter_tot<=0){
                counter_tot = 15;
                counter_w = 0;
                counter_w++;
            }
            counter_tot--;
        }
        if(counter_w<3){
            return 1;
        }
        else
            return 0;
    }

    //check if it reaches x boundary on the left
    if(x_new<=0){
        int i;
        int counter_w, counter_tot;

        counter_w = 0;
        counter_tot = 15;
        for(i=x_prev;i>=0;i--){
            if(inimg[coords(i,y_new,width,height)]>(unsigned char)150 && counter_tot>0){
                if(counter_w==0){
                    counter_tot=15;
                }
                counter_w++;
            }
            else if(inimg[coords(i,y_new,width,height)]>(unsigned char)150 && counter_tot<=0){
                counter_tot = 15;
                counter_w = 0;
                counter_w++;
            }
            counter_tot--;
        }
        if(counter_w<3){
            return 1;
        }
        else
            return 0;
    }

    //check if it reaches y boundary above
    if(y_new<=0){
        int i;
        int counter_w, counter_tot;

        counter_w = 0;
        counter_tot = 15;
        for(i=y_prev;i>=0;i--){
            if(inimg[coords(x_new,i,width,height)]>(unsigned char)150 && counter_tot>0){
                if(counter_w==0){
                    counter_tot=15;
                }
                counter_w++;
            }
            else if(inimg[coords(x_new,i,width,height)]>(unsigned char)150 && counter_tot<=0){
                counter_tot = 15;
                counter_w = 0;
                counter_w++;
            }
            counter_tot--;
        }
        if(counter_w<3){
            return 1;
        }
        else
            return 0;
    }

    //check if it reaches y boundary down
    if(y_new>=height-1){
        int i;
        int counter_w, counter_tot;

        counter_w = 0;
        counter_tot = 15;
        for(i=y_prev;i<height;i++){
            if(inimg[coords(x_new,i,width,height)]>(unsigned char)150 && counter_tot>0){
                if(counter_w==0){
                    counter_tot=15;
                }
                counter_w++;
            }
            else if(inimg[coords(x_new,i,width,height)]>(unsigned char)150 && counter_tot<=0){
                counter_tot = 15;
                counter_w = 0;
                counter_w++;
            }
            counter_tot--;
        }
        if(counter_w<3){
            return 1;
        }
        else
            return 0;
    }

    //check if it can go left
    if(x_new<x_prev){
        int i;
        int counter_w, counter_tot, counter_visited;
        //printf("\n\n%d, %d, %d, %d", x_new, x_prev, y_new, y_prev);
        counter_w = 0;
        counter_tot = 15;
        counter_visited = 0;
        for(i=x_prev;i>=x_new;i--){
            if(inimg[coords(i,y_new,width,height)]>(unsigned char)150 && counter_tot>0){
                if(counter_w==0){
                    counter_tot=15;
                }
                counter_w++;
            }
            else if(inimg[coords(i,y_new,width,height)]>(unsigned char)150 && counter_tot<=0){
                counter_tot = 15;
                counter_w = 0;
                counter_w++;
            }
            counter_tot--;
            if(inimg[coords(i,y_new,width,height)]==100){
                counter_visited++;
                if(counter_visited>5){
                    return 0;
                }
            }
            //printf(", %d", inimg[coords(i,y_new,width,height)]); 
        }
        if(counter_w<3){
            //printf("Left");
            for(i=x_prev;i>=x_new;i--){
                if(inimg[coords(i,y_new,width,height)]<(unsigned char)150)
                    inimg[coords(i,y_new,width,height)]=100;  //mark as visited
            }
        }
        else
            return 0;
    }

    //check if it can go right
    if(x_new>x_prev){
        int i;
        int counter_w, counter_tot, counter_visited;
        //printf("\n\n%d, %d, %d, %d", x_new, x_prev, y_new, y_prev);
        counter_w = 0;
        counter_tot = 15;
        counter_visited = 0;
        for(i=x_prev;i<=x_new;i++){
            if(inimg[coords(i,y_new,width,height)]>(unsigned char)150 && counter_tot>0){
                if(counter_w==0){
                    counter_tot=15;
                }
                counter_w++;
            }
            else if(inimg[coords(i,y_new,width,height)]>(unsigned char)150 && counter_tot<=0){
                counter_tot = 15;
                counter_w = 0;
                counter_w++;
            }
            counter_tot--;
            if(inimg[coords(i,y_new,width,height)]==100){
                counter_visited++;
                if(counter_visited>5){   
                    return 0;
                }
            }
            //printf(", %d", inimg[coords(i,y_new,width,height)]); 
        }
        if(counter_w<3){
            //printf("Right");
            for(i=x_prev;i<=x_new;i++){
                if(inimg[coords(i,y_new,width,height)]<(unsigned char)150)
                    inimg[coords(i,y_new,width,height)]=100;    //mark as visited
            }
        }
        else
            return 0;
    }

    //check if it can go up
    if(y_new<y_prev){
        int i;
        int counter_w, counter_tot,counter_visited;
        //printf("\n\n%d, %d, %d, %d", x_new, x_prev, y_new, y_prev);
        counter_w = 0;
        counter_tot = 15;
        counter_visited = 0;
        for(i=y_prev;i>=y_new;i--){
            if(inimg[coords(x_new,i,width,height)]>(unsigned char)150 && counter_tot>0){
                if(counter_w==0){
                    counter_tot=15;
                }
                counter_w++;
            }
            else if(inimg[coords(x_new,i,width,height)]>(unsigned char)150 && counter_tot<=0){
                counter_tot = 15;
                counter_w = 0;
                counter_w++;
                //printf("%d\n", counter_w);
            }
            counter_tot--;
            if(inimg[coords(x_new,i,width,height)]==100){
                counter_visited++;
                if(counter_visited>5){
                    return 0;
                }
            }
            //printf(", %d", inimg[coords(x_new,i,width,height)]); 
        }
        if(counter_w<3){
            //printf("Up");
            for(i=y_prev;i>=y_new;i--){
                if(inimg[coords(x_new,i,width,height)]<(unsigned char)150)
                    inimg[coords(x_new,i,width,height)]=100;        //mark as visited
            }
        }
        else
            return 0;
    }

    //check if it can go down
    if(y_new>y_prev){
        int i;
        int counter_w, counter_tot, counter_visited;
        //printf("\n\n%d, %d, %d, %d", x_new, x_prev, y_new, y_prev);
        counter_w = 0;
        counter_tot = 15;
        counter_visited = 0;
        for(i=y_prev;i<=y_new;i++){
            if(inimg[coords(x_new,i,width,height)]>(unsigned char)150 && counter_tot>0){
                if(counter_w==0){
                    counter_tot=15;
                }
                counter_w++;
            }
            else if(inimg[coords(x_new,i,width,height)]>(unsigned char)150 && counter_tot<=0){
                counter_tot = 15;
                counter_w = 0;
                counter_w++;
            }
            counter_tot--;
            if(inimg[coords(x_new,i,width,height)]==100){
                counter_visited++;
                if(counter_visited>5){
                    return 0;
                }
            }
            //printf(", %d", inimg[coords(x_new,i,width,height)]); 
        }
        if(counter_w<3){
            //printf("Down");
            for(i=y_prev;i<=y_new;i++){
                if(inimg[coords(x_new,i,width,height)]<(unsigned char)150)
                    inimg[coords(x_new,i,width,height)]=100;        //mark as visited
            }
        }
        else
            return 0;
    }

    //go left
    if(dfs(width,height,inimg, x_new-block_size, y_new, x_new, y_new, block_size, output, size)==1){
        *size = *size + 1;
        //printf("Left\n");
        //printf("(%d,%d)\n", x_new, y_new);
        output[*size]='L';
        return 1;
    }
        
    //go right
    if(dfs(width,height,inimg, x_new+block_size, y_new, x_new, y_new, block_size, output, size)==1){
        *size = *size + 1;
        //printf("Right\n");
        //printf("(%d,%d)\n", x_new, y_new);
        output[*size]='R';
        return 1;
    }

    //go up
    if(dfs(width,height,inimg, x_new, y_new-block_size, x_new, y_new, block_size, output, size)==1){
        *size = *size + 1;
        //printf("Up\n");
        //printf("(%d,%d)\n", x_new, y_new);
        output[*size]='U';
        return 1;
    }

    //go down
    if(dfs(width,height,inimg, x_new, y_new+block_size, x_new, y_new, block_size, output, size)==1){
        *size = *size + 1;
        //printf("Down\n");
        //printf("(%d,%d)\n", x_new, y_new);
        output[*size]='D';
        return 1;
    }

    //printf("couldnt solve...");
    return 0;

}

int main(void)
{
	unsigned error;
	unsigned char* image;
	unsigned width, height;
	unsigned char* png;
	size_t pngsize;
	LodePNGState state;
	char filename[] = "comp_maze.png";  /* NAME OF INPUT IMAGE */

	lodepng_state_init(&state);
	/*optionally customize the state*/
	state.info_raw.colortype = LCT_GREY;

	lodepng_load_file(&png, &pngsize, filename);
	error = lodepng_decode(&image, &width, &height, &state, png, pngsize);
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

	free(png);

	/*use image here*/
	//printf("Width=%d Height=%d\n", width, height);
    
    //FILE * fp;
    //fp = fopen("imgout.txt", "w");
    //fp = fopen("imgout.ram", "wb");
    //int i,x,y;
    //char * ramimg = (char*) calloc(2*width*height, sizeof(char));
    //for(i = 0; i < 2*width*height; i++){
        //fprintf(fp, "%d (%d,%d)=%d\n", i, i%width, i/width, image[i]);
    //    if(i%2 == 0)
    //        ramimg[i] = image[i/2];
    //    else
    //        ramimg[i] = 0;
    //}
    //fwrite(ramimg, sizeof(unsigned char), 2*width*height, fp);
    //free(ramimg);
    //fclose(fp);
    //for(y = 0; y < height; y++) {
    //    for(x = 0; x < width; x++) {
    //        fprintf(fp, "(%d,%d)=%d\n", x, y, image[coords(x,y,width,height)]);
    //    }
    //}
	
    unsigned char * gaussimg = (unsigned char*) calloc(width*height, sizeof(char));
	unsigned char * outimg = (unsigned char*) calloc(width*height, sizeof(char));
    gaussian_blur(width, height, image, gaussimg);
	sobel_filtering(width, height, gaussimg, outimg);
    
    int right_coord, startx, starty, index1=11, index2=7;
    int block_size =blocksize(width, height, image, &startx, &starty, 12, &right_coord, index1, index2);
    //printf("blocksize = %d\n", block_size);
    
    int solver;
    unsigned char path[144];
    int *size=(int *) malloc(sizeof(int));
    *size = 0;

    solver = dfs(width,height,image, startx, starty, startx, starty, block_size, path, size);
    
    //printf("size = %d\n", *size);
    int i1;
    for(i1 = *size; i1 >= 0; i1--)
    {
        printf("%c\n", path[i1]);
    }
    
    //int x, y;
    //for(x = 0; x < width; x++)
    //{
    //    for(y = 0; y < height; y++)
    //    {
    //        outimg[coords(x,y,width,height)] = image[coords(x,y,width,height)];
    //    }
    //}
	
	unsigned char* outpng;
	error = lodepng_encode(&outpng, &pngsize, outimg, width, height, &state);
	if(!error) lodepng_save_file(outpng, pngsize, "sobel.png");  /* NAME OF OUTPUT IMAGE */
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
	
    
    
	/* CLEANUP */
	lodepng_state_cleanup(&state);
	free(image);
	free(outimg);
	
	return 0;
}
