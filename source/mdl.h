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

//
// MDL types
//

// MDL magics
extern const char mdl_magic_quake[4];
extern const char mdl_magic_quake2[4];

// MDL version enumerations
enum mdl_versions
{
	MDL_VERSION_QTEST = 3,
	MDL_VERSION_QUAKE = 6,
	MDL_VERSION_QUAKE2 = 7,
};

// MDL version header
typedef struct
{
	uint8_t magic[4];
	uint32_t version;
} mdl_version_t;

// MDL header
typedef struct
{
	float scale[3];
	float translation[3];
	float bounding_radius;
	float eye_position[3];
	uint32_t num_skins;
	uint32_t skin_width;
	uint32_t skin_height;
	uint32_t num_vertices;
	uint32_t num_faces;
	uint32_t num_frames;
	uint32_t sync_type;
	uint32_t flags;
	float size;
} mdl_header_t;

// MDL skin
typedef struct
{
	uint32_t skin_type;
	void *skin_pixels;
} mdl_skin_t;

// MDL texcoord
typedef struct
{
	int32_t onseam;
	int32_t s;
	int32_t t;
} mdl_texcoord_t;

// MDL face
typedef struct
{
	uint32_t face_type;
	uint32_t vertex_indicies[3];
} mdl_face_t;

// MDL vertex
typedef struct
{
	uint8_t coordinates[3];
	uint8_t normal_index;
} mdl_vertex_t;

// MDL frame
typedef struct
{
	uint32_t frame_type;
	mdl_vertex_t min;
	mdl_vertex_t max;
	uint8_t name[16];
	mdl_vertex_t *vertices;
} mdl_frame_t;

// MDL container
typedef struct
{
	mdl_version_t *version;
	mdl_header_t *header;
	mdl_skin_t *skins;
	mdl_texcoord_t *texcoords;
	mdl_face_t *faces;
	mdl_frame_t *frames;
} mdl_t;

//
// MDL functions
//

// Load an id Software MDL file into memory. Returns a pointer to an MDL object.
mdl_t *MDL_Load(const char *filename);

// Free an id Software MDL object from memory.
void MDL_Free(mdl_t *mdl);
