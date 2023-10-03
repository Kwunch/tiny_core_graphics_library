#ifndef _graphics_h_
#define _graphics_h_

enum colors {
	black = 0x0000,
	navy = 0x000F,
	darkgreen = 0x03E0,
	darkcyan = 0x03EF,
	maroon = 0x7800,
	purple = 0x780F,
	olive = 0x7BE0,
	lightgrey = 0xC618,
	darkgrey = 0x7BEF,
	blue = 0x001F,
	green = 0x07E0,
	cyan = 0x07FF,
	red = 0xF800,
	magenta = 0xF81F,
	yellow = 0xFFE0,
	white = 0xFFFF,
	orange = 0xFD20,
	greenyellow = 0xAFE5,
	pink = 0xFC9F,
};
typedef unsigned short color_t;

#define BLACK ((color_t)black)
#define BLUE ((color_t)blue)
#define PINK ((color_t)pink)
#define WHITE ((color_t)white)
#define PURPLE ((color_t)purple)
#define CYAN ((color_t)cyan)
#define GREEN ((color_t)green)
#define HILBERT (color_t)0x1800
#define RED (color_t)red
#define ORANGE (color_t)orange

//#define RGB(r, g, b) (color_t)(((r & 0x1F) << 11) | ((g & 0x3F) << 5) | ((b & 0x1F) << 0))
#define RGB(r, g, b) ((color_t)(((r >> 3) << 11) | ((g >> 2) << 5) | b >> 3))

void init_graphics();

void exit_graphics();

char getkey();

void sleep_ms(long ms);

void clear_screen(void *img);

void draw_pixel(void *img, int x, int y, color_t color);

void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c);

void draw_circle(void *img, int x, int y, int r, color_t color);

void *new_offscreen_buffer();

void blit(void *src);

#endif
