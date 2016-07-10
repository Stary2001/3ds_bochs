#include <3ds.h>
#include <sf2d.h>
#include <string.h>

#include "3ds_font.h"

Font::Font(sf2d_texture *backing, const u8 *dat) 
{
	tex = backing;
	font_data = dat;
}

void Font::add_string(const char *c, FontStr &target)
{
	if(x + strlen(c) * 8 >= 256)
	{
		x = 0;
		y += 8;
	}

	target.x = x;
	target.y = y;
	target.w = strlen(c) * 8;
	target.h = 8;

	while(*c)
	{
		draw_char(x, y, *c++);
		x += 8;
	}
}

void Font::draw_char(int x, int y, char c)
{
  if(c > 128)
  {
    return;
  }

  uint32_t bytes_per_char = 8;
  const uint8_t *letter = &font_data[c * bytes_per_char];
  uint32_t xp,yp = 0;

  u8 *dat = (u8*)tex->data;

  // This one isn't in z-order. apparently. cool.

  for(yp=0 ; yp < 8; yp++)
  {
    for(xp=0; xp < 8; xp++)
    {
      uint32_t xx = x + xp;
      uint32_t yy = y + yp;

      if(letter[yp] & (1 << xp))
      {
        int ind = (xx + yy * tex->pow2_w) * 4;
        dat[ind] = 0xff;
        dat[ind+1] = 0xff;
        dat[ind+2] = 0xff;
        dat[ind+3] = 0xff;
      }
      else
      {
        int ind = (xx + yy * tex->pow2_w) * 4;
        dat[ind] = 0;
        dat[ind+1] = 0;
        dat[ind+2] = 0xff;
        dat[ind+3] = 0xff;
      }
    }
  }
}
