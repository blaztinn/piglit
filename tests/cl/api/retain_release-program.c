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
 * @file retain_release-program.c
 *
 * Test API functions:
 *
 *   cl_int clRetainProgram (cl_program program)
 *   cl_int clReleaseProgram (cl_program program)
 */

#include "piglit-framework-cl-api.h"


PIGLIT_CL_API_TEST_CONFIG_BEGIN

	config.name = "clRetainProgram and clReleaseProgram";
	config.version_min = 10;

	config.run_per_device = true;
	config.create_context = true;

PIGLIT_CL_API_TEST_CONFIG_END


const char* dummy_kernel = "kernel void dummy_kernel() {}";

enum piglit_result
piglit_cl_test(const int argc,
               const char** argv,
               const struct piglit_cl_api_test_config* config,
               const struct piglit_cl_api_test_env* env)
{
	int ref_count = 0;
	const int max_ref_count = 10;
	cl_uint* ref_count_ptr;

	/*** Normal usage ***/

	cl_program program = clCreateProgramWithSource(env->context.cl_ctx,
	                                               1,
	                                               &dummy_kernel,
	                                               NULL,
	                                               NULL);

	ref_count_ptr = piglit_cl_get_program_info(program, CL_PROGRAM_REFERENCE_COUNT);
	if(*ref_count_ptr != 1) {
		free(ref_count_ptr);
		printf("Invalid CL_PROGRAM_REFERENCE_COUNT.\n");
		return PIGLIT_FAIL;
	}
	free(ref_count_ptr);

	/* increase by two and decrease by one on each iteration */
	for(ref_count = 1; ref_count < max_ref_count; ref_count++) {
		if(!piglit_cl_check_error(clRetainProgram(program), CL_SUCCESS)) return PIGLIT_FAIL;
		if(!piglit_cl_check_error(clReleaseProgram(program), CL_SUCCESS)) return PIGLIT_FAIL;
		if(!piglit_cl_check_error(clRetainProgram(program), CL_SUCCESS)) return PIGLIT_FAIL;

		/* check internal value of context reference count */
		ref_count_ptr = piglit_cl_get_program_info(program, CL_PROGRAM_REFERENCE_COUNT);
		if(*ref_count_ptr != (ref_count+1)) {
			free(ref_count_ptr);
			printf("Invalid CL_PROGRAM_REFERENCE_COUNT.\n");
			return PIGLIT_FAIL;
		}
		free(ref_count_ptr);
	}
	/* Decrease reference count to 0 */
	for(ref_count = max_ref_count; ref_count > 0; ref_count--) {
		if(!piglit_cl_check_error(clReleaseProgram(program), CL_SUCCESS)) return PIGLIT_FAIL;

		/* check internal value of context reference count */
		if(ref_count > 1) {
			ref_count_ptr = piglit_cl_get_program_info(program, CL_PROGRAM_REFERENCE_COUNT);
			if(*ref_count_ptr != (ref_count-1)) {
				free(ref_count_ptr);
				printf("Invalid CL_PROGRAM_REFERENCE_COUNT.\n");
				return PIGLIT_FAIL;
			}
			free(ref_count_ptr);
		}
	}

	/*** Errors ***/

	/*
	 * CL_INVALID_PROGRAM if program is not a valid program object.
	 */
	if(!piglit_cl_check_error(clReleaseProgram(program), CL_INVALID_PROGRAM)) return PIGLIT_FAIL;
	if(!piglit_cl_check_error(clReleaseProgram(NULL), CL_INVALID_PROGRAM)) return PIGLIT_FAIL;

	return PIGLIT_PASS;
}
