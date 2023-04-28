/* ****************************************************************************
 *
 * ANTI-CAPITALIST SOFTWARE LICENSE (v 1.4)
 *
 * Copyright © 2023 erysdren (it/they/she)
 *
 * This is anti-capitalist software, released for free use by individuals
 * and organizations that do not operate by capitalist principles.
 *
 * Permission is hereby granted, free of charge, to any person or
 * organization (the "User") obtaining a copy of this software and
 * associated documentation files (the "Software"), to use, copy, modify,
 * merge, distribute, and/or sell copies of the Software, subject to the
 * following conditions:
 *
 *   1. The above copyright notice and this permission notice shall be
 *   included in all copies or modified versions of the Software.
 *
 *   2. The User is one of the following:
 *     a. An individual person, laboring for themselves
 *     b. A non-profit organization
 *     c. An educational institution
 *     d. An organization that seeks shared profit for all of its members,
 *     and allows non-members to set the cost of their labor
 *
 *   3. If the User is an organization with owners, then all owners are
 *   workers and all workers are owners with equal equity and/or equal vote.
 *
 *   4. If the User is an organization, then the User is not law enforcement
 *   or military, or working for or under either.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT EXPRESS OR IMPLIED WARRANTY OF
 * ANY KIND, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ************************************************************************* */

/*
 * keyboard scancodes (dos)
 */

#ifdef QMDLVIEW_DOS

enum
{
	KEY_ESCAPE = 0x01,
	KEY_1 = 0x02,
	KEY_2 = 0x03,
	KEY_3 = 0x04,
	KEY_4 = 0x05,
	KEY_5 = 0x06,
	KEY_6 = 0x07,
	KEY_7 = 0x08,
	KEY_8 = 0x09,
	KEY_9 = 0x0A,
	KEY_0 = 0x0B,
	KEY_MINUS = 0x0C,
	KEY_EQUALS = 0x0D,
	KEY_BACKSPACE = 0x0E,
	KEY_TAB = 0x0F,
	KEY_Q = 0x10,
	KEY_W = 0x11,
	KEY_E = 0x12,
	KEY_R = 0x13,
	KEY_T = 0x14,
	KEY_Y = 0x15,
	KEY_U = 0x16,
	KEY_I = 0x17,
	KEY_O = 0x18,
	KEY_P = 0x19,
	KEY_LEFTBRACKET = 0x1A,
	KEY_RIGHTBRACKET = 0x1B,
	KEY_ENTER = 0x1C,
	KEY_CONTROL = 0x1D,
	KEY_A = 0x1E,
	KEY_S = 0x1F,
	KEY_D = 0x20,
	KEY_F = 0x21,
	KEY_G = 0x22,
	KEY_H = 0x23,
	KEY_J = 0x24,
	KEY_K = 0x25,
	KEY_L = 0x26,
	KEY_SEMICOLON = 0x27,
	KEY_QUOTE = 0x28,
	KEY_TILDE = 0x29,
	KEY_LSHIFT = 0x2A,
	KEY_BACKSLASH = 0x2B,
	KEY_Z = 0x2C,
	KEY_X = 0x2D,
	KEY_C = 0x2E,
	KEY_V = 0x2F,
	KEY_B = 0x30,
	KEY_N = 0x31,
	KEY_M = 0x32,
	KEY_COMMA = 0x33,
	KEY_PERIOD = 0x34,
	KEY_SLASH = 0x35,
	KEY_RSHIFT = 0x36,
	KEY_MULTIPLY = 0x37,
	KEY_ALT = 0x38,
	KEY_SPACE = 0x39,
	KEY_CAPSLOCK = 0x3A,
	KEY_F1 = 0x3B,
	KEY_F2 = 0x3C,
	KEY_F3 = 0x3D,
	KEY_F4 = 0x3E,
	KEY_F5 = 0x3F,
	KEY_F6 = 0x40,
	KEY_F7 = 0x41,
	KEY_F8 = 0x42,
	KEY_F9 = 0x43,
	KEY_F10 = 0x44,
	KEY_NUMLOCK = 0x45,
	KEY_SCROLLLOCK = 0x46,
	KEY_HOME = 0x47,
	KEY_UP = 0x48,
	KEY_PAGEUP = 0x49,
	KEY_LEFT = 0x4B,
	KEY_RIGHT = 0x4D,
	KEY_PLUS = 0x4E,
	KEY_END = 0x4F,
	KEY_DOWN = 0x50,
	KEY_PAGEDOWN = 0x51,
	KEY_INSERT = 0x52,
	KEY_DELETE = 0x53,
	KEY_F11 = 0x57,
	KEY_F12 = 0x58
};

#else

/*
 * keyboard scancodes (from sdl2)
 */

enum
{
	KEY_A = 4,
	KEY_B = 5,
	KEY_C = 6,
	KEY_D = 7,
	KEY_E = 8,
	KEY_F = 9,
	KEY_G = 10,
	KEY_H = 11,
	KEY_I = 12,
	KEY_J = 13,
	KEY_K = 14,
	KEY_L = 15,
	KEY_M = 16,
	KEY_N = 17,
	KEY_O = 18,
	KEY_P = 19,
	KEY_Q = 20,
	KEY_R = 21,
	KEY_S = 22,
	KEY_T = 23,
	KEY_U = 24,
	KEY_V = 25,
	KEY_W = 26,
	KEY_X = 27,
	KEY_Y = 28,
	KEY_Z = 29,
	KEY_1 = 30,
	KEY_2 = 31,
	KEY_3 = 32,
	KEY_4 = 33,
	KEY_5 = 34,
	KEY_6 = 35,
	KEY_7 = 36,
	KEY_8 = 37,
	KEY_9 = 38,
	KEY_0 = 39,
	KEY_RETURN = 40,
	KEY_ESCAPE = 41,
	KEY_BACKSPACE = 42,
	KEY_TAB = 43,
	KEY_SPACE = 44,
	KEY_MINUS = 45,
	KEY_EQUALS = 46,
	KEY_LEFTBRACKET = 47,
	KEY_RIGHTBRACKET = 48,
	KEY_BACKSLASH = 49,
	KEY_NONUSHASH = 50,
	KEY_SEMICOLON = 51,
	KEY_APOSTROPHE = 52,
	KEY_GRAVE = 53,
	KEY_COMMA = 54,
	KEY_PERIOD = 55,
	KEY_SLASH = 56,
	KEY_CAPSLOCK = 57,
	KEY_F1 = 58,
	KEY_F2 = 59,
	KEY_F3 = 60,
	KEY_F4 = 61,
	KEY_F5 = 62,
	KEY_F6 = 63,
	KEY_F7 = 64,
	KEY_F8 = 65,
	KEY_F9 = 66,
	KEY_F10 = 67,
	KEY_F11 = 68,
	KEY_F12 = 69,
	KEY_PRINTSCREEN = 70,
	KEY_SCROLLLOCK = 71,
	KEY_PAUSE = 72,
	KEY_INSERT = 73,
	KEY_HOME = 74,
	KEY_PAGEUP = 75,
	KEY_DELETE = 76,
	KEY_END = 77,
	KEY_PAGEDOWN = 78,
	KEY_RIGHT = 79,
	KEY_LEFT = 80,
	KEY_DOWN = 81,
	KEY_UP = 82,
	KEY_NUMLOCKCLEAR = 83,
	KEY_KP_DIVIDE = 84,
	KEY_KP_MULTIPLY = 85,
	KEY_KP_MINUS = 86,
	KEY_KP_PLUS = 87,
	KEY_KP_ENTER = 88,
	KEY_KP_1 = 89,
	KEY_KP_2 = 90,
	KEY_KP_3 = 91,
	KEY_KP_4 = 92,
	KEY_KP_5 = 93,
	KEY_KP_6 = 94,
	KEY_KP_7 = 95,
	KEY_KP_8 = 96,
	KEY_KP_9 = 97,
	KEY_KP_0 = 98,
	KEY_KP_PERIOD = 99,
	KEY_NONUSBACKSLASH = 100,
	KEY_APPLICATION = 101,
	KEY_POWER = 102,
	KEY_KP_EQUALS = 103,
	KEY_F13 = 104,
	KEY_F14 = 105,
	KEY_F15 = 106,
	KEY_F16 = 107,
	KEY_F17 = 108,
	KEY_F18 = 109,
	KEY_F19 = 110,
	KEY_F20 = 111,
	KEY_F21 = 112,
	KEY_F22 = 113,
	KEY_F23 = 114,
	KEY_F24 = 115,
	KEY_EXECUTE = 116,
	KEY_HELP = 117,
	KEY_MENU = 118,
	KEY_SELECT = 119,
	KEY_STOP = 120,
	KEY_AGAIN = 121,
	KEY_UNDO = 122,
	KEY_CUT = 123,
	KEY_COPY = 124,
	KEY_PASTE = 125,
	KEY_FIND = 126,
	KEY_MUTE = 127,
	KEY_VOLUMEUP = 128,
	KEY_VOLUMEDOWN = 129,
	KEY_KP_COMMA = 133,
	KEY_KP_EQUALSAS400 = 134,
	KEY_INTERNATIONAL1 = 135,
	KEY_INTERNATIONAL2 = 136,
	KEY_INTERNATIONAL3 = 137,
	KEY_INTERNATIONAL4 = 138,
	KEY_INTERNATIONAL5 = 139,
	KEY_INTERNATIONAL6 = 140,
	KEY_INTERNATIONAL7 = 141,
	KEY_INTERNATIONAL8 = 142,
	KEY_INTERNATIONAL9 = 143,
	KEY_LANG1 = 144,
	KEY_LANG2 = 145,
	KEY_LANG3 = 146,
	KEY_LANG4 = 147,
	KEY_LANG5 = 148,
	KEY_LANG6 = 149,
	KEY_LANG7 = 150,
	KEY_LANG8 = 151,
	KEY_LANG9 = 152,
	KEY_ALTERASE = 153,
	KEY_SYSREQ = 154,
	KEY_CANCEL = 155,
	KEY_CLEAR = 156,
	KEY_PRIOR = 157,
	KEY_RETURN2 = 158,
	KEY_SEPARATOR = 159,
	KEY_OUT = 160,
	KEY_OPER = 161,
	KEY_CLEARAGAIN = 162,
	KEY_CRSEL = 163,
	KEY_EXSEL = 164,
	KEY_KP_00 = 176,
	KEY_KP_000 = 177,
	KEY_THOUSANDSSEPARATOR = 178,
	KEY_DECIMALSEPARATOR = 179,
	KEY_CURRENCYUNIT = 180,
	KEY_CURRENCYSUBUNIT = 181,
	KEY_KP_LEFTPAREN = 182,
	KEY_KP_RIGHTPAREN = 183,
	KEY_KP_LEFTBRACE = 184,
	KEY_KP_RIGHTBRACE = 185,
	KEY_KP_TAB = 186,
	KEY_KP_BACKSPACE = 187,
	KEY_KP_A = 188,
	KEY_KP_B = 189,
	KEY_KP_C = 190,
	KEY_KP_D = 191,
	KEY_KP_E = 192,
	KEY_KP_F = 193,
	KEY_KP_XOR = 194,
	KEY_KP_POWER = 195,
	KEY_KP_PERCENT = 196,
	KEY_KP_LESS = 197,
	KEY_KP_GREATER = 198,
	KEY_KP_AMPERSAND = 199,
	KEY_KP_DBLAMPERSAND = 200,
	KEY_KP_VERTICALBAR = 201,
	KEY_KP_DBLVERTICALBAR = 202,
	KEY_KP_COLON = 203,
	KEY_KP_HASH = 204,
	KEY_KP_SPACE = 205,
	KEY_KP_AT = 206,
	KEY_KP_EXCLAM = 207,
	KEY_KP_MEMSTORE = 208,
	KEY_KP_MEMRECALL = 209,
	KEY_KP_MEMCLEAR = 210,
	KEY_KP_MEMADD = 211,
	KEY_KP_MEMSUBTRACT = 212,
	KEY_KP_MEMMULTIPLY = 213,
	KEY_KP_MEMDIVIDE = 214,
	KEY_KP_PLUSMINUS = 215,
	KEY_KP_CLEAR = 216,
	KEY_KP_CLEARENTRY = 217,
	KEY_KP_BINARY = 218,
	KEY_KP_OCTAL = 219,
	KEY_KP_DECIMAL = 220,
	KEY_KP_HEXADECIMAL = 221,
	KEY_LCTRL = 224,
	KEY_LSHIFT = 225,
	KEY_LALT = 226,
	KEY_LGUI = 227,
	KEY_RCTRL = 228,
	KEY_RSHIFT = 229,
	KEY_RALT = 230,
	KEY_RGUI = 231,
	KEY_MODE = 257
};

#endif

/*
 * functions
 */

int platform_init(int w, int h, const char *title);
void platform_quit();
int platform_running();
void platform_frame_start();
void platform_frame_end();
void platform_screen_clear(uint32_t c);
int platform_key(int sc);
void platform_draw_pixel(uint16_t x, uint16_t y, uint32_t c);
void platform_mouse(int *x, int *y, int *dx, int *dy);
void platform_error(const char *s, ...);
