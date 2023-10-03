#include "graphics.h"
#include <stddef.h>

void *src;

int main() {
	int height = 640;
	int width = 480;
	init_graphics();
	src = new_offscreen_buffer();

	draw_line(src, 0, 0, 480, 480, PINK);
	draw_circle(src, 320, 240, 60, PURPLE);
	blit(src);
	
	do {
		char key = getkey();
		if (key == 'q') {
			break;
		} else if (key == 'h') {
			clear_screen(src);
			draw_line(src, 80, 120, 480, 120, RGB(31, 0, 0));
			draw_line(src, 80, 280, 480, 280, WHITE);
			draw_line(src, 80, 200, 480, 200, RGB(31, 0, 0));
			draw_line(src, 80, 320, 480, 320, CYAN);
			blit(src);
		} else if (key == 'm') {
			draw_circle(src, 240, 100, 80, RGB(31, 0, 0));
			sleep_ms(40);
			draw_circle(src, 100, 50, 25, RGB(255, 255, 255));
			blit(src);
		} else  if (key == 'a') {
			clear_screen(src);
			sleep_ms(500);
			int x = 320;
			int xf = 0;
			int y = 60;
			int yf = 240;
			while ( x > xf) {
				draw_circle(src, x + 1, y, 30, BLACK);
				draw_circle(src, x, y, 30, RGB(0,0,255));
				sleep_ms(1);
				blit(src);
				x--;
			}
			draw_circle(src, x + 1, y, 30, BLACK);
			draw_circle(src, x, y, 30, RGB(0,0,255));
			sleep_ms(1);
			blit(src);
			clear_screen(src);
			while (y < yf) {
				draw_circle(src, x, y - 1, 30, BLACK);
				draw_circle(src, x, y, 30, GREEN);
				sleep_ms(1);
				blit(src);
				y++;
			}
		} else if (key == 't') {
			clear_screen(src);
			int firstX = (width / 2) + 80;
			int firstY = (height / 2) - 250;
			int iSize = 256;
			drawSTriangle(firstX, firstY, iSize);
		}
	} while(1);
	
	clear_screen(src);
	sleep_ms(1000);
	exit_graphics();
}

void drawSTriangle(int x, int y, int size) {
	if (size == 1) {
		draw_pixel(src, x, y, WHITE);
		blit(src);
		sleep_ms(1);
	} else {
		int newSize = size / 2;
		
		drawSTriangle(x, y, newSize);
		drawSTriangle(x - newSize / 2, y + newSize, newSize);
		drawSTriangle(x + newSize / 2, y + newSize, newSize);
	}
}