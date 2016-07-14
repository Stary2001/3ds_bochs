#pragma once

struct FontStr
{
	int x;
	int y;
	int w;
	int h;
};

class Font
{
public:
	Font(u8 *framebuffer, u16 width, u16 height, const u8 *dat);
	void draw_string(int x, int y, const char *c);
	void draw_char(int x, int y, char c);
private:
	u8 *fb;
	const u8 *font_data;
	int x;
	int y;
	u16 w;
	u16 h;
};