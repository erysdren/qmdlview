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

/* drummyfish */
#define S3L_RESOLUTION_X 640
#define S3L_RESOLUTION_Y 480
#define S3L_PIXEL_FUNCTION S3L_Pixel
#define S3L_Z_BUFFER 1
#define S3L_PERSPECTIVE_CORRECTION 1
#define S3L_NEAR_CROSS_STRATEGY 3
#include "thirdparty/small3dlib.h"

/* platform */
#ifdef QMDLVIEW_DOS
#define PLATFORM_DOS 1
#else
#define PLATFORM_SDL2 1
#endif
#include "platform.h"

/* bleck */
#ifndef QMDLVIEW_DOS
#define KEY_PLUS KEY_KP_PLUS
#define KEY_MINUS KEY_KP_MINUS
#endif

/* quake palette */
#include "palette.h"

/* mdl loader */
#include "mdl.h"

/* font8x8 */
#include "thirdparty/font8x8_basic.h"

/* dos helpers */
#ifdef QMDLVIEW_DOS
#include "dos_helpers.h"
#endif

/* tinyfiledialogs */
#include "thirdparty/tinyfiledialogs.h"

/*
 *
 * macros
 *
 */

#ifdef QMDLVIEW_DOS
#define BPP 8
#else
#define BPP 32
#endif

#define MDL_MAGIC 0x4f504449

#define PALETTE_RGB(i) ((rgb24_t *)(&palette[(i) * 3]))

#define ARGB(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define RGBA(r, g, b, a) (((r) << 24) | ((g) << 16) | ((b) << 8) | (a))
#define RGB(r, g, b) ARGB((r), (g), (b), 255)

#define UNIT(i) (S3L_Unit)((i) * S3L_F)

#if BPP == 8
#define PALETTE(i) (i)
#elif BPP == 32
#define PALETTE(i) RGB(PALETTE_RGB(i)->r, PALETTE_RGB(i)->g, PALETTE_RGB(i)->b)
#endif

/*
 *
 * types
 *
 */

typedef struct
{
	uint8_t r, g, b;
} rgb24_t;

typedef S3L_Model3D model_t;

/*
 *
 * globals
 *
 */

mdl_t *mdl;

S3L_Unit *s3l_vertices;
S3L_Index *s3l_triangles;
S3L_Unit *s3l_uvs;
S3L_Index *s3l_uv_indices;
S3L_Scene s3l_scene;
S3L_Model3D s3l_model;
model_t *models[256];

int frame_num;

int wireframe;

const char *mdl_patterns[1] = {"*.mdl"};
const char *open_filename;

/*
 *
 * functions
 *
 */

/*
 * S3L_Pixel
 */

static inline void S3L_Pixel(S3L_PixelInfo *pixel)
{
	/* variables */
	S3L_Vec4 uv0;
	S3L_Vec4 uv1;
	S3L_Vec4 uv2;
	S3L_Unit uv[2];
	uint8_t *pixels;
	int pos;

	/* draw background color */
	platform_draw_pixel(pixel->x, pixel->y, PALETTE(0));

	/* get vertex uvs */
	S3L_getIndexedTriangleValues(pixel->triangleIndex, s3l_uv_indices, s3l_uvs, 2, &uv0, &uv1, &uv2);

	/* interpolate barycentric coords */
	uv[0] = S3L_interpolateBarycentric(uv0.x, uv1.x, uv2.x, pixel->barycentric);
	uv[1] = S3L_interpolateBarycentric(uv0.y, uv1.y, uv2.y, pixel->barycentric);

	/* wrap coordinates */
	uv[0] = S3L_wrap(uv[0], mdl->header->skin_width);
	uv[1] = S3L_wrap(uv[1], mdl->header->skin_height);

	/* draw texture */
	pixels = (uint8_t *)mdl->skins[0].skin_pixels;
	pos = (uv[1] * mdl->header->skin_width) + uv[0];
	platform_draw_pixel(pixel->x, pixel->y, PALETTE(pixels[pos]));

	/* draw wireframe */
	if (wireframe && (pixel->barycentric[0] == 0 || pixel->barycentric[1] == 0 || pixel->barycentric[2] == 0))
		platform_draw_pixel(pixel->x, pixel->y, PALETTE(254));
}

/*
 * model_alloc
 */

model_t *model_alloc(int num_vertices, int num_triangles)
{
	/* variables */
	model_t *model;
	S3L_Unit *vertices;
	S3L_Index *triangles;

	/* alloc model */
	model = malloc(sizeof(model_t));
	if (model == NULL) return NULL;

	/* alloc elements */
	vertices = malloc(sizeof(S3L_Unit) * num_vertices * 3);
	triangles = malloc(sizeof(S3L_Index) * num_triangles * 3);
	if (vertices == NULL || triangles == NULL)
		return NULL;

	/* init model */
	S3L_model3DInit(vertices, num_vertices, triangles, num_triangles, model);

	/* return ptr */
	return model;
}

void model_free(model_t *model)
{
	if (model)
	{
		if (model->vertices) free((void *)model->vertices);
		if (model->triangles) free((void *)model->triangles);
		free(model);
	}
}

/*
 * draw_font8x8
 */

void draw_font8x8(int x, int y, uint32_t c, uint8_t *bitmap)
{
	/* variables */
	int xx, yy;

	/* plot loop */
	for (yy = 0; yy < 8; yy++)
	{
		for (xx = 0; xx < 8; xx++)
		{
			if (x + xx > S3L_RESOLUTION_X - 1 || y + yy > S3L_RESOLUTION_Y - 1) return;

			if (bitmap[yy] & 1 << xx)
				platform_draw_pixel(x + xx, y + yy, c);
		}
	}
}

/*
 * draw_text
 */

void draw_text(int x, int y, uint32_t c, const char *fmt, ...)
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

void qmdlview_init()
{
	/* init platform */
	if (!platform_init(S3L_RESOLUTION_X, S3L_RESOLUTION_Y, BPP, "qmdlview"))
		platform_error("could not init platform!\n");

	/* set palette */
	#ifdef QMDLVIEW_DOS

	for (int i = 0; i < 256; i++)
	{
		dos_set_palette_color(i, PALETTE_RGB(i)->r, PALETTE_RGB(i)->g, PALETTE_RGB(i)->b);
	}

	#endif
}

/*
 * qmdlview_deinit
 */

void qmdlview_deinit()
{
	int i;

	/* free memory */
	for (i = 0; i < mdl->header->num_frames; i++)
	{
		model_free(models[i]);
	}

	if (mdl) MDL_Free(mdl);
	if (s3l_vertices) free(s3l_vertices);
	if (s3l_triangles) free(s3l_triangles);
	if (s3l_uv_indices) free(s3l_uv_indices);
	if (s3l_uvs) free(s3l_uvs);


	/* close platform */
	platform_quit();
}

/*
 * qmdlview_open
 */

void qmdlview_open(const char *filename)
{
	/* variables */
	int i, v, t;
	S3L_Vec4 origin;

	/* load model */
	mdl = MDL_Load(filename);
	if (mdl == NULL) platform_error("couldn't load %s", filename);

	/* make a model for each frame */
	for (i = 0; i < mdl->header->num_frames; i++)
	{
		/* variables */
		S3L_Unit *vertices;
		S3L_Index *triangles;

		/* alloc model */
		models[i] = model_alloc(mdl->header->num_vertices, mdl->header->num_faces);

		/* assign ptrs */
		vertices = (S3L_Unit *)models[i]->vertices;
		triangles = (S3L_Index *)models[i]->triangles;

		/* calculate vertices */
		for (v = 0; v < mdl->header->num_vertices; v++)
		{
			vertices[v * 3] = UNIT(((float)mdl->frames[i].vertices[v].coordinates[0] * mdl->header->scale[0]) + mdl->header->translation[0]);
			vertices[(v * 3) + 1] = UNIT(((float)mdl->frames[i].vertices[v].coordinates[1] * mdl->header->scale[1]) + mdl->header->translation[1]);
			vertices[(v * 3) + 2] = UNIT(((float)mdl->frames[i].vertices[v].coordinates[2] * mdl->header->scale[2]) + mdl->header->translation[2]);
		}

		/* calculate triangles */
		for (t = 0; t < mdl->header->num_faces; t++)
		{
			triangles[t * 3] = mdl->faces[t].vertex_indicies[0];
			triangles[(t * 3) + 1] = mdl->faces[t].vertex_indicies[1];
			triangles[(t * 3) + 2] = mdl->faces[t].vertex_indicies[2];
		}

		/* fix transform */
		models[i]->transform.rotation.x += 128;
	}

	/* allocate s3l uvs */
	s3l_uvs = calloc(mdl->header->num_vertices, sizeof(S3L_Unit) * 2);
	if (s3l_uvs == NULL) platform_error("couldn't allocate uvs");

	/* claculate uvs */
	for (i = 0; i < mdl->header->num_vertices; i++)
	{
		s3l_uvs[i * 2] = mdl->texcoords[i].s;
		s3l_uvs[(i * 2) + 1] = mdl->texcoords[i].t;
	}

	/* allocate s3l uv indices */
	s3l_uv_indices = calloc(mdl->header->num_faces, sizeof(S3L_Index) * 3);
	if (s3l_uv_indices == NULL) platform_error("couldn't allocate uv indices");

	/* calculate uv indices */
	for (i = 0; i < mdl->header->num_faces; i++)
	{
		s3l_uv_indices[i * 3] = mdl->faces[i].vertex_indicies[0];
		s3l_uv_indices[(i * 3) + 1] = mdl->faces[i].vertex_indicies[1];
		s3l_uv_indices[(i * 3) + 2] = mdl->faces[i].vertex_indicies[2];
	}

	/* init s3l scene */
	S3L_sceneInit(models[0], 1, &s3l_scene);

	s3l_scene.camera.transform.translation.x += S3L_F * 128;
	s3l_scene.camera.transform.translation.z += S3L_F * 128;

	/* look at model */
	S3L_vec4Init(&origin);
	S3L_lookAt(origin, &s3l_scene.camera.transform);
}

/*
 * main
 */

int main(int argc, char **argv)
{
	/* variables */
	S3L_Vec4 v_forward, v_up, v_right;

	/* init vectors */
	S3L_vec4Init(&v_forward);
	S3L_vec4Init(&v_up);
	S3L_vec4Init(&v_right);

	/* open model from command line */
	if (argc > 1)
	{
		qmdlview_init();
		qmdlview_open(argv[1]);
	}
	else
	{
		#ifdef QMDLVIEW_DOS

		tinyfd_forceConsole = 1;

		#endif

		open_filename = tinyfd_openFileDialog("Choose an MDL File", "", 1, mdl_patterns, "Quake MDL Files", 0);

		if (open_filename == NULL)
			platform_error("open_filename is NULL!");

		qmdlview_init();
		qmdlview_open(open_filename);
	}

	/* main loop */
	while (platform_running())
	{
		/* variables */
		int dx, dy;

		/* frame start */
		platform_frame_start();

		/* get movedir */
		S3L_rotationToDirections(s3l_scene.camera.transform.rotation, S3L_F, &v_forward, &v_right, &v_up);

		/* process inputs */
		if (platform_key(KEY_W)) S3L_vec3Add(&s3l_scene.camera.transform.translation, v_forward);
		if (platform_key(KEY_A)) S3L_vec3Sub(&s3l_scene.camera.transform.translation, v_right);
		if (platform_key(KEY_S)) S3L_vec3Sub(&s3l_scene.camera.transform.translation, v_forward);
		if (platform_key(KEY_D)) S3L_vec3Add(&s3l_scene.camera.transform.translation, v_right);

		if (platform_key(KEY_Q)) s3l_scene.camera.transform.translation.y += S3L_F * 2;
		if (platform_key(KEY_E)) s3l_scene.camera.transform.translation.y -= S3L_F * 2;

		if (platform_key(KEY_UP)) s3l_scene.camera.transform.rotation.x += 4;
		if (platform_key(KEY_DOWN)) s3l_scene.camera.transform.rotation.x -= 4;
		if (platform_key(KEY_LEFT)) s3l_scene.camera.transform.rotation.y += 4;
		if (platform_key(KEY_RIGHT)) s3l_scene.camera.transform.rotation.y -= 4;

		if (platform_key(KEY_TAB)) wireframe = wireframe ? 0 : 1;

		if (platform_key(KEY_ESCAPE)) break;

		/* process mouse */
		if (platform_mouse(NULL, NULL, &dx, &dy))
		{
			platform_mouse_capture();
			s3l_scene.camera.transform.rotation.y -= dx;
			s3l_scene.camera.transform.rotation.x -= dy;
		}
		else
		{
			platform_mouse_release();
		}

		/* clear screen */
		platform_screen_clear(PALETTE(255));

		/* frame num */
		if (platform_key(KEY_PLUS)) frame_num++;
		if (platform_key(KEY_MINUS)) frame_num--;
		if (frame_num < 0) frame_num = 0;
		if (frame_num >= mdl->header->num_frames) frame_num = mdl->header->num_frames - 1;
		s3l_scene.models = models[frame_num];

		/* draw model */
		S3L_newFrame();
		S3L_drawScene(s3l_scene);

		/* draw text */
		draw_text(2, 2, PALETTE(254), "WASD: move\nARROW KEYS: look\nTAB: wireframe\nESCAPE: quit\nMOUSE: click\nFRAME: %d / %d", frame_num + 1, mdl->header->num_frames);

		/* frame end */
		platform_frame_end();
	}

	/* deinit */
	qmdlview_deinit();

	/* return success */
	return 0;
}
