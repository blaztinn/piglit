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
 * @file retain_release-kernel.c
 *
 * Test API functions:
 *
 *   cl_int clRetainKernel (cl_kernel kernel)
 *   cl_int clReleaseKernel (cl_kernel kernel)
 */

#include "piglit-framework-cl-api.h"


PIGLIT_CL_API_TEST_CONFIG_BEGIN

	config.name = "clRetainKernel and clReleaseKernel";
	config.version_min = 10;

	config.run_per_device = true;
	config.create_context = true;

	config.program_source =  "kernel void dummy_kernel() {}";

PIGLIT_CL_API_TEST_CONFIG_END


enum piglit_result
piglit_cl_test(const int argc,
               const char** argv,
               const struct piglit_cl_api_test_config* config,
               const struct piglit_cl_api_test_env* env)
{
	int ref_count = 0;
	const int max_ref_count = 10;
	cl_uint* ref_count_ptr;
	cl_kernel kernel;

	/*** Normal usage ***/

	kernel = clCreateKernel(env->program, "dummy_kernel", NULL);
	if(kernel == NULL) {
		printf("Could not create kernel.\n");
		return PIGLIT_FAIL;
	}

	ref_count_ptr = piglit_cl_get_kernel_info(kernel, CL_KERNEL_REFERENCE_COUNT);
	if(*ref_count_ptr != 1) {
		free(ref_count_ptr);
		printf("Invalid CL_KERNEL_REFERENCE_COUNT.\n");
		return PIGLIT_FAIL;
	}
	free(ref_count_ptr);

	/* increase by two and decrease by one on each iteration */
	for(ref_count = 1; ref_count < max_ref_count; ref_count++) {
		if(!piglit_cl_check_error(clRetainKernel(kernel), CL_SUCCESS)) return PIGLIT_FAIL;
		if(!piglit_cl_check_error(clReleaseKernel(kernel), CL_SUCCESS)) return PIGLIT_FAIL;
		if(!piglit_cl_check_error(clRetainKernel(kernel), CL_SUCCESS)) return PIGLIT_FAIL;

		/* check internal value of context reference count */
		ref_count_ptr = piglit_cl_get_kernel_info(kernel, CL_KERNEL_REFERENCE_COUNT);
		if(*ref_count_ptr != (ref_count+1)) {
			free(ref_count_ptr);
			printf("Invalid CL_KERNEL_REFERENCE_COUNT.\n");
			return PIGLIT_FAIL;
		}
		free(ref_count_ptr);
	}
	/* Decrease reference count to 0 */
	for(ref_count = max_ref_count; ref_count > 0; ref_count--) {
		if(!piglit_cl_check_error(clReleaseKernel(kernel), CL_SUCCESS)) return PIGLIT_FAIL;

		/* check internal value of context reference count */
		if(ref_count > 1) {
			ref_count_ptr = piglit_cl_get_kernel_info(kernel, CL_KERNEL_REFERENCE_COUNT);
			if(*ref_count_ptr != (ref_count-1)) {
				free(ref_count_ptr);
				printf("Invalid CL_KERNEL_REFERENCE_COUNT.\n");
				return PIGLIT_FAIL;
			}
			free(ref_count_ptr);
		}
	}

	/*** Errors ***/

	/*
	 * CL_INVALID_KERNEL if kernel is not a valid kernel object.
	 */
	if(!piglit_cl_check_error(clReleaseKernel(kernel), CL_INVALID_KERNEL)) return PIGLIT_FAIL;
	if(!piglit_cl_check_error(clReleaseKernel(NULL), CL_INVALID_KERNEL)) return PIGLIT_FAIL;

	return PIGLIT_PASS;
}
