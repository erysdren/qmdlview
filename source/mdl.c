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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* mdl */
#include "mdl.h"

/* platform */
#include "platform.h"

/* mdl magics */
const char mdl_magic_quake[4] = "IDPO";
const char mdl_magic_quake2[4] = "IDP2";

/* Load an id Software MDL file into memory. Returns a pointer to an MDL object. */
mdl_t *MDL_Load(const char *filename)
{
	/* variables */
	int i, num_pixels;
	mdl_t *mdl;
	FILE *file;

	/* open file */
	file = fopen(filename, "rb");

	/* allocate memory */
	mdl = calloc(1, sizeof(mdl_t));
	mdl->version = calloc(1, sizeof(mdl_version_t));
	mdl->header = calloc(1, sizeof(mdl_header_t));

	/* read in version header */
	fread(mdl->version, sizeof(mdl_version_t), 1, file);

	/* check file signature */
	if (!memcmp(mdl->version->magic, mdl_magic_quake2, 4) != 0)
	{
		platform_error("%s is a Quake 2 model file, which is currently not supported.", filename);
		return NULL;
	}

	/* check file signature */
	if (memcmp(mdl->version->magic, mdl_magic_quake, 4) != 0)
	{
		platform_error("%s has an unrecognized file signature %x.", filename, mdl->version->magic);
		return NULL;
	}

	/* check file version */
	if (mdl->version->version == MDL_VERSION_QTEST)
	{
		platform_error("%s is a QTest model file, which is currently not supported.", filename);
		return NULL;
	}
	else if (mdl->version->version == MDL_VERSION_QUAKE2)
	{
		platform_error("%s is a Quake 2 model file, which is currently not supported.", filename);
		return NULL;
	}
	else if (mdl->version->version != MDL_VERSION_QUAKE)
	{
		platform_error("%s has an unrecognized file version.", filename);
		return NULL;
	}

	/* read header  */
	fread(mdl->header, sizeof(mdl_header_t), 1, file);

	/* calculate number of pixels  */
	num_pixels = mdl->header->skin_width * mdl->header->skin_height;

	/* allocate more memory */
	mdl->skins = calloc(mdl->header->num_skins, sizeof(mdl_skin_t));
	mdl->texcoords = calloc(mdl->header->num_vertices, sizeof(mdl_texcoord_t));
	mdl->faces = calloc(mdl->header->num_faces, sizeof(mdl_face_t));
	mdl->frames = calloc(mdl->header->num_frames, sizeof(mdl_frame_t));

	/* read skins */
	for (i = 0; i < mdl->header->num_skins; i++)
	{
		fread(&mdl->skins[i].skin_type, sizeof(uint32_t), 1, file);

		if (mdl->skins[i].skin_type != 0)
		{
			fclose(file);
			platform_error("%s skin %d has an unsupported type.", i);
			return NULL;
		}

		mdl->skins[i].skin_pixels = calloc(num_pixels, sizeof(uint8_t));
		fread(mdl->skins[i].skin_pixels, sizeof(uint8_t), num_pixels, file);
	}

	/* read texcoords */
	fread(mdl->texcoords, sizeof(mdl_texcoord_t), mdl->header->num_vertices, file);

	/* read faces */
	fread(mdl->faces, sizeof(mdl_face_t), mdl->header->num_faces, file);

	/* read frames */
	for (i = 0; i < mdl->header->num_frames; i++)
	{
		fread(&mdl->frames[i].frame_type, sizeof(uint32_t), 1, file);
		fread(&mdl->frames[i].min, sizeof(mdl_vertex_t), 1, file);
		fread(&mdl->frames[i].max, sizeof(mdl_vertex_t), 1, file);
		fread(&mdl->frames[i].name, sizeof(uint8_t), 16, file);

		if (mdl->frames[i].frame_type != 0)
		{
			fclose(file);
			platform_error("%s frame %d has an unsupported type.", i);
			return NULL;
		}

		mdl->frames[i].vertices = calloc(mdl->header->num_vertices, sizeof(mdl_vertex_t));
		fread(mdl->frames[i].vertices, sizeof(mdl_vertex_t), mdl->header->num_vertices, file);
	}

	/* close file pointer */
	fclose(file);

	/* return pointer to mdl container */
	return mdl;
}

/* Free an id Software MDL object from memory. */
void MDL_Free(mdl_t *mdl)
{
	/* variables */
	int i, num_skins, num_frames;

	/* set values */
	num_skins = mdl->header->num_skins;
	num_frames = mdl->header->num_frames;

	/* free version header */
	free(mdl->version);

	/* free mdl header */
	free(mdl->header);

	/* free skin pixels */
	for (i = 0; i < num_skins; i++) free(mdl->skins[i].skin_pixels);

	/* free skins */
	free(mdl->skins);

	/* free texcoords */
	free(mdl->texcoords);

	/* free faces */
	free(mdl->faces);

	/* free frame vertices */
	for (i = 0; i < num_frames; i++) free(mdl->frames[i].vertices);

	/* free frames */
	free(mdl->frames);

	/* free model */
	free(mdl);
}
