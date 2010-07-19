/*
 * Copyright © 2009 Intel Corporation
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
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *    Marek Olšák <maraeo@gmail.com>
 *
 */

/** @file fbo-d24s8.c
 *
 * Tests that rendering to a 1D texture with a D24S8 texture
 * and then drawing both to the framebuffer succeeds.
 */

#include "piglit-util.h"

#define BUF_WIDTH 16
int piglit_width = 50;
int piglit_height = 20;
int piglit_window_mode = GLUT_DOUBLE | GLUT_DEPTH;

static void
create_1d_fbo(GLuint *out_tex, GLuint *out_ds)
{
	GLuint tex, ds, fb;
	GLenum status;

    /* Create the color buffer. */
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_1D, tex);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA,
		     BUF_WIDTH,
		     0,
		     GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	assert(glGetError() == 0);

	/* Create the depth-stencil buffer. */
	glGenTextures(1, &ds);
	glBindTexture(GL_TEXTURE_1D, ds);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_DEPTH24_STENCIL8_EXT,
		     BUF_WIDTH,
		     0,
		     GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, NULL);
	assert(glGetError() == 0);

    /* Create the FBO. */
	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

	glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT,
				  GL_COLOR_ATTACHMENT0_EXT,
				  GL_TEXTURE_1D,
				  tex,
				  0);

	glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT,
				  GL_DEPTH_ATTACHMENT_EXT,
				  GL_TEXTURE_1D,
				  ds,
				  0);

	glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT,
				  GL_STENCIL_ATTACHMENT_EXT,
				  GL_TEXTURE_1D,
				  ds,
				  0);

	assert(glGetError() == 0);

	status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		fprintf(stderr, "FBO incomplete\n");
		goto done;
	}

	glViewport(0, 0, BUF_WIDTH, 1);
	glClearDepth(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glDepthRange(0, 0);

	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	piglit_ortho_projection(BUF_WIDTH, 1, GL_FALSE);

    /* green */
	glColor4f(0.0, 1.0, 0.0, 0.0);
	piglit_draw_rect(0, 0, BUF_WIDTH, 1);

done:
	glDeleteFramebuffersEXT(1, &fb);

	*out_tex = tex;
	*out_ds = ds;
}

static void
draw_fbo_1d(int x, int y)
{
	glViewport(0, 0, piglit_width, piglit_height);
	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glEnable(GL_TEXTURE_1D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	piglit_draw_rect_tex(x, y, BUF_WIDTH, 1,
			     0, 0, 1, 1);
}

enum piglit_result
piglit_display(void)
{
	GLboolean pass = GL_TRUE;
	float black[] = {0,0,0,0};
	float green[] = {0,1,0,0};
	float *expected;
	int x;
	GLuint tex, ds;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	create_1d_fbo(&tex, &ds);

	glBindTexture(GL_TEXTURE_1D, tex);
	draw_fbo_1d(10, 10);
	glBindTexture(GL_TEXTURE_1D, ds);
	draw_fbo_1d(10+BUF_WIDTH, 10);

	for (x = 0; x < BUF_WIDTH*2; x++) {
		if (x < BUF_WIDTH)
			expected = green;
		else
			expected = black;

		pass &= piglit_probe_pixel_rgb(10 + x, 10, expected);
	}

	glDeleteTextures(1, &tex);
	glDeleteTextures(1, &ds);

	glutSwapBuffers();

	return pass ? PIGLIT_SUCCESS : PIGLIT_FAILURE;
}

void piglit_init(int argc, char **argv)
{
	piglit_require_extension("GL_EXT_framebuffer_object");
	piglit_require_extension("GL_EXT_packed_depth_stencil");
}