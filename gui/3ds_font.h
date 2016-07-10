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
	Font(sf2d_texture *backing, const u8 *dat);
	void add_string(const char *c, FontStr &target);
private:
	void draw_char(int x, int y, char c);
	sf2d_texture *tex;
	const u8 *font_data;
	int x;
	int y;
};