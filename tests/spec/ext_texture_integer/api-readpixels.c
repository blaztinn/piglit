/*
 * Copyright (c) 2010 VMware, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL VMWARE AND/OR THEIR SUPPLIERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file api-readpixels.c
 *
 * Tests GL_EXT_texture_integer's error behavior with glReadPixels().
 */

#include "piglit-util-gl-common.h"

int piglit_width = 10, piglit_height = 10;
int piglit_window_mode = GLUT_RGB | GLUT_ALPHA | GLUT_DOUBLE;

enum piglit_result
piglit_display(void)
{
	/* UNREACHED */
	return PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	GLfloat buf[64];

	piglit_require_extension("GL_EXT_texture_integer");

	/* Is GL_INVALID_ENUM generated by glReadPixels? */
	glReadPixels(0, 0, 4, 4, GL_RGBA_INTEGER, GL_FLOAT, buf);
	if (!piglit_check_gl_error(GL_INVALID_ENUM))
		piglit_report_result(PIGLIT_FAIL);

	/* Is GL_INVALID_OPERATION generated by glReadPixels? */
	glReadPixels(0, 0, 4, 4, GL_RGBA_INTEGER, GL_UNSIGNED_INT, buf);
	if (!piglit_check_gl_error(GL_INVALID_OPERATION))
		piglit_report_result(PIGLIT_FAIL);

	piglit_report_result(PIGLIT_PASS);
}
