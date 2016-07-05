#define KEY_SIZE 20
#define NUM_KEYS 54

struct key
{
  Bit32u code;
  const char *c[2];
  int x;
  int y;
  int w;
  int h;
};

const key map[] = // Keys are 24x24.
{
  {BX_KEY_GRAVE, {"`", "¬"}, 0, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_1, {"1", "!"}, KEY_SIZE, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_2, {"2", "\""}, KEY_SIZE * 2, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_3, {"3", "£"}, KEY_SIZE * 3, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_4, {"4", "$"}, KEY_SIZE * 4, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_5, {"5", "%"}, KEY_SIZE * 5, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_6, {"6", "^"}, KEY_SIZE * 6, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_7, {"7", "&"}, KEY_SIZE * 7, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_8, {"8", "*"}, KEY_SIZE * 8, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_9, {"9", "("}, KEY_SIZE * 9, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_0, {"0", ")"}, KEY_SIZE * 10, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_MINUS, {"-", "_"}, KEY_SIZE * 11, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_EQUALS, {"=", "+"}, KEY_SIZE * 12, 0, KEY_SIZE, KEY_SIZE},
  {BX_KEY_BACKSPACE, {"Bksp", "Bksp"}, KEY_SIZE * 13, 0, KEY_SIZE * 2, KEY_SIZE},

  {BX_KEY_TAB, {"Tab", "Tab"}, 0, KEY_SIZE, KEY_SIZE + KEY_SIZE / 2, KEY_SIZE},
  {BX_KEY_Q, {"q", "Q"}, KEY_SIZE + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_W, {"w", "W"}, KEY_SIZE * 2+ KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_E, {"e", "E"}, KEY_SIZE * 3 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_R, {"r", "R"}, KEY_SIZE * 4 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_T, {"t", "T"}, KEY_SIZE * 5 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_Y, {"y", "Y"}, KEY_SIZE * 6 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_U, {"u", "U"}, KEY_SIZE * 7 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_I, {"i", "I"}, KEY_SIZE * 8 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_O, {"o", "O"}, KEY_SIZE * 9 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_P, {"p", "P"}, KEY_SIZE * 10 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_LEFT_BRACKET, {"[", "{"}, KEY_SIZE * 11 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},
  {BX_KEY_RIGHT_BRACKET, {"]", "}"}, KEY_SIZE * 12 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE, KEY_SIZE},

  {BX_KEY_CAPS_LOCK, {"Caps", "Caps"}, 0, KEY_SIZE * 2, KEY_SIZE + KEY_SIZE / 2, KEY_SIZE},
  {BX_KEY_A, {"a", "A"}, KEY_SIZE + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_S, {"s", "S"}, KEY_SIZE * 2+ KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_D, {"d", "D"}, KEY_SIZE * 3 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_F, {"f", "F"}, KEY_SIZE * 4 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_G, {"g", "G"}, KEY_SIZE * 5 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_H, {"h", "H"}, KEY_SIZE * 6 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_J, {"j", "J"}, KEY_SIZE * 7 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_K, {"k", "K"}, KEY_SIZE * 8 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_L, {"l", "L"}, KEY_SIZE * 9 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_SEMICOLON, {";", ":"}, KEY_SIZE * 10 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_SINGLE_QUOTE, {"'", "@"}, KEY_SIZE * 11 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},
  {BX_KEY_BACKSLASH, {"#", "~"}, KEY_SIZE * 12 + KEY_SIZE / 2, KEY_SIZE * 2, KEY_SIZE, KEY_SIZE},

  {BX_KEY_SHIFT_L, {"->", "->"}, 0, KEY_SIZE * 3, KEY_SIZE + KEY_SIZE / 2, KEY_SIZE},
  {BX_KEY_LEFT_BACKSLASH, {"\\", "|"}, KEY_SIZE + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_Z, {"z", "S"}, KEY_SIZE * 2+ KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_X, {"x", "D"}, KEY_SIZE * 3 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_C, {"c", "F"}, KEY_SIZE * 4 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_V, {"v", "G"}, KEY_SIZE * 5 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_B, {"b", "H"}, KEY_SIZE * 6 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_N, {"n", "J"}, KEY_SIZE * 7 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_M, {"m", "K"}, KEY_SIZE * 8 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_COMMA, {",", "<"}, KEY_SIZE * 9 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_PERIOD, {".", ">"}, KEY_SIZE * 10 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_SLASH, {"'", "@"}, KEY_SIZE * 11 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},
  {BX_KEY_SHIFT_R, {"->", "->"}, KEY_SIZE * 12 + KEY_SIZE / 2, KEY_SIZE * 3, KEY_SIZE, KEY_SIZE},

  {BX_KEY_ENTER, {"Ret", "Ret"}, KEY_SIZE * 13 + KEY_SIZE / 2, KEY_SIZE, KEY_SIZE + KEY_SIZE / 2, KEY_SIZE * 2}
};
