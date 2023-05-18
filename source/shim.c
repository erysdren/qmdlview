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
 * todo:
 * - finish fenster support
 * - add some kind of shim for audio (SDL_audio, fenster_audio, DOS, etc)
 * - build up GLFW support
 */

/*
 *
 * headers
 *
 */

/* std */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/* shim */
#include "shim.h"

#if SHIM_FENSTER
#include "fenster.h"
typedef struct fenster fenster_t;
#endif

#if SHIM_DOS
#include "dos_helpers.h"
#endif

#if !SHIM_SDL2 && defined(_WIN32)
#include <windows.h>
#endif

/*
 *
 * macros
 *
 */

/* keyboard scancode array bounds */
#if SHIM_SDL2
#define NUM_SCANCODES SDL_NUM_SCANCODES
#elif SHIM_SDL
#define NUM_SCANCODES SDLK_LAST
#elif SHIM_GLFW
#define NUM_SCANCODES GLFW_KEY_LAST
#elif SHIM_FENSTER
#define NUM_SCANCODES 256
#else
#define NUM_SCANCODES 256
#endif

/* scratch buffer size */
#define SCRATCH_SIZE 256

/* unused */
#define UNUSED(x) (void)(x)

/*
 *
 * globals
 *
 */

/* shim structure */
struct
{
	/* platform specific */
	#if SHIM_SDL2
	SDL_Window *window;
	SDL_Texture *texture;
	SDL_Renderer *renderer;
	#elif SHIM_SDL
	SDL_Surface *window;
	SDL_Surface *texture;
	#elif SHIM_FENSTER
	fenster_t *fenster;
	#elif SHIM_DOS
	_go32_dpmi_seginfo kbhandler_old;
	_go32_dpmi_seginfo kbhandler_new;
	vesa_info_t vesa_info;
	vesa_mode_info_t vesa_mode_info;
	unsigned char key_last;
	int old_mode;
	int mode;
	#endif

	#if SHIM_SDL || SHIM_FENSTER
	uint32_t *pixels;
	#endif

	/* variables */
	int width;
	int height;
	int bpp;
	int running;

	struct
	{
		int x;		/* x pos */
		int y;		/* y pos */
		int dx;		/* delta x */
		int dy;		/* delta y */
		int b;		/* button mask */
	} mouse;

	/* keys */
	#if !SHIM_FENSTER
	char keys[NUM_SCANCODES];
	#endif

	/* scratch buffer */
	char scratch[SCRATCH_SIZE];

	/* string buffer */
	char *strbuf;
	size_t len_strbuf;

	/* temp string buffer */
	char tempstrbuf[SCRATCH_SIZE];
	size_t tempstrbuf_pos;

} shim;

/*
 *
 * functions
 *
 */

#if SHIM_DOS

/*
 * shim_dos_kbhandler
 */

void shim_dos_kbhandler()
{
	char key = (char)inp(0x60);
	shim.keys[shim.key_last = key & 0x7F] = !(key & 0x80);
	outp(0x20, 0x20);
}

#endif

/*
 * shim_init
 */

int shim_init(int w, int h, int bpp, const char *title)
{
	#if SHIM_SDL2
	/* variables */
	int pixelformat;
	#endif

	/* not allowed bpp */
	if (bpp != 8 && bpp != 16 && bpp != 32)
		return shim_error("invalid bpp %d", bpp);

	#if SHIM_DOS
	/* get old mode */
	shim.old_mode = dos_get_mode();

	/* suppress */
	UNUSED(title);

	/* get vesa info */
	dos_vesa_get_info(&shim.vesa_info);
	shim.mode = dos_vesa_find_mode(w, h, bpp);
	if (!shim.mode)
		return shim_error("couldn't get vesa mode");

	/* get vesa mode info */
	if (!dos_vesa_get_mode_info(&shim.vesa_mode_info, shim.mode))
		return shim_error("couldn't get vesa mode info");

	/* set vesa mode */
	dos_vesa_set_mode(shim.mode);

	/* enable mouse */
	dos_mouse_enable();
	dos_mouse_hide();

	/* setup keyboard handler */
	_go32_dpmi_get_protected_mode_interrupt_vector(9, &shim.kbhandler_old);
	shim.kbhandler_new.pm_offset = (int)shim_dos_kbhandler;
	shim.kbhandler_new.pm_selector = _go32_my_cs();
	_go32_dpmi_allocate_iret_wrapper(&shim.kbhandler_new);
	_go32_dpmi_set_protected_mode_interrupt_vector(9, &shim.kbhandler_new);

	/* start counting time */
	uclock();

	#endif

	#if SHIM_FENSTER

	/* only 32 bpp allowed for fenster, sorry */
	if (bpp != 32)
		return shim_error("invalid bpp %d", bpp);

	#endif

	#if SHIM_FENSTER || SHIM_SDL

	/* allocate pixels */
	shim.pixels = malloc(w * h * (bpp / 8));
	if (shim.pixels == NULL)
		return shim_error("failed to allocate pixels");

	#endif

	#if SHIM_FENSTER

	/* make fenster struct */
	fenster_t f = {
		.height = h,
		.width = w,
		.title = shim_string(title),
		.buf = shim.pixels
	};

	/* allocate persistent fenster struct */
	shim.fenster = malloc(sizeof(fenster_t));
	if (shim.fenster == NULL)
		return shim_error("failed to allocate fenster struct");

	/* copy local copy into it */
	memcpy(shim.fenster, &f, sizeof(fenster_t));

	/* open */
	fenster_open(shim.fenster);

	#endif

	#if SHIM_SDL2 || SHIM_SDL
	/* init everything */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		return shim_error("%s", SDL_GetError());
	#endif

	#if SHIM_SDL
	/* create window */
	shim.window = SDL_SetVideoMode(w, h, bpp, SDL_SWSURFACE | SDL_RESIZABLE);
	if (shim.window == NULL)
		return shim_error("%s", SDL_GetError());

	/* set window title */
	SDL_WM_SetCaption(title, NULL);

	/* create screen texture */
	shim.texture = SDL_CreateRGBSurfaceFrom(shim.pixels, w, h, bpp, w * (bpp / 8), 0, 0, 0, 0);
	if (shim.texture == NULL)
		return shim_error("%s", SDL_GetError());

	#endif

	#if SHIM_SDL2
	/* create window */
	shim.window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w,
		h,
		SDL_WINDOW_RESIZABLE
	);

	if (shim.window == NULL)
		return shim_error("%s", SDL_GetError());

	/* create screen renderer */
	shim.renderer = SDL_CreateRenderer(
		shim.window,
		-1,
		SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC
	);

	if (shim.renderer == NULL)
		return shim_error("%s", SDL_GetError());

	/* bpp */
	switch (bpp)
	{
		case 8:
			pixelformat = SDL_PIXELFORMAT_INDEX8;
			break;

		case 16:
			pixelformat = SDL_PIXELFORMAT_RGB565;
			break;

		case 32:
			pixelformat = SDL_PIXELFORMAT_ARGB8888;
			break;
	}

	/* create screen texture */
	shim.texture = SDL_CreateTexture(
		shim.renderer,
		pixelformat,
		SDL_TEXTUREACCESS_STREAMING,
		w,
		h
	);

	if (shim.texture == NULL)
		return shim_error("%s", SDL_GetError());

	#endif

	/* set values */
	shim.width = w;
	shim.height = h;
	shim.bpp = bpp;
	shim.running = SHIM_TRUE;
	shim.len_strbuf = 0;
	shim.strbuf = NULL;

	/* return success */
	return SHIM_TRUE;
}

/*
 * shim_quit
 */

void shim_quit()
{
	/* free string buffer */
	if (shim.strbuf) free(shim.strbuf);

	/* no longer running */
	shim.running = SHIM_FALSE;

	#if SHIM_DOS

	/* restore mouse and video mode */
	dos_mouse_hide();
	dos_set_mode(shim.old_mode);

	/* restore keyboard handler */
	_go32_dpmi_set_protected_mode_interrupt_vector(9, &shim.kbhandler_old);
	_go32_dpmi_free_iret_wrapper(&shim.kbhandler_new);

	#endif

	#if SHIM_FENSTER || SHIM_SDL
	/* free */
	if (shim.pixels) free(shim.pixels);
	#endif

	#if SHIM_FENSTER
	if (shim.fenster)
	{
		fenster_close(shim.fenster);
		free(shim.fenster);
	}
	#endif

	#if SHIM_SDL2
	/* free sdl memory */
	if (shim.texture != NULL) SDL_DestroyTexture(shim.texture);
	if (shim.renderer != NULL) SDL_DestroyRenderer(shim.renderer);
	if (shim.window != NULL) SDL_DestroyWindow(shim.window);
	#endif

	#if SHIM_SDL
	/* free screen surface */
	if (shim.texture != NULL) SDL_FreeSurface(shim.texture);
	#endif

	#if SHIM_SDL2 || SHIM_SDL
	/* sdl */
	SDL_Quit();
	#endif
}

#if SHIM_SDL2 || SHIM_SDL

#define ASPECT1 (float)(shim.width) / (float)(shim.height)
#define ASPECT2 (float)(shim.height) / (float)(shim.width)

/*
 * calc_screen_size
 */

static inline void calc_screen_size(int x, int y, SDL_Rect *rect)
{
	if (y < x && (y * ASPECT1) < x)
	{
		rect->w = y * ASPECT1;
		rect->h = y;
	}
	else if ((x / y) == ASPECT1)
	{
		rect->w = x;
		rect->h = y;
	}
	else
	{
		rect->w = x;
		rect->h = x * ASPECT2;
	}
}

/*
 * calc_screen_pos
 */

static inline void calc_screen_pos(int x, int y, SDL_Rect *rect)
{
	if (y < x && (y * ASPECT1) < x)
	{
		rect->x = (x / 2) - ((y * ASPECT1) / 2);
		rect->y = 0;
	}
	else if (x / y == ASPECT1)
	{
		rect->x = 0;
		rect->y = 0;
	}
	else
	{
		rect->x = 0;
		rect->y = (y / 2) - ((x * ASPECT2) / 2);
	}
}

#endif

/*
 * shim_blit
 */

void shim_blit(int w, int h, int bpp, void *pixels)
{
	#if SHIM_DOS

	dos_vesa_putb(&shim.vesa_mode_info, pixels, w * h * (bpp / 8));

	#elif SHIM_FENSTER

	/* copy */
	memcpy(shim.fenster->buf, pixels, w * h * (bpp / 8));

	#elif SHIM_SDL

	/* variables */
	SDL_Rect rect;

	/* copy */
	memcpy(shim.texture->pixels, pixels, w * h * (bpp / 8));

	/* calc pos */
	calc_screen_pos(shim.window->w, shim.window->h, &rect);
	calc_screen_size(shim.window->w, shim.window->h, &rect);

	/* blit */
	SDL_BlitSurface(shim.texture, NULL, shim.window, &rect);
	SDL_Flip(shim.window);

	#elif SHIM_SDL2

	int x, y;
	SDL_Rect rect;

	/* suppress warnings */
	UNUSED(h);

	SDL_GetWindowSize(shim.window, &x, &y);
	calc_screen_pos(x, y, &rect);
	calc_screen_size(x, y, &rect);

	SDL_UpdateTexture(shim.texture, NULL, pixels, w * (bpp / 8));
	SDL_RenderClear(shim.renderer);
	SDL_RenderCopy(shim.renderer, shim.texture, NULL, &rect);
	SDL_RenderPresent(shim.renderer);

	#endif
}

/*
 * shim_should_quit
 */

void shim_should_quit(int should_quit)
{
	shim.running = should_quit ? 0 : 1;
}

/*
 * shim_frame
 */

int shim_frame()
{
	#if SHIM_DOS

	/* variables */
	int16_t x, y, b;

	/* get */
	dos_mouse_get(&x, &y, &b);

	/* delta */
	shim.mouse.dx = x - shim.mouse.x;
	shim.mouse.dy = y - shim.mouse.y;

	/* pos and button */
	shim.mouse.x = x;
	shim.mouse.y = y;

	if (b & DOS_MOUSE_LMB)
		shim.mouse.b |= SHIM_MOUSE_LEFT;
	else
		shim.mouse.b &= ~SHIM_MOUSE_LEFT;

	if (b & DOS_MOUSE_RMB)
		shim.mouse.b |= SHIM_MOUSE_RIGHT;
	else
		shim.mouse.b &= ~SHIM_MOUSE_RIGHT;

	#elif SHIM_FENSTER

	shim.running = fenster_loop(shim.fenster) ? 0 : 1;

	#endif

	#if SHIM_SDL2 || SHIM_SDL
	/* variables */
	SDL_Event event;

	/* event poll loop */
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				shim.running = SHIM_FALSE;
				break;

			case SDL_MOUSEBUTTONDOWN:
			{
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						shim.mouse.b |= SHIM_MOUSE_LEFT;
						break;

					case SDL_BUTTON_RIGHT:
						shim.mouse.b |= SHIM_MOUSE_RIGHT;
						break;

					case SDL_BUTTON_MIDDLE:
						shim.mouse.b |= SHIM_MOUSE_MIDDLE;
						break;

					default:
						break;
				}
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						shim.mouse.b &= ~SHIM_MOUSE_LEFT;
						break;

					case SDL_BUTTON_RIGHT:
						shim.mouse.b &= ~SHIM_MOUSE_RIGHT;
						break;

					case SDL_BUTTON_MIDDLE:
						shim.mouse.b &= ~SHIM_MOUSE_MIDDLE;
						break;

					default:
						break;
				}
				break;

			}

			case SDL_MOUSEMOTION:
				shim.mouse.x = event.motion.x;
				shim.mouse.y = event.motion.y;
				shim.mouse.dx = event.motion.xrel;
				shim.mouse.dy = event.motion.yrel;
				break;

			#if SHIM_SDL

			case SDL_KEYDOWN:
				shim.keys[event.key.keysym.sym] = SHIM_TRUE;
				break;

			case SDL_KEYUP:
				shim.keys[event.key.keysym.sym] = SHIM_FALSE;
				break;

			#elif SHIM_SDL2

			case SDL_KEYDOWN:
				shim.keys[event.key.keysym.scancode] = SHIM_TRUE;
				break;

			case SDL_KEYUP:
				shim.keys[event.key.keysym.scancode] = SHIM_FALSE;
				break;

			#endif
		}
	}
	#endif

	/* return run status */
	return shim.running;
}

/*
 * shim_key_read
 */

int shim_key_read(int sc)
{
	#if SHIM_FENSTER
	return shim.fenster->keys[sc] ? SHIM_TRUE : SHIM_FALSE;
	#else
	return shim.keys[sc] ? SHIM_TRUE : SHIM_FALSE;
	#endif
}

/*
 * shim_mouse_read
 */

int shim_mouse_read(int *x, int *y, int *dx, int *dy)
{
	/* set ptrs */
	if (x) *x = shim.mouse.x;
	if (y) *y = shim.mouse.y;
	if (dx) *dx = shim.mouse.dx;
	if (dy) *dy = shim.mouse.dy;

	/* reset delta after each read? */
	shim.mouse.dx = 0;
	shim.mouse.dy = 0;

	/* return button mask */
	return shim.mouse.b;
}

/*
 * shim_error
 */

int shim_error(const char *s, ...)
{
	/* variables */
	va_list ap;

	/* shutdown everything */
	shim_quit();

	/* do vargs */
	va_start(ap, s);
	vsnprintf(shim.scratch, SCRATCH_SIZE, s, ap);
	va_end(ap);

	/* show message box error */
	#if SHIM_SDL2
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", shim.scratch, NULL);
	#elif defined(_WIN32)
	MessageBox(NULL, "Error", shim.scratch, MB_OK | MB_TASKMODAL);
	#endif

	/* print to stderr */
	fprintf(stderr, "%s\n", shim.scratch);

	/* return "error code" */
	return SHIM_FALSE;
}

/*
 * shim_string
 */

const char *shim_string(const char *s, ...)
{
	/* variables */
	va_list ap;
	char *ofs;
	int start;
	int end;

	/* do vargs */
	va_start(ap, s);
	vsnprintf(shim.scratch, SCRATCH_SIZE, s, ap);
	va_end(ap);

	/* start and end */
	start = shim.len_strbuf;
	end = shim.len_strbuf += (strlen(shim.scratch) + 1);

	/* realloc string buffer */
	shim.strbuf = realloc(shim.strbuf, end);

	/* assign return ptr */
	ofs = shim.strbuf + start;

	/* copy string */
	memcpy(ofs, shim.scratch, end - start);

	/* return pointer to the string we just made */
	return (const char *)ofs;
}

/*
 * shim_ticks
 */

uint64_t shim_ticks()
{
	#if SHIM_FENSTER
	return (uint64_t)fenster_time(shim.fenster);
	#elif SHIM_DOS
	return (uint64_t)uclock();
	#elif SHIM_SDL
	return (uint64_t)SDL_GetTicks();
	#elif SHIM_SDL2
	return (uint64_t)SDL_GetTicks64();
	#else
	return SHIM_FALSE;
	#endif
}

/*
 * shim_ticks_per_second
 */

uint64_t shim_ticks_per_second()
{
	/* these libraries always use ms (apparently) */
	#if SHIM_FENSTER || SHIM_SDL || SHIM_SDL2
	return 1000;
	#elif SHIM_DOS
	return UCLOCKS_PER_SEC;
	#else
	return SHIM_FALSE;
	#endif
}
