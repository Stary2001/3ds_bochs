
//  Copyright (C) 2016  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "plugin.h"
#include "param_names.h"
#include "iodev.h"

#if BX_WITH_3DS
#include "icon_bochs.h"

#include <3ds.h>
#include "3ds_5x8_font.h"
#include "3ds_8x8_font.h"
#include "3ds_keymap.h"
#include "3ds_font.h"

#define NUM_SCR_MODES 2
#define NUM_INP_MODES 2

#define SCR_SCALE 0
#define SCR_PAN 1
#define INP_KEYBOARD 0
#define INP_MOUSE 1

#define NUM_BITMAPS 16

const char *screen_mode_names[] =
{
  "Scale",
  "Pan"
};

const char *input_mode_names[] =
{
  "Keyboard",
  "Mouse"
};

class bx_3ds_gui_c : public bx_gui_c {
public:
  bx_3ds_gui_c (void) {}
  DECLARE_GUI_VIRTUAL_METHODS()
  DECLARE_GUI_NEW_VIRTUAL_METHODS()

  void draw_mouse();
  void draw_keyboard();
  void draw_key(int k);
  void draw_rectangle(int x, int y, int w, int h, u32 colour);

  void vga_draw_char(int x, int y, char c);
  void font_draw_char(int x, int y, char c);
  #if BX_SHOW_IPS
    virtual void show_ips(Bit32u ips_count);
  #endif

  unsigned int guest_xchars;
  unsigned int guest_ychars;

  void *screen_fb;
  u8 *top_fb;
  u16 top_w;
  u16 top_h;

  u8 *bottom_fb;
  u16 bottom_w;
  u16 bottom_h;

  const char *static_strs[5];
  Font *font;
  int current_screen_mode;
  int current_input_mode;

  bool key_state[NUM_KEYS];
  bool shift;
  bool ctrl;
  bool alt;

  u32 vga_fg;
  u32 vga_bg;

  u32 palette[256];

  u32 pan_x;
  u32 pan_y;

  u32 mouse_last_x;
  u32 mouse_last_y;

  bool mouse_state;

  double mouse_sens;
  u32 button_state;

  bool ips_update;
  char *ips_text;

  double screen_xscale;
  double screen_yscale;
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_3ds_gui_c *theGui = NULL;
IMPLEMENT_GUI_PLUGIN_CODE(3ds)

#define LOG_THIS theGui->

void bx_3ds_gui_c::specific_init(int argc, char **argv, unsigned headerbar_y)
{
  //APT_SetAppCpuTimeLimit(80);

  BX_INFO(("bx_3ds_gui_c::specific_init"));

  gfxSetScreenFormat(GFX_TOP, GSP_RGBA8_OES);
  gfxSetScreenFormat(GFX_BOTTOM, GSP_RGBA8_OES);
  gfxSetDoubleBuffering(GFX_TOP, false);
  gfxSetDoubleBuffering(GFX_BOTTOM, false);

  gfxSwapBuffersGpu();
  gspWaitForVBlank();

  UNUSED(argc);
  UNUSED(argv);
  UNUSED(headerbar_y);
  UNUSED(bochs_icon_bits);  // global variable

  if (SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get())
  {
    BX_INFO(("private_colormap option ignored."));
  }

  top_fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &top_w, &top_h);
  bottom_fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, &bottom_w, &bottom_h);

  BX_INFO(("top: %ix%i ", top_w, top_h));
  BX_INFO(("bottom: %ix%i ", bottom_w, bottom_h));

  memset(top_fb, 0, top_w * top_h * 4);
  memset(bottom_fb, 0, bottom_w * bottom_h * 4);

  screen_fb = malloc(400 * 240 * 4);
  memset(screen_fb, 0, 400 * 240 * 4);

  screen_xscale = 1;
  screen_yscale = 1;

  int i;

  font = new Font(bottom_fb, bottom_w, bottom_h, font_8x8_data);

  static_strs[0] = "Current screen mode: ";
  static_strs[1] = "Current input mode: ";
  static_strs[2] = "Mouse disabled";
  static_strs[3] = "Mouse enabled";
  static_strs[4] = "IPS: ";

  shift = false;
  ctrl = false;
  alt = false;

  mouse_state = false;
  mouse_sens = 2.0;
  button_state = 0;

  pan_x = 0;
  pan_y = 0;

  ips_text = (char*)malloc(16);
  sprintf(ips_text, "%u.%3.3u", 99, 9999);
  ips_update = false;

  draw_keyboard();
}

bool aabb(int xp, int yp, int x, int y, int w, int h)
{
  return xp >= x && xp <= x + w &&
         yp >= y && yp <= y + h;
}

void bx_3ds_gui_c::handle_events(void)
{
  if(!aptMainLoop())
  {
    BX_PANIC(("APT wants us dead."));
  }
  hidScanInput();

  uint32_t press = hidKeysDown();
  uint32_t hold = hidKeysHeld();

  if(current_input_mode == INP_KEYBOARD)
  {
    if((press & KEY_TOUCH)) // run on touch
    {
      touchPosition p;
      hidTouchRead(&p);

      int i = 0;

      for(; i < NUM_KEYS; i++)
      {
        if(p.px > map[i].x && p.px < (map[i].x + map[i].w) &&
          p.py > map[i].y && p.py < (map[i].y + map[i].h))
        {
          if(map[i].flags & KEY_SHIFT)
          {
            shift = !shift;
            draw_keyboard();
          }
          else if(map[i].flags & KEY_CTRL)
          {
            ctrl = !ctrl;
          }
          else if(map[i].flags & KEY_ALT)
          {
            alt = !alt;
          }
          else
          {
            key_state[i] = !key_state[i];
            if(shift)
            {
              DEV_kbd_gen_scancode(BX_KEY_SHIFT_L);
            }
            if(ctrl)
            {
              DEV_kbd_gen_scancode(BX_KEY_CTRL_L);
            }
            if(alt)
            {
              DEV_kbd_gen_scancode(BX_KEY_ALT_L);
            }

            DEV_kbd_gen_scancode(map[i].code);
          }
          draw_key(i);
        }
      }
    }
    else if(!(hold & KEY_TOUCH)) // not held? ie, released
    {
      int i = 0;
      for(; i < NUM_KEYS; i++)
      {
        if(key_state[i] && !(map[i].flags & KEY_STICKY))
        {
          if(shift)
          {
            DEV_kbd_gen_scancode(BX_KEY_SHIFT_L | BX_KEY_RELEASED);
            shift = false;
            draw_keyboard();
          }

          if(ctrl)
          {
            DEV_kbd_gen_scancode(BX_KEY_CTRL_L | BX_KEY_RELEASED);
            ctrl = false;
          }

          if(alt)
          {
            DEV_kbd_gen_scancode(BX_KEY_ALT_L | BX_KEY_RELEASED);
            alt = false;
          }

          DEV_kbd_gen_scancode(map[i].code | BX_KEY_RELEASED);
          key_state[i] = false;

          draw_key(i);
        }
      }
    }
  }
  else if(current_input_mode == INP_MOUSE)
  {
    // int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy)

    u32 old_buttons = button_state;

    touchPosition pos;
    hidTouchRead(&pos);

    u32 p = press | hold;
    if((p & KEY_TOUCH))
    {
      if(aabb(pos.px, pos.py, 10, 10, 220, 140))
      {
        if(mouse_last_x != 0 && mouse_last_y != 0)
        {
          s32 dx = pos.px - mouse_last_x;
          s32 dy = pos.py - mouse_last_y;
          if(dx != 0 || dy != 0)
          {
            DEV_mouse_motion(dx * mouse_sens, -dy * mouse_sens, 0, button_state, 0);
          }
        }

        mouse_last_x = pos.px;
        mouse_last_y = pos.py;
      }
      else if(aabb(pos.px, pos.py, 10, 150, 150, 20))
      {
        button_state |= 1;
        mouse_last_x = mouse_last_y = 0;
      }
      else if(aabb(pos.px, pos.py, 160, 150, 150, 20))
      {
        button_state |= (1 << 1);
        mouse_last_x = mouse_last_y = 0;
      }
    }
    else
    {
      button_state = 0;
      mouse_last_x = mouse_last_y = 0;
    }

    if(button_state != old_buttons)
    {
      DEV_mouse_motion(0, 0, 0, button_state, 0);
    }
  }

  if(current_screen_mode == SCR_PAN && !guest_textmode)
  {
    u32 p = press | hold;

    if((p & KEY_UP) && pan_y != 0)
    {
      pan_y--;
    }
    if((p & KEY_DOWN) && pan_y < guest_yres - 240)
    {
      pan_y++;
    }

    if((p & KEY_LEFT) && pan_x != 0)
    {
      pan_x--;
    }
    if((p & KEY_RIGHT) && pan_x < guest_xres - 400)
    {
      pan_x++;
    }
  }

  if((press | hold) & KEY_SELECT) // select pressed/held?
  {
    if((press & KEY_L) && current_input_mode != 0)
    {
      current_input_mode--;
    }

    if((press & KEY_R) && current_input_mode != NUM_INP_MODES - 1)
    {
      current_input_mode++;
    }

    if(mouse_state && current_input_mode != INP_MOUSE)
    {
      toggle_mouse_enable();
    }

    if(!mouse_state && current_input_mode == INP_MOUSE)
    {
      toggle_mouse_enable();
    }

    memset(bottom_fb, 0, bottom_w * bottom_h * 4); // clear bottom screen..
    if(current_input_mode == INP_KEYBOARD)
    {
      draw_keyboard();
    }
    else if(current_input_mode == INP_MOUSE)
    {
      draw_mouse();
    }
  }
  else
  {
    if((press & KEY_L) && current_screen_mode != 0)
    {
      current_screen_mode--;
    }

    if((press & KEY_R) && current_screen_mode != NUM_SCR_MODES - 1)
    {
      current_screen_mode++;
    }
    //clear_screen();
  }

  u32 keys = KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R;

  if((hold & keys) == keys)
  {
    SIM->quit_sim(0);
  }
}

void bx_3ds_gui_c::draw_rectangle(int x, int y, int w, int h, u32 colour)
{
  int xx, yy;
  u32 *dat = (u32*)bottom_fb;

  for(yy = y; yy < y + h ; yy++)
  {
    for(xx = x; xx < x + w; xx++)
    {
      int ind = xx * bottom_w + (bottom_w - yy);
      dat[ind] = colour;
    }
  }
}

void bx_3ds_gui_c::draw_key(int k)
{
  bool active = key_state[k];
  if(map[k].flags & KEY_SHIFT)
  {
    active = shift;
  }
  if(map[k].flags & KEY_CTRL)
  {
    active = ctrl;
  }
  if(map[k].flags & KEY_ALT)
  {
    active = alt;
  }
  draw_rectangle(map[k].x, map[k].y, map[k].w, map[k].h, active ? 0xff0000ff : 0xffffffff);
  font->draw_string(map[k].x + 2, map[k].y + 2, map[k].c[(int)shift]);
}

void bx_3ds_gui_c::draw_keyboard()
{
  int i = 0;
  for(; i < NUM_KEYS; i++)
  {
    draw_key(i);
  }
}

void bx_3ds_gui_c::draw_mouse()
{
  draw_rectangle(10, 10, 300, 140, 0xffffffff);
  draw_rectangle(10, 150, 150, 20, ((button_state & 1) != 0) ? 0xffff0000 : 0xff00ff00);
  draw_rectangle(160, 150, 150, 20, ((button_state & (1<<1)) != 0) ? 0xff0000ff : 0xff00ff00);
}

void bx_3ds_gui_c::flush(void)
{
  /*sf2d_start_frame(GFX_TOP, GFX_LEFT);
  if(current_screen_mode == SCR_SCALE)
  {
    sf2d_draw_texture_scale(screen_tex, 0, 0, screen_xscale, screen_yscale);
  }
  else if(current_screen_mode == SCR_PAN)
  {
    sf2d_draw_texture_part(screen_tex, 0, 0, pan_x, pan_y, 400, 240);
  }
  sf2d_end_frame();


  FontStr curr = static_strs[0];
  sf2d_draw_texture_part(font_tex, 10, 200, curr.x, curr.y, curr.w, curr.h);

  FontStr mode_str = screen_mode_strs[current_screen_mode];
  sf2d_draw_texture_part(font_tex, 10 + curr.w, 200, mode_str.x, mode_str.y, mode_str.w, mode_str.h);

  curr = static_strs[1];
  sf2d_draw_texture_part(font_tex, 10, 208, curr.x, curr.y, curr.w, curr.h);

  mode_str = input_mode_strs[current_input_mode];
  sf2d_draw_texture_part(font_tex, 10 + curr.w, 208, mode_str.x, mode_str.y, mode_str.w, mode_str.h);

  FontStr mouse_str = static_strs[2 + (int)mouse_state];
  sf2d_draw_texture_part(font_tex, 10, 216, mouse_str.x, mouse_str.y, mouse_str.w, mouse_str.h);

  FontStr ips_str = static_strs[5];
  sf2d_draw_texture_part(font_tex, 10, 224, ips_str.x, ips_str.y, ips_str.w, ips_str.h);

  FontStr nums_str = static_strs[4];
  int s_i = 0;
  int s_off = 0;
  int x = 10 + ips_str.w;

  // draw ips_str

  sf2d_end_frame();
  sf2d_swapbuffers();*/
}

void bx_3ds_gui_c::clear_screen(void)
{
  memset(top_fb, 0, top_w * top_h * 4);
}

void bx_3ds_gui_c::vga_draw_char(int x, int y, char c)
{
  uint32_t bytes_per_char = 8;
  const uint8_t *letter = &font_5x8_data[c * bytes_per_char];
  uint32_t xp,yp = 0;

  Bit32u *dat = (Bit32u*)top_fb;

  for(yp=0 ; yp < 8; yp++)
  {
    for(xp=0; xp < 5; xp++)
    {
      uint32_t xx = (x*5) + xp;
      uint32_t yy = (y * 8) + yp;

      int ind = xx * top_w + (top_w - yy);
      if(letter[yp + ((xp / 5) * bytes_per_char)] & (1 << (7 - xp)))
      {
        dat[ind] = vga_fg;
      }
      else
      {
        dat[ind] = vga_bg;
      }
    }
  }
}

// Stolen from term.cc, slightly adapted.

void bx_3ds_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
        unsigned long cursor_x, unsigned long cursor_y,
        bx_vga_tminfo_t *tm_info)
{

  // rgb565
  const u32 vga_colours[] =
  {
    0x000000ff, // black #000000
    0x0000aaff, // blue #0000aa
    0x00aa00ff, // green #00aa00
    0x00aaaaff, // cyan #00aaaa
    0xaa0000ff, // red #aa0000
    0xaa00aaff, // magenta #aa00aa
    0xaa5500ff, // brown #aa5500
    0xaaaaaaff, // grey #aaaaaa
    0x555555ff, // dark grey #555555
    0x5555ffff, // bright blue #5555ff
    0x55ff55ff, // bright green #55ff55
    0x55ffffff, // bright cyan #55ffff
    0xff5555ff, // bright red #ff5555
    0xff55ffff, // bright magenta #ff55ff
    0xffff55ff, // yellow #ffff55
    0xffffffff // white #ffffff
  };

  unsigned char *old_line, *new_line, *new_start;
  unsigned char cAttr;
  unsigned int hchars, rows, x, y;
  char ch;
  bx_bool force_update = 0;

  if(charmap_updated) {
    force_update = 1;
    charmap_updated = 0;
  }

  new_start = new_text;
  rows = guest_ychars;
  y = 0;
  do {
    hchars = guest_xchars;
    new_line = new_text;
    old_line = old_text;
    x = 0;
    do {
      if (force_update || (old_text[0] != new_text[0])
          || (old_text[1] != new_text[1])) {
        
        vga_fg = vga_colours[new_text[1] & 0xf];
        vga_bg = vga_colours[(new_text[1] & 0xf0) >> 4];

        ch = new_text[0];
        //if ((new_text[1] & 0x08) > 0) ch |= A_BOLD;
        //if ((new_text[1] & 0x80) > 0) ch |= A_BLINK;
        vga_draw_char(x, y, ch);
      }
      x++;
      new_text+=2;
      old_text+=2;
    } while (--hchars);
    y++;
    new_text = new_line + tm_info->line_offset;
    old_text = old_line + tm_info->line_offset;
  } while (--rows);

  if ((cursor_x<guest_xchars) && (cursor_y<guest_ychars)
      && (tm_info->cs_start <= tm_info->cs_end)) {
    if(cursor_x>0)
      cursor_x--;
    else {
      cursor_x=25-1;
      cursor_y--;
    }

    cAttr = new_start[cursor_y*tm_info->line_offset+cursor_x*2+1];
    vga_fg = vga_colours[cAttr & 0xf];
    vga_bg = vga_colours[(cAttr & 0xf0) >> 8];

    ch = new_start[cursor_y*tm_info->line_offset+cursor_x*2];
    vga_draw_char(x, y, ch);
    //curs_set(2);
  } else {
    //curs_set(0);
  }
}


int bx_3ds_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  UNUSED(bytes);
  UNUSED(nbytes);
  return 0;
}

int bx_3ds_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  UNUSED(text_snapshot);
  UNUSED(len);
  return 0;
}

bx_bool bx_3ds_gui_c::palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue)
{
  // 0xRRGGBBAA
  palette[index] = red << 24 | green << 16 | blue << 8 | 0xff;
  return(0);
}

void bx_3ds_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  u32 x = 0;
  u32 y = 0;
  int i = 0;
  Bit32u *dat = (Bit32u*)top_fb;

  for(; y < y_tilesize; y++)
  {
    for(x = 0; x < x_tilesize; x++)
    {
      int ind = (x0 + x) * top_w + (top_w - (y0 + y));
      dat[ind] = palette[tile[i]];
      i++;
    }
  }
}

bx_svga_tileinfo_t * bx_3ds_gui_c::graphics_tile_info(bx_svga_tileinfo_t *info)
{
  info->bpp = 32;
  info->pitch = 240 * 4;
  info->red_shift = 24;
  info->green_shift = 16;
  info->blue_shift = 8;
  info->red_mask =   0xff000000;
  info->green_mask = 0x00ff0000;
  info->blue_mask =  0x0000ff00;

  info->is_indexed = false;
  info->is_little_endian = 1;

  return info;
}

Bit8u * bx_3ds_gui_c::graphics_tile_get(unsigned x, unsigned y, unsigned *w, unsigned *h)
{
  if (x + x_tilesize > guest_xres)
  {
    *w = guest_xres - x;
  }
  else
  {
    *w = x_tilesize;
  }

  if (y + y_tilesize > guest_yres)
  {
    *h = guest_yres - y;
  }
  else
  {
    *h = y_tilesize;
  }

  Bit8u *dat = (Bit8u*) screen_fb;
  return dat + (y * 400 * 4 + (x * 4));
}

void bx_3ds_gui_c::graphics_tile_update_in_place(unsigned x, unsigned y, unsigned w, unsigned h)
{
  UNUSED(x);
  UNUSED(y);
  UNUSED(w);
  UNUSED(h);

  // Just do nothing. It's fine. Really.
}

void bx_3ds_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight, unsigned fwidth, unsigned bpp)
{
  guest_textmode = (fheight > 0);
  guest_xres = x;
  guest_yres = y;
  guest_bpp = bpp;

  if(guest_textmode)
  {
    guest_xchars = x / fwidth;
    guest_ychars = y / fheight;
    screen_xscale = 1.0;
    screen_yscale = 1.0;
    pan_x = 0;
    pan_y = 0;
  }
  else
  {
    free(screen_fb);
    screen_fb = malloc(x * y * 4);
    memset(screen_fb, 0, x * y * 4);
    screen_xscale = 400.0 / x;
    screen_yscale = 240.0 / y;

    if(x <= 400)
    {
      pan_x = 0;
    }
    
    if(y <= 240)
    {
      pan_y = 0;
    }
  }

  BX_INFO(("mode switch to %i by %i at %i bpp, text mode: %i", x, y, bpp, (int)guest_textmode));
}

unsigned bx_3ds_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
  UNUSED(bmap);
  UNUSED(xdim);
  UNUSED(ydim);
  return(0);
}

unsigned bx_3ds_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
  UNUSED(bmap_id);
  UNUSED(alignment);
  UNUSED(f);
  return(0);
}

void bx_3ds_gui_c::show_headerbar(void)
{
}

void bx_3ds_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  UNUSED(hbar_id);
  UNUSED(bmap_id);
}

void bx_3ds_gui_c::exit(void)
{
  ::exit(0);
}

void bx_3ds_gui_c::mouse_enabled_changed_specific(bx_bool val)
{
  mouse_state = (bool)val;
}

#if BX_SHOW_IPS
void bx_3ds_gui_c::show_ips(Bit32u ips_count)
{
  ips_count /= 1000;
  sprintf(ips_text, "%u.%3.3u", ips_count / 1000, ips_count % 1000);
  ips_update = true;
}
#endif

#endif /* if BX_WITH_3ds */
