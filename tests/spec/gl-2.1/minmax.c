/* Copyright © 2011 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/** @file minmax.c
 *
 * Test for the minimum maximum values in section 6.2 "State Tables"
 * of the GL 2.1 spec.
 */

#include "piglit-util-gl-common.h"
#include "minmax-test.h"

int piglit_width = 32;
int piglit_height = 32;
int piglit_window_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA;

enum piglit_result
piglit_display(void)
{
	/* UNREACHED */
	return PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	piglit_require_gl_version(21);

	piglit_print_minmax_header();

	piglit_test_min_int(GL_MAX_LIGHTS, 8);
	piglit_test_min_int(GL_MAX_CLIP_PLANES, 6);
	if (piglit_is_extension_supported("GL_ARB_imaging"))
		piglit_test_min_int(GL_MAX_COLOR_MATRIX_STACK_DEPTH, 2);
	piglit_test_min_int(GL_MAX_MODELVIEW_STACK_DEPTH, 32);
	piglit_test_min_int(GL_MAX_PROJECTION_STACK_DEPTH, 2);
	piglit_test_min_int(GL_MAX_TEXTURE_STACK_DEPTH, 2);
	piglit_test_min_int(GL_SUBPIXEL_BITS, 4);

	piglit_test_min_int(GL_MAX_3D_TEXTURE_SIZE, 16);
	piglit_test_min_int(GL_MAX_TEXTURE_SIZE, 64);
	piglit_test_min_float(GL_MAX_TEXTURE_LOD_BIAS, 2.0);
	piglit_test_min_int(GL_MAX_CUBE_MAP_TEXTURE_SIZE, 16);
	piglit_test_min_int(GL_MAX_PIXEL_MAP_TABLE, 32);
	piglit_test_min_int(GL_MAX_NAME_STACK_DEPTH, 64);
	piglit_test_min_int(GL_MAX_LIST_NESTING, 64);
	piglit_test_min_int(GL_MAX_EVAL_ORDER, 8);

	piglit_test_min_viewport_dimensions();

	piglit_test_min_int(GL_MAX_ATTRIB_STACK_DEPTH, 16);
	piglit_test_min_int(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH, 16);

	piglit_test_range_float(GL_ALIASED_POINT_SIZE_RANGE, 1, 1);
	piglit_test_range_float(GL_SMOOTH_POINT_SIZE_RANGE, 1, 1);
	piglit_test_range_float(GL_ALIASED_LINE_WIDTH_RANGE, 1, 1);
	piglit_test_range_float(GL_SMOOTH_LINE_WIDTH_RANGE, 1, 1);

	piglit_test_oq_bits();

	piglit_test_min_int(GL_AUX_BUFFERS, 0);

	if (piglit_is_extension_supported("GL_ARB_imaging")) {
		/* FINISHME: GL_MAX_CONVOLUTION_WIDTH */
		/* FINISHME: GL_MAX_CONVOLUTION_HEIGHT */
	}

	piglit_test_min_int(GL_SAMPLE_BUFFERS, 0);
	piglit_test_min_int(GL_SAMPLES, 0);

	piglit_test_min_int(GL_MAX_TEXTURE_UNITS, 2);
	piglit_test_min_int(GL_MAX_VERTEX_ATTRIBS, 16);
	piglit_test_min_int(GL_MAX_VERTEX_UNIFORM_COMPONENTS, 512);
	piglit_test_min_int(GL_MAX_VARYING_COMPONENTS, 32);
	piglit_test_min_int(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 2);
	piglit_test_min_int(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, 0);
	piglit_test_min_int(GL_MAX_TEXTURE_IMAGE_UNITS, 2);
	piglit_test_min_int(GL_MAX_TEXTURE_COORDS, 2);
	piglit_test_min_int(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, 64);
	piglit_test_min_int(GL_MAX_DRAW_BUFFERS, 1);

	if (!piglit_check_gl_error(GL_NO_ERROR))
		piglit_report_result(PIGLIT_FAIL);

	piglit_report_result(piglit_minmax_pass ? PIGLIT_PASS : PIGLIT_FAIL);
}
