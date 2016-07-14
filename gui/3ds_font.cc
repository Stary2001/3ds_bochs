#include <3ds.h>
#include <sf2d.h>
#include <string.h>

#include "3ds_font.h"

Font::Font(u8 *framebuffer, u16 fb_w, u16 fb_h, const u8 *dat)
{
	fb = framebuffer;
	font_data = dat;
  w = fb_w;
  h = fb_h;
}

void Font::draw_string(int x, int y, const char *c)
{
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

  for(yp=0 ; yp < 8; yp++)
  {
    for(xp=0; xp < 8; xp++)
    {
      uint32_t xx = x + xp;
      uint32_t yy = y + yp;
      int ind = xx * w + (w - yy);
      ind *= 4;

      if(letter[yp] & (1 << xp))
      {
        fb[ind] = 0xff;
        fb[ind+1] = 0xff;
        fb[ind+2] = 0xff;
        fb[ind+3] = 0xff;
      }
      else
      {
        fb[ind] = 0xff;
        fb[ind+1] = 0xff;
        fb[ind+2] = 0;
        fb[ind+3] = 0;
      }
    }
  }
}
