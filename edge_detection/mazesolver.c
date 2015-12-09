#include "mazesolver.h"

static volatile unsigned int *LED_PIO       = (unsigned int*) 0x20C0;
static volatile unsigned int *KEY_1_PIO     = (unsigned int*) 0x20A0;
static volatile unsigned int *STEP_SIG_PIO  = (unsigned int*) 0x2090;

static volatile unsigned int *SRAM_CE_N     = (unsigned int*) 0x2080;
static volatile unsigned int *SRAM_OE_N     = (unsigned int*) 0x2070;
static volatile unsigned int *SRAM_WE_N     = (unsigned int*) 0x2060;
static volatile unsigned int *SRAM_UB_N     = (unsigned int*) 0x2050;
static volatile unsigned int *SRAM_LB_N     = (unsigned int*) 0x2040;
static volatile unsigned int *SRAM_ADDR     = (unsigned int*) 0x2030;
static volatile unsigned int *SRAM_DQ       = (unsigned int*) 0x2020;

int main()
{
	*STEP_SIG_PIO = 0;
	init_char_lcd();

	printf("Waiting for webcam image...\n");
	printf("Press KEY_1 when ready.\n\n");
	while(*KEY_1_PIO == 1);

	printf("Reading image now...\n");
	mImage * inimg;
	inimg = (mImage*) malloc(sizeof(mImage));
	read_image_hostfs(inimg, "comp_maze.png");

	/* image loaded, use here */
	printf("Image read complete!\n");
	printf("Width: %d, Height: %d\n\n", inimg->width, inimg->height);

	printf("Copying image to SRAM...\n");
//	copy_img_to_sram(inimg, VGA_BUFFER);
	copy_img_to_sram(inimg, 0x0);

//	write_sram(0x0, 0x31B3);
//	write_sram(0x1, 0x710A);
//	write_sram(0x2, 0x51F6);
//
//	printf("0x0 = %x\n", read_sram(0x0));
//	printf("0x1 = %x\n", read_sram(0x1));
//	printf("0x2 = %x\n", read_sram(0x2));
//	printf("0x0 = %x\n", read_sram(0x0));

	printf("Copying complete!\n\n");

//	printf("(%d,%d) = 0x%x\n", 1, 3, get_pixel(0x0, 1, 3, 640, 480));//t, (unsigned int) VGA_BUFFER[t]);
//	printf("(%d,%d) = 0x%x\n", 5, 11, get_pixel(0x0, 5, 11, 640, 480));
//	printf("(%d,%d) = 0x%x\n", 58, 39, get_pixel(0x0, 58, 39, 640, 480));
//	printf("(%d,%d) = 0x%x\n", 559, 432, get_pixel(0x0, 559, 432, 640, 480));

	printf("Showing image on VGA screen...\n");
	*STEP_SIG_PIO = 1;

	printf("Press KEY_1 to solve.\n\n");
	while(*KEY_1_PIO == 1);
	*STEP_SIG_PIO = 2;

	int solver;
	unsigned char path[144];
	int *size=(int *) malloc(sizeof(int));
	*size = 0;

	int right_coord;
	int block_size =blocksize(inimg->width, inimg->height, inimg->image, 428, 329, 12, &right_coord);
	//solver = dfs(inimg->width, inimg->height, inimg->image, 334, 404, 334, 404, block_size, path, size);
	//solver = dfs(inimg->width, inimg->height, inimg->image, 448, 425, 448, 425, block_size, path, size);
	solver = dfs(inimg->width, inimg->height, inimg->image, 482, 329, 482, 329, block_size, path, size);

	int i1, i2;
	int sx = 482;
	int sy = 329;
	for(i1 = *size; i1 >= 0; i1--)
	{
		if(sx > right_coord)
			break;
		//printf("%c\n", path[i1]);
		int drawx, drawy;
		for(drawx = sx - 4; drawx <= sx + 4; drawx++)
		{
			for(drawy = sy - 4; drawy <= sy + 4; drawy++)
			{
				set_pixel(0x0, drawx, drawy, inimg->width, inimg->height, 0x40);
			}
		}

		if(path[i1] == 'R')
			sx += block_size;
		if(path[i1] == 'D')
			sy += block_size;
		if(path[i1] == 'L')
			sx -= block_size;
		if(path[i1] == 'U')
			sy -= block_size;

		*STEP_SIG_PIO = 3;
		for(i2 = 0; i2 < 100000; i2++);
		*STEP_SIG_PIO = 2;
	}

	printf("Showing solution...\n");
	*STEP_SIG_PIO = 3;


	// *** AFTER PROCESSING *** //
	free(inimg->image);
	lodepng_state_cleanup(&(inimg->state));


	// *** EXTRA CODE TO BLINK LEDs! *** //
	blink_leds(LED_PIO);
	return 1; //never gets here
}

size_t coords(int x, int y, int width, int height)
{
	//return x*height + y;
    return y*width + x;
}

int blocksize(int width, int height, unsigned char *inimg, int start_x, int start_y, int maze_size, int * right_cd)
{
    int i, j;
    int counter_w, counter_tot;
    int left_coord, right_coord;
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

    *right_cd = right_coord;
    block_size = (right_coord-left_coord) / maze_size;

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

unsigned char get_pixel(volatile unsigned int base_addr, int x, int y, int width, int height)
{
	int pix_offset = y*width + x;
	return (unsigned char) read_sram(base_addr + pix_offset);
	//int addr_offset = pix_offset / 2;
	//unsigned int sram_val = base_addr[addr_offset];
	//printf("0x%x   ", (unsigned int) sram_val);

	//if(pix_offset & 0x01) // pix_offset is odd
	//	return sram_val >> 16;// & 0xFF0000;
	//else
	//	return sram_val & 0x0000FF;
}

void set_pixel(volatile unsigned int base_addr, int x, int y, int width, int height, unsigned char pixel)
{
	int pix_offset = y*width + x;
	write_sram(pix_offset, pixel);
	//(unsigned char) read_sram(base_addr + pix_offset);
}

void read_image_hostfs(mImage * img_struct, char * filename)
{
	char* file = "/mnt/host/";
	strcat(file, filename); //cvPNG.png
	unsigned error;
	//unsigned char* image;
	//unsigned width, height;
	unsigned char* png;
	size_t pngsize;

	lodepng_state_init(&(img_struct->state));
	/* customize the state, make image type greyscale */
	img_struct->state.info_raw.colortype = LCT_GREY;

	lodepng_load_file(&png, &pngsize, file);
	//error = lodepng_decode(&image, &width, &height, &state, png, pngsize);
	error = lodepng_decode(&(img_struct->image), &(img_struct->width), &(img_struct->height),
			&(img_struct->state), png, pngsize);
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
	free(png);
}

void copy_img_to_sram(mImage * img, volatile unsigned int base_addr)
{
//	int i;
//	for(i = 0; i < img->width*img->height; i = i+2)
//	{
//		unsigned int addr_val = img->image[i+1];
//		addr_val = (addr_val << 16) + img->image[i];
//
//		base_addr[i/2] = addr_val;
//	}
	alt_u16 data;
	int i;
	for(i = 0; i < img->width*img->height; i++)
	{
		data = img->image[i];
		write_sram(base_addr + i, data);
	}
}

alt_u16 read_sram(volatile unsigned int addr)
{
	*SRAM_ADDR = addr;
	*SRAM_CE_N = 0;
	*SRAM_LB_N = 0;
	*SRAM_UB_N = 0;
	*SRAM_WE_N = 1;
	*SRAM_OE_N = 0;
	alt_u16 data = *SRAM_DQ;
	*SRAM_CE_N = 1;
	*SRAM_LB_N = 1;
	*SRAM_UB_N = 1;
	*SRAM_WE_N = 1;
	*SRAM_OE_N = 1;
	return data;
}

void write_sram(volatile unsigned int addr, alt_u16 data)
{
	*SRAM_ADDR = addr;
	*SRAM_CE_N = 0;
	*SRAM_LB_N = 0;
	*SRAM_UB_N = 0;
	*SRAM_WE_N = 0;
	*SRAM_OE_N = 1;
	*SRAM_DQ   = data;
	*SRAM_CE_N = 1;
	*SRAM_LB_N = 1;
	*SRAM_UB_N = 1;
	*SRAM_WE_N = 1;
	*SRAM_OE_N = 1;
}

void read_test_file(char * filename)
{
	FILE * fp;
	char file[] = "/mnt/host/";
	strcat(file, filename); //test.txt
	fp = fopen(file, "r");
	char buffer[100];
	fgets(buffer, 100, fp);
	printf("File read done!\n");
	printf(buffer);
	fflush(stdout);
	fclose(fp);
}

void init_char_lcd()
{
	alt_up_character_lcd_dev * char_lcd_dev;

	// open the Character LCD port
	char_lcd_dev = alt_up_character_lcd_open_dev ("/dev/character_lcd");
	if(char_lcd_dev == NULL)
		alt_printf ("Error: could not open character LCD device\n");
	else
		alt_printf ("Opened character LCD device\n");

	/* Initialize the character display */
	alt_up_character_lcd_init (char_lcd_dev);
	/* Write "Welcome to" in the first row */
	alt_up_character_lcd_string(char_lcd_dev, "Hi Shivam");
	/* Write "the DE2 board" in the second row */
	char second_row[] = "I'm DE2 board\0";
	alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
	alt_up_character_lcd_string(char_lcd_dev, second_row);
}

void blink_leds(volatile unsigned int * led_pio)
{
	*led_pio = 0; //clear all LEDs
	int i = 0;
	while ( (1+1) != 3) //infinite loop
	{
		for (i = 0; i < 100000; i++); //software delay
		*led_pio |= 0x1; //set LSB
		for (i = 0; i < 100000; i++); //software delay
		*led_pio &= ~0x1; //clear LSB
	}
}
