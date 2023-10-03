#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <linux/fb.h>
#include <asm/termbits.h>
#include <time.h>
#include <sys/time.h>

#include "graphics.h"

#define framebuffer_path "/dev/fb0" //On screen framebuffer path
#define xin 0 //Standard Input File Descriptor
#define xout 1 //Standard Output File Descriptor
#define xerr 2 //Standard Error File Descriptor

int fd; //Contains file descriptor for 'dev/fb0'

void *addr; //On Screen FrameBuffer

size_t totalSize; //Total file size

struct termios ogTerm; //Original Terminal Settings
struct termios term; //Terminal Struct (ICANON AND ECHO)

void init_graphics() {
	struct fb_var_screeninfo vsinfo; //Struct for screen resolution
	struct fb_fix_screeninfo fsinfo; //Struct for bitdepth
	
	//Open framebuffer file using constant with r/w permissions
	fd = open(framebuffer_path, O_RDWR);
	if (fd == -1) {
		//Failed to open file
	}
	
	//Fill vsinfo struct to get Virtual Resolution
	if (ioctl(fd, FBIOGET_VSCREENINFO, &vsinfo) == -1) {
		//Failed to get vscreen info
	}
	
	//Fill fsinfo struct to get bit depth
	if (ioctl(fd, FBIOGET_FSCREENINFO, &fsinfo) == -1) {
		//Failed to get fscreen info
	}
	
	//Multiply into tsize to get file size for mmap
	totalSize = vsinfo.yres_virtual * fsinfo.line_length;
	
	//Run mmap and store starting index of address into char * addr
	addr = mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		//Mapping failed exit program
	}
	
	//Load current terminal settings into term struct
	if (ioctl(xin, TCGETS, &ogTerm) == -1) {
		//Getting current terminal settings failed
	}
	
	term = ogTerm;
	
	term.c_lflag &= ~ICANON; //Disable CANONICAL mode
	term.c_lflag &= ~ECHO; //Disable ECHO 
	
	//Set current terminal setting from term struct
	if (ioctl(xin, TCSETS, &term) == -1) {
		//Setting new terminal settings failed
	}
	
}

void exit_graphics() {
	if (close(fd) == -1) {
	}
	
	if (munmap(addr, totalSize) == -1) {
		//Failed to unallocate (deallocate?)
	}
	
	if (ioctl(xin, TCSETS, &ogTerm) == -1) {
		//Failed to set new terminal settings
	}
}

char getkey() {
	//xin = 0 (Standard Input File Descriptor)
	//xout = 1 (Standard Output File Descriptor)
	//xerr = 2 (Standard Error File Descriptor)
	struct timeval timeout;
	struct timeval *pto;
	
	long timeoutTime = 2;
	pto = &timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = timeoutTime;
	
	int nfds = 1; //Num file descriptors to select
	char buffer[1]; //Size of buffer to return
	fd_set readfds, writefds, errorfds; //File descriptor sets
	
	FD_ZERO(&readfds); //Reset read file descriptor
	//FD_ZERO(&writefds);
	//FD_ZERO(&errorfds);
	
	FD_SET(xin, &readfds); //Set standard input into readfds
	//FD_SET(xout, &writefds);
	//FD_SET(xerr, &errorfds);
	
	//TODO Set Timeout
	
	if (select(1, &readfds, NULL, NULL, pto) == 1) {
		//Input is waiting. Load into buffer
		read(xin, buffer, 1);
		return buffer[0];
	}
	return '0';
}

void sleep_ms(long ms) {
	long nano_secs = ms * 1000000; //milisecs converted to nanosecs
	
	struct timespec request; //Struct for sleeptime
	
	request.tv_sec = 0;
	request.tv_nsec = (long) nano_secs; //Set nanosecs field to nanosecs
	
	if (nanosleep(&request, NULL) == -1) {
		//Time sleep interupt (Shouldn't happen)
	}
}

void *new_offscreen_buffer() {
	//Map off screen file buffer
	short *oaddr;
	
	oaddr = mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	
	if (oaddr == MAP_FAILED) {
		//Mapping failed
	}
	
	return oaddr;
}

void blit(void *src) {
	//SRC and ADDR are short pointers (2 bytes per pixel)
	//Only one for loop is needed (I hope)
	int i;
	for (i = 0; i < totalSize / 2; i++) {
		*((short *) addr + i) = *((short *) src + i);
	}
}

void draw_pixel(void *img, int x, int y, color_t color) {
	int width = 640;
	int height = 480;
	
	if (x > 639 || y > 479) {
		return;
	}
	
	int index = (y * width) + x;
	*((short *) img + index) = color;
}

void draw_line(void *img, int x1, int y1, int x2, int y2, color_t color) 
{
	//Source Bresenhams Line Alorithm UCDavis
	//Site too long to copy url
	int dx, dy, p, incx, incy, x, y;
	
	dx = x2 - x1;
	dy = y2 - y1;
	if (dx < 0) {
		dx = -dx; //Keep DX Positive
	}
	if (dy < 0) {
		dy = -dy; //Keep DX Positive
	}
	
	if (x2 > x1) {
		incx = 1;
	} else {
		incx = -1;
	}
	
	if (y2 > y1) {
		incy = 1;
	} else {
		incy = -1; //Negative Slope
	}
	
	x = x1;
	y = y1;
	
	if (dx > dy) {
		p = 2 * dy - dx;
		while (x != x2) {
			draw_pixel(img, x, y, color);
			x += incx;
			if (p < 0) {
				p += 2 * dy;
			} else {
				p += 2 * (dy - dx);
				y += incy;
			}
		}
	} else {
		p = 2 * dx - dy;
		while (y != y2) {
			draw_pixel(img, x, y, color);
			y += incy;
			if (p < 0) {
				p += 2 * dx;
			} else {
				p += 2 * (dx - dy);
				x += incx;
			}
		}
	}
	draw_pixel(img, x, y, color); //Last pixel to draw
}

void draw_circle(void *img, int x, int y, int r, color_t color) {
	int xCenter = x;
	int yCenter = y;
	
	int currX = 0;
	int currY = r;
	int p = 1 - r;
	
	draw_pixel(img, xCenter + currX, yCenter + currY, color);
	draw_pixel(img, xCenter - currX, yCenter + currY, color);
	draw_pixel(img, xCenter + currX, yCenter - currY, color);
	draw_pixel(img, xCenter - currX, yCenter - currY, color);
	
	while (currX < currY) {
		currX++;
		
		if (p < 0) {
			p += 2 * currX + 1;
		} else {
			currY--;
			p += 2 * (currX - currY) + 1;
		}
		
		draw_pixel(img, xCenter + currX, yCenter + currY, 
		color);
		draw_pixel(img, xCenter - currX, yCenter + currY, 
		color);
		draw_pixel(img, xCenter + currX, yCenter - currY, 
		color);
		draw_pixel(img, xCenter - currX, yCenter - currY, 
		color);
		
		if (currX != currY) {
			draw_pixel(img, xCenter + currY, yCenter + 
			currX, color);
			draw_pixel(img, xCenter - currY, yCenter + 
			currX, color);
			draw_pixel(img, xCenter + currY, yCenter - 
			currX, color);
			draw_pixel(img, xCenter - currY, yCenter - 
			currX, color);
		}
	}
}

void clear_screen(void *img) {
	//Cast IMG to Char used 2 for loops as it seemed easier
	int i;
	for (i = 0; i < totalSize; i++) {
		*((char *) (img + i)) = 0;
	}
	for (i = 0; i < totalSize; i++) {
		*((char *) addr + i) = *((char *) (img + i));
	}
}
