/* Copyright © 2012 Intel Corporation
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
 * of the GL 3.1 spec.
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
	int vuniforms = 0, vblocks = 0;
	int funiforms = 0, fblocks = 0;
	int blocksize = 0;

	piglit_require_gl_version(31);

	piglit_print_minmax_header();

	/* These should be in the section with "Minimum Value" but
	 * appear in the section with "Initial Value".
	 */
	piglit_test_min_int(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, 64);
	piglit_test_min_int(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, 4);
	piglit_test_min_int(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, 4);

	piglit_test_min_int(GL_MAX_CLIP_DISTANCES, 8);
	piglit_test_min_int(GL_SUBPIXEL_BITS, 4);
	piglit_test_min_int(GL_MAX_3D_TEXTURE_SIZE, 256);
	piglit_test_min_int(GL_MAX_TEXTURE_SIZE, 1024);
	piglit_test_min_int(GL_MAX_ARRAY_TEXTURE_LAYERS, 256);
	piglit_test_min_float(GL_MAX_TEXTURE_LOD_BIAS, 2.0);
	piglit_test_min_int(GL_MAX_CUBE_MAP_TEXTURE_SIZE, 1024);
	piglit_test_min_int(GL_MAX_RENDERBUFFER_SIZE, 1024);

	piglit_test_min_viewport_dimensions();

	piglit_test_range_float(GL_POINT_SIZE_RANGE, 1, 1);
	piglit_test_range_float(GL_ALIASED_LINE_WIDTH_RANGE, 1, 1);
	piglit_test_range_float(GL_SMOOTH_LINE_WIDTH_RANGE, 1, 1);
	piglit_test_min_int(GL_NUM_COMPRESSED_TEXTURE_FORMATS, 4);
	piglit_test_min_int(GL_MAX_TEXTURE_BUFFER_SIZE, 65536);
	piglit_test_min_int(GL_MAX_RECTANGLE_TEXTURE_SIZE, 1024);

	piglit_test_tf_bits(GL_PRIMITIVES_GENERATED);
	piglit_test_tf_bits(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	piglit_test_oq_bits();

	piglit_test_min_int(GL_MAX_VERTEX_ATTRIBS, 16);
	piglit_test_min_int(GL_MAX_VERTEX_UNIFORM_COMPONENTS, 1024);
	piglit_test_min_int(GL_MAX_VARYING_COMPONENTS, 64);
	piglit_test_min_int(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 32);
	piglit_test_min_int(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, 16);
	piglit_test_min_int(GL_MAX_TEXTURE_IMAGE_UNITS, 16);
	piglit_test_min_int(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, 1024);

	piglit_test_max_int(GL_MIN_PROGRAM_TEXEL_OFFSET, -8);
	piglit_test_min_int(GL_MAX_PROGRAM_TEXEL_OFFSET, 7);

	piglit_test_min_int(GL_MAX_VERTEX_UNIFORM_BLOCKS, 12);
	piglit_test_min_int(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, 12);
	piglit_test_min_int(GL_MAX_COMBINED_UNIFORM_BLOCKS, 24);
	piglit_test_min_int(GL_MAX_UNIFORM_BUFFER_BINDINGS, 24);
	piglit_test_min_int(GL_MAX_UNIFORM_BLOCK_SIZE, 16384);
	piglit_test_min_int(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, 1);

	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &vblocks);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &vuniforms);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &fblocks);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &funiforms);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &blocksize);

	/* Note that these two tokens already existed in the table
	 * above, with realistic minimum values.  This appears to be a
	 * typo and was dropped in 3.2.
	 */
	/* piglit_test_min_int(GL_MAX_VERTEX_UNIFORM_COMPONENTS, 1); */
	/* piglit_test_min_int(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, 1); */

	/* In this case, the "1" in the table refers to the footnote:
	 *
	 *     "(1) The minimum value for each stage is
	 *      MAX_stage_UNIFORM_BLOCKS ×
	 *      MAX_stage_UNIFORM_BLOCK_SIZE +
	 *      MAX_stage_UNIFORM_COMPONENTS"
	 */
	piglit_test_min_int(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
			 vblocks * blocksize + vuniforms);
	piglit_test_min_int(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
			 fblocks * blocksize + funiforms);

	piglit_test_min_int(GL_MAX_DRAW_BUFFERS, 8);

	piglit_test_min_int(GL_SAMPLE_BUFFERS, 0);
	piglit_test_min_int(GL_SAMPLES, 0);

	piglit_test_min_int(GL_MAX_COLOR_ATTACHMENTS, 8);
	piglit_test_min_int(GL_MAX_SAMPLES, 4);

	if (!piglit_check_gl_error(GL_NO_ERROR))
		piglit_report_result(PIGLIT_FAIL);

	piglit_report_result(piglit_minmax_pass ? PIGLIT_PASS : PIGLIT_FAIL);
}
