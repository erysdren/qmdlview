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
 * headers
 */

/* std */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

/* dos helpers */
#include "dos_helpers.h"

/* platform */
#define PLATFORM_DOS 1
#include "platform.h"

/*
 * globals
 */

/* platform context */
struct
{

	/* variables */
	uint8_t running;
	uint8_t *pixels;
	int width;
	int height;
	int bpp;
	int old_mode;
	int mode;

	struct
	{
		int x;		/* x pos */
		int y;		/* y pos */
		int dx;		/* delta x */
		int dy;		/* delta y */
		int b;		/* button mask */
	} mouse;

	/* keys */
	char keys[256];
	char key_last;

	_go32_dpmi_seginfo kbhandler_old;
	_go32_dpmi_seginfo kbhandler_new;

	/* vesa */
	vesa_info_t vesa_info;
	vesa_mode_info_t vesa_mode_info;

} context;

/*
 * kbhandler
 */

void kbhandler()
{
	char key = (char)inp(0x60);
	context.keys[context.key_last = key & 0x7F] = !(key & 0x80);
	outp(0x20, 0x20);
}

/*
 * platform_init
 */

int platform_init(int w, int h, int bpp, const char *title)
{
	/* get current video mode */
	context.old_mode = dos_get_mode();

	/* suppress warnings */
	(void)title;

	/* get vesa info  */
	dos_vesa_get_info(&context.vesa_info);
	context.mode = dos_vesa_find_mode(w, h, bpp);
	if (!context.mode)
		platform_error("couldn't get vesa mode");

	/* get vesa mode info */
	if (!dos_vesa_get_mode_info(&context.vesa_mode_info, context.mode))
		platform_error("couldn't get vesa mode info");

	/* set vesa mode */
	dos_vesa_set_mode(context.mode);

	/* enable mouse */
	dos_mouse_enable();
	dos_mouse_hide();

	/* set values */
	context.width = w;
	context.height = h;
	context.bpp = bpp;
	context.running = 1;

	/* alloc pixels */
	context.pixels = malloc(context.width * context.height * (bpp / 8));
	if (context.pixels == NULL)
		platform_error("failed malloc");

	/* setup keyboard handler */
	_go32_dpmi_get_protected_mode_interrupt_vector(9, &context.kbhandler_old);
	context.kbhandler_new.pm_offset = (int)kbhandler;
	context.kbhandler_new.pm_selector = _go32_my_cs();
	_go32_dpmi_allocate_iret_wrapper(&context.kbhandler_new);
	_go32_dpmi_set_protected_mode_interrupt_vector(9, &context.kbhandler_new);

	/* return success */
	return 1;
}

/*
 * platform_quit
 */

void platform_quit()
{
	if (context.pixels) free(context.pixels);
	dos_mouse_hide();
	dos_set_mode(context.old_mode);

	/* restore keyboard handler */
	_go32_dpmi_set_protected_mode_interrupt_vector(9, &context.kbhandler_old);
	_go32_dpmi_free_iret_wrapper(&context.kbhandler_new);
}

/*
 * platform_frame_start
 */

void platform_frame_start()
{
	/* variables */
	int16_t x, y, b;

	/* get */
	dos_mouse_get(&x, &y, &b);

	/* delta */
	context.mouse.dx = x - context.mouse.x;
	context.mouse.dy = y - context.mouse.y;

	/* pos and button */
	context.mouse.x = x;
	context.mouse.y = y;
	context.mouse.b = b;
}

/*
 * platform_frame_end
 */

void platform_frame_end()
{
	dos_vesa_putb(&context.vesa_mode_info, context.pixels, context.width * context.height * (context.bpp / 8));
}

/*
 * platform_frame
 */

int platform_frame()
{
	/* run end of last frame */
	platform_frame_end();

	/* run start of this frame */
	platform_frame_start();

	/* return run status */
	return context.running;
}

/*
 * platform_screen_clear
 */

void platform_screen_clear(uint32_t c)
{
	int i;

	for (i = 0; i < context.width * context.height * (context.bpp / 8); i++)
		context.pixels[i] = c;
}

/*
 * platform_key
 */

int platform_key(int sc)
{
	return context.keys[sc];
}

/*
 * platform_draw_pixel
 */

void platform_draw_pixel(uint16_t x, uint16_t y, uint32_t c)
{
	switch (context.bpp)
	{
		case 8:
			((uint8_t *)context.pixels)[(y * context.width) + x] = c;
			break;

		case 16:
			((uint16_t *)context.pixels)[(y * context.width) + x] = c;
			break;

		case 32:
			((uint32_t *)context.pixels)[(y * context.width) + x] = c;
			break;

		default:
			break;
	}
}

/*
 * platform_mouse
 */

int platform_mouse(int *x, int *y, int *dx, int *dy)
{
	/* set ptrs */
	if (x) *x = context.mouse.x;
	if (y) *y = context.mouse.y;
	if (dx) *dx = context.mouse.dx;
	if (dy) *dy = context.mouse.dy;

	/* reset delta after each read? */
	context.mouse.dx = 0;
	context.mouse.dy = 0;

	return context.mouse.b;
}

/*
 * platform_error
 */

void platform_error(const char *s, ...)
{
	va_list ap;

	platform_quit();

	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);

	exit(1);
}

/*
 * platform_mouse_capture
 */

void platform_mouse_capture()
{

}

/*
 * platform_mouse_release
 */

void platform_mouse_release()
{

}

/*
 * platform set test
 */

#ifdef PLATFORM_SELF_TEST

#define ARGB(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

int main(int argc, char **argv)
{
	/* init platform */
	platform_init(640, 480, 32, "hello");

	while (platform_running())
	{
		platform_frame_start();

		if (platform_key(KEY_ESCAPE)) break;

		platform_screen_clear(UINT32_MAX);

		platform_frame_end();
	}

	/* quit */
	platform_quit();

	/* return success */
	return 0;
}

#endif
