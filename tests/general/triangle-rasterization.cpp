/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

/**
 * Triangle Rasterization Test
 *
 * This tests OpenGL triangle rasterization by comparing it with a software rasteriser
 *
 * There are 2 components to the test;
 *   1. Predefined sanity tests ensuring bounding box calculations are correct
 *   2. Randomised triangle drawing to attempt to test all possible triangles
 */

#include "piglit-util-gl-common.h"
#include "mersenne.hpp"

#include <time.h>
#include <vector>
#include <algorithm>

/* Data structures */
struct Vector
{
   Vector()
   {
   }

   Vector(float x, float y)
      : x(x), y(y)
   {
   }

   float x, y;
};

struct Triangle {
   Triangle()
   {
   }

   Triangle(const Vector& v0, const Vector& v1, const Vector& v2)
   {
      v[0] = v0;
      v[1] = v1;
      v[2] = v2;
   }

   Vector& operator[](int i)
   {
      return v[i];
   }

   const Vector& operator[](int i) const
   {
      return v[i];
   }

   Vector v[3];
};


/* Command line arguments */
bool use_fbo = false;
bool break_on_fail = false;
bool print_triangle = false;
int random_test_count = 100;

/* Fixed point format */
const int FIXED_SHIFT = 4;
const int FIXED_ONE = 1 << FIXED_SHIFT;

/* Default test size */
int fbo_width = 256;
int fbo_height = 256;

/* Piglit variables */
int piglit_width = fbo_width;
int piglit_height = fbo_height;
int piglit_window_mode = GLUT_RGB | GLUT_ALPHA | GLUT_DOUBLE;

/* Globals */
int test_id = 0;
Mersenne mersenne;
std::vector<Triangle> fixed_tests;


/* std::algorithm min/max with 3 arguments! :D */
namespace std {
   template<typename T>
   T min(T& a, T& b, T& c)
   {
      return (a < b) ? std::min(a, c) : std::min(b, c);
   }

   template<typename T>
   T max(T& a, T& b, T& c)
   {
      return (a > b) ? std::max(a, c) : std::max(b, c);
   }
}

/* Proper rounding of float to integer */
int iround(float v)
{
   if (v > 0.0f) v += 0.5f;
   if (v < 0.0f) v -= 0.5f;
   return (int)v;
}

/* Calculate log2 for integers */
int log2i(int x)
{
   int res = 0 ;
   while (x >>= 1) ++res;
   return res ;
}


/* Based on http://devmaster.net/forums/topic/1145-advanced-rasterization */
void rast_triangle(uint8_t* buffer, uint32_t stride, const Triangle& tri)
{
   float center_offset = -0.5f;

   /* 28.4 fixed point coordinates */
   int x1 = iround(FIXED_ONE * (tri[0].x + center_offset));
   int x2 = iround(FIXED_ONE * (tri[1].x + center_offset));
   int x3 = iround(FIXED_ONE * (tri[2].x + center_offset));

   int y1 = iround(FIXED_ONE * (tri[0].y + center_offset));
   int y2 = iround(FIXED_ONE * (tri[1].y + center_offset));
   int y3 = iround(FIXED_ONE * (tri[2].y + center_offset));

   /* Force correct vertex order */
   const int cross = (x2 - x1) * (y3 - y2) - (y2 - y1) * (x3 - x2);
   if (cross > 0) {
      std::swap(x1, x3);
      std::swap(y1, y3);
   }

   /* Deltas */
   const int dx12 = x1 - x2;
   const int dx23 = x2 - x3;
   const int dx31 = x3 - x1;

   const int dy12 = y1 - y2;
   const int dy23 = y2 - y3;
   const int dy31 = y3 - y1;

   /* Fixed-point deltas */
   const int fdx12 = dx12 << FIXED_SHIFT;
   const int fdx23 = dx23 << FIXED_SHIFT;
   const int fdx31 = dx31 << FIXED_SHIFT;

   const int fdy12 = dy12 << FIXED_SHIFT;
   const int fdy23 = dy23 << FIXED_SHIFT;
   const int fdy31 = dy31 << FIXED_SHIFT;

   /* Bounding rectangle */
   int minx = std::min(x1, x2, x3) >> FIXED_SHIFT;
   int maxx = (std::max(x1, x2, x3) - 1) >> FIXED_SHIFT;

   int miny = (std::min(y1, y2, y3) + 1) >> FIXED_SHIFT;
   int maxy = std::max(y1, y2, y3) >> FIXED_SHIFT;

   minx = std::max(minx, 0);
   maxx = std::min(maxx, fbo_width - 1);

   miny = std::max(miny, 0);
   maxy = std::min(maxy, fbo_height - 1);

   /* Half-edge constants */
   int c1 = dy12 * x1 - dx12 * y1;
   int c2 = dy23 * x2 - dx23 * y2;
   int c3 = dy31 * x3 - dx31 * y3;

   /* Correct for fill convention */
   if (dy12 < 0 || (dy12 == 0 && dx12 > 0)) c1++;
   if (dy23 < 0 || (dy23 == 0 && dx23 > 0)) c2++;
   if (dy31 < 0 || (dy31 == 0 && dx31 > 0)) c3++;

   int cy1 = c1 + dx12 * (miny << FIXED_SHIFT) - dy12 * (minx << FIXED_SHIFT);
   int cy2 = c2 + dx23 * (miny << FIXED_SHIFT) - dy23 * (minx << FIXED_SHIFT);
   int cy3 = c3 + dx31 * (miny << FIXED_SHIFT) - dy31 * (minx << FIXED_SHIFT);

   /* Perform rasterization */
   buffer += miny * stride;
   for (int y = miny; y <= maxy; y++) {
      int cx1 = cy1;
      int cx2 = cy2;
      int cx3 = cy3;

      for (int x = minx; x <= maxx; x++) {
         if (cx1 > 0 && cx2 > 0 && cx3 > 0) {
            ((uint32_t*)buffer)[x] = 0x00FF00FF;
         }

         cx1 -= fdy12;
         cx2 -= fdy23;
         cx3 -= fdy31;
      }

      cy1 += fdx12;
      cy2 += fdx23;
      cy3 += fdx31;

      buffer += stride;
   }
}


/* Prints an ascii representation of the triangle */
void triangle_art(uint32_t* buffer)
{
   int minx = fbo_width - 1, miny = fbo_height - 1;
   int maxx = 0, maxy = 0;

   /* Find bounds so we dont have to print whole screen */
   for (int y = 0; y < fbo_height; ++y) {
      for (int x = 0; x < fbo_width; ++x) {
         if (buffer[y*fbo_width + x] & 0xFFFFFF00) {
            if (x < minx) minx = x;
            if (y < miny) miny = y;
            if (x > maxx) maxx = x;
            if (y > maxy) maxy = y;
         }
      }
   }

   /* Nothing drawn */
   if (minx > maxx || miny > maxy)
      return;

   --minx; --miny;
   ++maxx; ++maxy;

   /* Print an ascii representation of triangle */
   for (int y = maxy; y >= miny; --y) {
      for (int x = minx; x <= maxx; ++x) {
         uint32_t val = buffer[y*fbo_width + x] & 0xFFFFFF00;

         if (val == 0xFF000000) {
            printf("+");
         } else if (val == 0x00FF0000) {
            printf("-");
         } else if (val == 0xFFFF0000) {
            printf("o");
         } else if (val == 0) {
            printf(".");
         } else {
            printf("?");
         }
      }

      printf("\n");
   }

   printf("\n");
}


/* Reads buffer from OpenGL and checks for any colour other than black or yellow
 * (black = background, yellow = both opengl AND software rast drew to that pixel)
 */
uint32_t* check_triangle()
{
   static uint32_t* buffer = 0;
   if (!buffer) buffer = new uint32_t[fbo_width * fbo_height];

   glReadPixels(0, 0, fbo_width, fbo_height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer);

   for (int y = 0; y < fbo_height; ++y) {
      for (int x = 0; x < fbo_width; ++x) {
         uint32_t val = buffer[y*fbo_width + x] & 0xFFFFFF00;

         if (val != 0 && val != 0xFFFF0000) {
            return buffer;
         }
      }
   }

   return NULL;
}


/* Performs test using tri */
GLboolean test_triangle(const Triangle& tri)
{
   static uint32_t* buffer = 0;
   if (!buffer) buffer = new uint32_t[fbo_width * fbo_height];

   /* Clear OpenGL and software buffer */
   glClear(GL_COLOR_BUFFER_BIT);
   memset(buffer, 0, sizeof(uint32_t) * fbo_width * fbo_height);

   /* Software rasterise triangle and blit it to OpenGL */
   rast_triangle((uint8_t*)buffer, fbo_width * 4, tri);
   glDrawPixels(fbo_width, fbo_height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer);

   /* Draw OpenGL triangle */
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(2, GL_FLOAT, 0, tri.v);
   glDrawArrays(GL_TRIANGLES, 0, 3);
   glDisableClientState(GL_VERTEX_ARRAY);

   /* Check the result and print relevant error messages */
   if (uint32_t* result = check_triangle()) {
      printf("FAIL: %d. (%f, %f), (%f, %f), (%f, %f)\n", test_id,
             tri[0].x, tri[0].y, tri[1].x, tri[1].y, tri[2].x, tri[2].y);

      if (print_triangle) {
         triangle_art(result);
      }

      fflush(stdout);
      return GL_FALSE;
   }

   return GL_TRUE;
}


/* Generate a random triangle */
void random_triangle(Triangle& tri)
{
   int size = 1 << (mersenne.value() % (log2i(fbo_width) + 1));

   for (int i = 0; i < 3; ++i) {
      tri[i].x = (mersenne.value() % (size * FIXED_ONE)) * (1.0f / FIXED_ONE);
      tri[i].y = (mersenne.value() % (size * FIXED_ONE)) * (1.0f / FIXED_ONE);
   }

   test_id++;
}


/* Render */
enum piglit_result
piglit_display(void)
{
   GLuint fb, tex;

   /* If using FBO, set it up */
   if (use_fbo) {
      glDisable(GL_CULL_FACE);

      glGenTextures(1, &tex);
      glBindTexture(GL_TEXTURE_2D, tex);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbo_width, fbo_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

      glGenFramebuffersEXT(1, &fb);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
      glViewport(0, 0, fbo_width, fbo_height);

      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                 GL_COLOR_ATTACHMENT0_EXT,
                 GL_TEXTURE_2D,
                 tex,
                 0);

      assert(glGetError() == 0);
      assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT);

      glBindTexture(GL_TEXTURE_2D, 0);
   }

   /* Set render state */
   glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

   glEnable(GL_BLEND);
   glBlendEquation(GL_FUNC_ADD);
   glBlendFunc(GL_ONE, GL_ONE);

   glViewport(0, 0, fbo_width, fbo_height);
   piglit_ortho_projection(fbo_width, fbo_height, GL_FALSE);

   /* Perform test */
   GLboolean pass = GL_TRUE;
   if (piglit_automatic) {
      int fail_count = 0;

      printf("Running %d fixed tests\n", (int)fixed_tests.size());
      for (std::vector<Triangle>::iterator itr = fixed_tests.begin(); itr != fixed_tests.end() && !(fail_count && break_on_fail); ++itr) {
         if (!test_triangle(*itr))
            ++fail_count;
      }

      printf("Running %d random tests\n", random_test_count);
      for (int i = 0; i < random_test_count && !(fail_count && break_on_fail); ++i) {
         Triangle tri;
         random_triangle(tri);

         if (!test_triangle(tri))
            ++fail_count;
      }

      printf("Failed %d tests\n", fail_count);
      fflush(stdout);

      if (fail_count)
         pass = GL_FALSE;
   } else {
      Triangle tri;
      random_triangle(tri);
      pass &= test_triangle(tri);

      glDisable(GL_BLEND);

      /* If using FBO, draw the fbo to screen */
      if (use_fbo) {
         glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
         glViewport(0, 0, piglit_width, piglit_height);
         piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);

         glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
         glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,   GL_REPLACE);
         glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);

         glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
         glEnable(GL_TEXTURE_2D);
         glBindTexture(GL_TEXTURE_2D, tex);

         piglit_draw_rect_tex(0, 0, piglit_width, piglit_height, 0, 0, 1, 1);

         glDisable(GL_TEXTURE_2D);
      }

      piglit_present_results();
   }

   /* Cleanup FBO if necessary */
   if (use_fbo) {
      glDeleteTextures(1, &tex);
      glDeleteFramebuffersEXT(1, &fb);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   }

   assert(glGetError() == 0);

   return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}


/* Create some fixed tests to test bounding box in/exclusivity,
 *
 *  /|\
 * /_|_\  Tests these 4 triangles but shifting them from -1/16 to +1/16
 * \ | /  around the center point
 *  \|/
 */
void init_fixed_tests()
{
   const float mid = 0.5f;
   const float shift = 1.0f / FIXED_ONE;
   const float size  = 3.0f / FIXED_ONE;

   Vector vv[] = {
      Vector(mid, mid + size),
      Vector(mid, mid - size),
   };

   Vector vh[] = {
      Vector(mid - size, mid),
      Vector(mid + size, mid),
   };

   Vector vm(mid, mid);

   /* Loop through the 4 possible triangles */
   for (int vy = 0; vy < 2; ++vy) {
      for (int vx = 0; vx < 2; ++vx) {
         Triangle tri;

         tri[0] = vh[vx];
         tri[1] = vv[vy];
         tri[2] = vm;

         /* Loop through the x and y shifts */
         for (int y = -1; y <= 1; ++y) {
            for (int x = -1; x <= 1; ++x) {
               Triangle shifted = tri;

               for (int i = 0; i < 3; ++i) {
                  shifted[i].x += x * shift;
                  shifted[i].y += y * shift;
               }

               fixed_tests.push_back(shifted);
            }
         }
      }
   }
}


/* Read command line arguments! */
void
piglit_init(int argc, char **argv)
{
   uint32_t seed = 0xfacebeef ^ time(NULL);

   for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "-break_on_fail") == 0){
         break_on_fail = true;
         printf("Execution will stop on first fail\n");
      } else if (strcmp(argv[i], "-print_triangle") == 0){
         print_triangle = true;
      } else if (strcmp(argv[i], "-max_size") == 0){
         glGetIntegerv(GL_MAX_TEXTURE_SIZE, &fbo_width);

         fbo_height = fbo_width;
         piglit_width = fbo_width;
         piglit_height = fbo_height;
      } else if (strcmp(argv[i], "-use_fbo") == 0){
         use_fbo = true;
         printf("FBOs are in use\n");
      } else if (i + 1 < argc) {
         if (strcmp(argv[i], "-count") == 0) {
            random_test_count = strtoul(argv[++i], NULL, 0);
         } else if (strcmp(argv[i], "-seed") == 0) {
            seed = strtoul(argv[++i], NULL, 0);
         }
      }
   }

   printf("Random seed: 0x%08X\n", seed);
   mersenne.init(seed);

   init_fixed_tests();
}
