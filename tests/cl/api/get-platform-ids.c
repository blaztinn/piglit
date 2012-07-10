/*
 * Copyright (c) Blaž Tomažič
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEM, IBM AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file get-platform-ids.c
 *
 * Test API function:
 *
 *   cl_int clGetPlatformIDs(cl_uint num_entries,
 *                           cl_platform_id *platforms,
 *                           cl_uint *num_platforms)
 */

#include "piglit-framework-cl-api.h"


PIGLIT_CL_API_TEST_CONFIG_BEGIN

	config.name = "clGetPlatformIDs";
	config.version_min = 10;

PIGLIT_CL_API_TEST_CONFIG_END


enum piglit_result
piglit_cl_test(const int argc,
               const char** argv,
               const struct piglit_cl_api_test_config* config,
               const struct piglit_cl_api_test_env* env)
{
	enum piglit_result result = PIGLIT_PASS;

	int i;
	cl_uint num_platforms;
	cl_platform_id* platforms;

	/*** Normal usage ***/

	/* get number of platforms */
	if(!piglit_cl_check_error(clGetPlatformIDs(0, NULL, &num_platforms),
	                          CL_SUCCESS)) {
		piglit_merge_result(&result, PIGLIT_FAIL);
	}

	/*
	 * Get platform list.
	 * Try returning from 1 to num_platforms platforms.
	 */
	for(i = 1; i <= num_platforms; i++) {
		platforms = malloc(i * sizeof(cl_platform_id));
		if(!piglit_cl_check_error(clGetPlatformIDs(i, platforms, NULL),
		                          CL_SUCCESS)) {
			piglit_merge_result(&result, PIGLIT_FAIL);
		}
		free(platforms);
	}
	
	/*** Errors ***/

	/*
	 * Returns CL_SUCCESS if the function is executed successfully.
	 * Otherwise it returns CL_INVALID_VALUE if num_entries is equal
	 * to zero and platforms is not NULL, or if both num_platforms 
	 * and platforms are NULL.
	 */
	if(!piglit_cl_check_error(clGetPlatformIDs(0, platforms, NULL),
	                          CL_INVALID_VALUE)) {
		piglit_merge_result(&result, PIGLIT_FAIL);
	}
	if(!piglit_cl_check_error(clGetPlatformIDs(100, NULL, NULL),
	                          CL_INVALID_VALUE)) {
		piglit_merge_result(&result, PIGLIT_FAIL);
	}

	return result;
}
