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
#include <stdarg.h>
#include <string.h>
#include <math.h>

/* gl */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/ostinygl.h>

/* shim */
#include "shim.h"

/* quake palette */
#include "palette.h"

/* utilities */
#include "rgb.h"

/* mdl loader */
#include "mdl.h"

/* font8x8 */
#include "thirdparty/font8x8_basic.h"

/* tinyfiledialogs */
#include "thirdparty/tinyfiledialogs.h"

/*
 *
 * types
 *
 */

/* vec3 */
typedef struct
{
	float x, y, z;
} vec3_t;

/*
 *
 * macros
 *
 */

/* screen settings */
#define WIDTH 640
#define HEIGHT 480
#define BPP 16

/* aspect ratios */
#define ASPECT_WH (float)gl_context->width / (float)gl_context->height
#define ASPECT_HW (float)gl_context->height / (float)gl_context->width

/* camera settings */
#define SPEED 2
#define FOV 90

/* pi */
#ifndef M_PI
#define M_PI 3.14159265
#endif
#ifndef M_PI_2
#define M_PI_2 M_PI / 2
#endif

/* get palette entry as RGB565 short */
#define PALETTE_RGB565(x) RGB565(PALETTE_RGB24(x, palette)->r, \
	PALETTE_RGB24(x, palette)->g, PALETTE_RGB24(x, palette)->b)

/* get palette entry as 3 floats, plus fixed alpha */
#define PALETTE_RGBAFLOAT(x) PALETTE_RGB24(x, palette)->r / 255.0f, \
	PALETTE_RGB24(x, palette)->g / 255.0f, PALETTE_RGB24(x, palette)->b / 255.0f, 1.0f

/*
 *
 * globals
 *
 */

/* gl */
ostgl_context *gl_context;
GLint *gl_models;
GLuint gl_texture;
void *gl_texture_pixels;

/* mdl */
mdl_t *mdl;

/* render settings */
int frame_num;
int wireframe;

/* tinyfd */
const char *mdl_patterns[1] = {"*.mdl"};
const char *open_filename = NULL;
size_t open_filename_len = 0;

/* camera */
struct
{
	vec3_t position;
	vec3_t rotation;
	vec3_t look;
	vec3_t strafe;
	vec3_t velocity;
	vec3_t speedkey;
} camera;

/*
 *
 * functions
 *
 */

/*
 * draw_font8x8
 */

void draw_font8x8(int x, int y, uint16_t c, uint8_t *bitmap)
{
	/* variables */
	int xx, yy;

	/* plot loop */
	for (yy = 0; yy < 8; yy++)
	{
		for (xx = 0; xx < 8; xx++)
		{
			if (x + xx > gl_context->width - 1 || y + yy > gl_context->height - 1) return;

			if (bitmap[yy] & 1 << xx)
				((uint16_t *)gl_context->pixels)[((y + yy) * gl_context->width) + (x + xx)] = c;
		}
	}
}

/*
 * draw_text
 */

void draw_text(int x, int y, uint16_t c, const char *fmt, ...)
{
	/* variables */
	int i, p, n;
	va_list va;
	char scratch[128];

	/* process vargs */
	va_start(va, fmt);
	vsnprintf(scratch, sizeof(scratch), fmt, va);
	va_end(va);

	/* plot loop */
	p = 0;
	for (i = 0; i < strlen(scratch); i++)
	{
		/* check for newlines */
		if (scratch[i] == '\n')
		{
			y += 8;
			x -= (p + 1) * 8;
			p = 0;
			continue;
		}

		p++;

		n = scratch[i];
		draw_font8x8(x + (i * 8), y, c, font8x8_basic[n]);
	}
}

/*
 * qmdlview_init
 */

int qmdlview_init()
{
	/* gl */
	gl_context = ostgl_create_context(WIDTH, HEIGHT, BPP);
	if (gl_context == NULL) return shim_error("couldn't allocate gl context");

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);

	/* init shim */
	if (!shim_init(gl_context->width, gl_context->height, gl_context->depth, "qmdlview"))
		return shim_error("couldn't init shim!");

	/* return success */
	return SHIM_TRUE;
}

/*
 * qmdlview_deinit
 */

void qmdlview_deinit()
{
	/* free */
	if (gl_context) ostgl_delete_context(gl_context);
	if (gl_texture_pixels) free(gl_texture_pixels);
	if (gl_models) free(gl_models);
	if (mdl) MDL_Free(mdl);

	/* close shim */
	shim_quit();
}

/*
 * qmdlview_open
 */

int qmdlview_open(const char *filename)
{
	/* variables */
	int i, f, v;

	/* set */
	frame_num = 0;

	/* load model */
	mdl = MDL_Load(filename);
	if (mdl == NULL)
		return shim_error("couldn't load %s", filename);

	/* allocate list */
	gl_models = (GLint *)malloc(sizeof(GLint) * mdl->header->num_frames);
	if (gl_models == NULL)
		return shim_error("failed malloc in qmdlview_open");

	/* create texture pixels */
	gl_texture_pixels = malloc(mdl->header->skin_width * mdl->header->skin_height * 3);

	for (i = 0; i < mdl->header->skin_width * mdl->header->skin_height; i++)
	{
		((uint8_t *)gl_texture_pixels)[i * 3] = palette[((uint8_t *)mdl->skins[0].skin_pixels)[i] * 3];
		((uint8_t *)gl_texture_pixels)[(i * 3) + 1] = palette[(((uint8_t *)mdl->skins[0].skin_pixels)[i] * 3) + 1];
		((uint8_t *)gl_texture_pixels)[(i * 3) + 2] = palette[(((uint8_t *)mdl->skins[0].skin_pixels)[i] * 3) + 2];
	}

	/* generate texture */
	glGenTextures(1, &gl_texture);
	glBindTexture(GL_TEXTURE_2D, gl_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, mdl->header->skin_width, mdl->header->skin_height, 0, GL_RGB, GL_UNSIGNED_BYTE, gl_texture_pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/* create lists */
	for (i = 0; i < mdl->header->num_frames; i++)
	{
		/* new list */
		gl_models[i] = glGenLists(1);
		glNewList(gl_models[i], GL_COMPILE);

		/* begin triangles */
		glBindTexture(GL_TEXTURE_2D, gl_texture);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLES);

		/* do vertices */
		for (f = 0; f < mdl->header->num_faces; f++)
		{
			for (v = 0; v < 3; v++)
			{
				GLfloat x, y, z, s, t;

				x = (mdl->frames[i].vertices[mdl->faces[f].vertex_indicies[v]].coordinates[0] * mdl->header->scale[0]) + mdl->header->translation[0];
				y = (mdl->frames[i].vertices[mdl->faces[f].vertex_indicies[v]].coordinates[1] * mdl->header->scale[1]) + mdl->header->translation[1];
				z = (mdl->frames[i].vertices[mdl->faces[f].vertex_indicies[v]].coordinates[2] * mdl->header->scale[2]) + mdl->header->translation[2];

				s = (float)mdl->texcoords[mdl->faces[f].vertex_indicies[v]].s;
				t = (float)mdl->texcoords[mdl->faces[f].vertex_indicies[v]].t;

				/* handle onseam */
				if (mdl->faces[f].face_type == 0 && mdl->texcoords[mdl->faces[f].vertex_indicies[v]].onseam)
					s += (float)mdl->header->skin_width / 2.0f;

				s /= (float)mdl->header->skin_width;
				t /= (float)mdl->header->skin_height;

				glTexCoord2f(s, t);
				glVertex3f(x, y, z);
			}
		}

		/* end triangles */
		glEnd();
		glDisable(GL_TEXTURE_2D);

		/* end list */
		glEndList();
	}

	/* return success */
	return SHIM_TRUE;
}

/*
 * qmdlview_camera
 */

void qmdlview_camera()
{
	/*
	 * process inputs
	 */

	/* speed */
	if (shim_key_read(SHIM_KEY_LSHIFT))
	{
		camera.speedkey.x = 2;
		camera.speedkey.y = 2;
		camera.speedkey.z = 2;
	}
	else
	{
		camera.speedkey.x = 1;
		camera.speedkey.y = 1;
		camera.speedkey.z = 1;
	}

	/* forwards */
	if (shim_key_read(SHIM_KEY_W))
	{
		camera.position.x += camera.look.x * SPEED * camera.speedkey.x;
		camera.position.y += camera.look.y * SPEED * camera.speedkey.y;
		camera.position.z += camera.look.z * SPEED * camera.speedkey.z;
	}

	/* backwards */
	if (shim_key_read(SHIM_KEY_S))
	{
		camera.position.x -= camera.look.x * SPEED * camera.speedkey.x;
		camera.position.y -= camera.look.y * SPEED * camera.speedkey.y;
		camera.position.z -= camera.look.z * SPEED * camera.speedkey.z;
	}

	/* left */
	if (shim_key_read(SHIM_KEY_A))
	{
		camera.position.x += camera.strafe.x * SPEED * camera.speedkey.x;
		camera.position.z += camera.strafe.z * SPEED * camera.speedkey.z;
	}

	/* right */
	if (shim_key_read(SHIM_KEY_D))
	{
		camera.position.x -= camera.strafe.x * SPEED * camera.speedkey.x;
		camera.position.z -= camera.strafe.z * SPEED * camera.speedkey.z;
	}

	/* arrow keys */
	if (shim_key_read(SHIM_KEY_UP)) camera.rotation.x += 0.1f;
	if (shim_key_read(SHIM_KEY_DOWN)) camera.rotation.x -= 0.1f;
	if (shim_key_read(SHIM_KEY_LEFT)) camera.rotation.y -= 0.1f;
	if (shim_key_read(SHIM_KEY_RIGHT)) camera.rotation.y += 0.1f;

	/*
	 * gl
	 */

	/* set viewport */
	glViewport(0, 0, (GLint)gl_context->width, (GLint)gl_context->height);

	/* set perspective */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV * ASPECT_HW, ASPECT_WH, 2.0f, 512.0f);

	/* set camera look/strafe values */
	camera.look.x = cosf(camera.rotation.y) * cosf(camera.rotation.x);
	camera.look.y = sinf(camera.rotation.x);
	camera.look.z = sinf(camera.rotation.y) * cosf(camera.rotation.x);
	camera.strafe.x = cosf(camera.rotation.y - M_PI_2);
	camera.strafe.z = sinf(camera.rotation.y - M_PI_2);

	/* set camera view */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.position.x + camera.look.x, camera.position.y + camera.look.y, camera.position.z + camera.look.z, 0.0f, 1.0f, 0.0);
}

/*
 * main
 */

int main(int argc, char **argv)
{
	/* open model from command line */
	if (argc > 1)
	{
		qmdlview_init();
		qmdlview_open(argv[1]);
	}
	else
	{
		#ifdef __HAIKU__

		FILE *program = popen("filepanel -l -1 -t \"Choose an MDL File\"", "r");
		getline(&open_filename, &open_filename_len, program);
		pclose(program);

		#else

		open_filename = tinyfd_openFileDialog("Choose an MDL File", "", 1, mdl_patterns, "Quake MDL Files", 0);

		#endif

		if (open_filename == NULL)
		{
			shim_error("open_filename is NULL!");
			return 1;
		}

		qmdlview_init();
		qmdlview_open(open_filename);
	}

	/* main loop */
	while (shim_frame())
	{
		/* process inputs */
		if (shim_key_read(SHIM_KEY_ESCAPE))
			shim_should_quit(SHIM_TRUE);

		if (shim_key_read(SHIM_KEY_TAB))
			wireframe = wireframe ? SHIM_FALSE : SHIM_TRUE;

		if (shim_key_read(SHIM_KEY_PLUS)) frame_num++;
		if (shim_key_read(SHIM_KEY_MINUS)) frame_num--;
		if (frame_num < 0) frame_num = 0;
		if (frame_num >= mdl->header->num_frames) frame_num = mdl->header->num_frames - 1;

		/* process camera */
		qmdlview_camera();

		/* clear screen */
		glClearColor(PALETTE_RGBAFLOAT(255));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* save matrix */
		glPushMatrix();

		/* draw model */
		glRotatef(-90, 1, 0, 0);
		glPolygonMode(GL_FRONT, GL_FILL);
		glCallList(gl_models[frame_num]);

		/* draw wireframe */
		if (wireframe)
		{
			GLfloat white[4] = {1.0, 1.0, 1.0, 1.0};
			glPolygonMode(GL_FRONT, GL_LINE);
			glMaterialfv(GL_FRONT, GL_EMISSION, white);
			glPolygonOffset(0, -1);
			glCallList(gl_models[frame_num]);
		}

		/* restore matrix */
		glPopMatrix();

		/* draw text */
		draw_text(2, 2, PALETTE_RGB565(254), "WASD: move\nARROW KEYS: look\nTAB: wireframe\nESCAPE: quit\nMOUSE: click\nFRAME: %d / %d", frame_num + 1, mdl->header->num_frames);

		/* blit to screen */
		shim_blit(gl_context->width, gl_context->height, gl_context->depth, gl_context->pixels);
	}

	/* deinit */
	qmdlview_deinit();

	/* return success */
	return 0;
}
