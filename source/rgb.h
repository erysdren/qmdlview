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

/* guards */
#pragma once
#ifndef __RGB_H__
#define __RGB_H__
#ifdef __cplusplus
extern "C" {
#endif

/*
 * headers
 */

#include <stdint.h>

/*
 * rgb packing macros
 */

#define ARGB(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define RGBA(r, g, b, a) (((r) << 24) | ((g) << 16) | ((b) << 8) | (a))
#define RGB565(r, g, b) ((uint16_t)(((((r) >> 3) & 0x1f) << 11) | ((((g) >> 2) & 0x3f) << 5) | (((b) >> 3) & 0x1f)))

/*
 * palette macros
 */

#define PALETTE_RGB24(i, p) ((rgb24_t *)(&p[(i) * 3]))

/*
 * rgb types
 */

typedef struct
{
	uint8_t r, g, b;
} rgb24_t;

typedef struct
{
	uint8_t r, g, b, a;
} rgba32_t;

typedef struct
{
	uint8_t a, r, g, b;
} argb32_t;

/* guards */
#ifdef __cplusplus
}
#endif
#endif /* __RGB_H__ */
