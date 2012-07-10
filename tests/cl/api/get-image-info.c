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
 * @file get-image-info.c
 *
 * Test API function:
 *
 *   cl_int clGetImageInfo (cl_mem image,
 *                          cl_image_info param_name,
 *                          size_t param_value_size,
 *                          void *param_value,
 *                          size_t *param_value_size_ret)
 */

#include "piglit-framework-cl-api.h"


PIGLIT_CL_API_TEST_CONFIG_BEGIN

	config.name = "clGetImageInfo";
	config.version_min = 10;

	config.run_per_platform = true;
	config.create_context = true;

PIGLIT_CL_API_TEST_CONFIG_END


enum piglit_result
piglit_cl_test(const int argc,
               const char** argv,
               const struct piglit_cl_api_test_config* config,
               const struct piglit_cl_api_test_env* env)
{
	enum piglit_result result = PIGLIT_PASS;

	int i;
	cl_int errNo;
	cl_mem image;
	cl_image_format image_format = {
		.image_channel_order = CL_RGBA,
		.image_channel_data_type = CL_FLOAT,
	};

	size_t param_value_size;
	void* param_value;
	
	int num_image_infos = PIGLIT_CL_ENUM_NUM(cl_image_info, env->version);
	const cl_image_info* image_infos = PIGLIT_CL_ENUM_ARRAY(cl_image_info);
	
	if(env->version <= 11) {
		image = clCreateImage2D(env->context.cl_ctx,
		                        CL_MEM_READ_WRITE,
		                        &image_format,
		                        128, 128, 0,
		                        NULL,
		                        &errNo);
	} else {
#if defined CL_VERSION_1_2
		cl_image_desc image_desc = {
			.image_type = CL_MEM_OBJECT_IMAGE2D,
			.image_width = 128,
			.image_height = 128,
			.image_row_pitch = 0,
			.image_slice_pitch = 0,
			.num_mip_levels = 0,
			.num_samples = 0,
			.buffer = NULL,
		};

		image = clCreateImage(env->context.cl_ctx,
		                      CL_MEM_READ_WRITE,
		                      &image_format,
		                      &image_desc,
		                      NULL,
		                      &errNo);
#else //CL_VERSION_1_2
		printf("This should not happen.\n");
		return PIGLIT_FAIL;
#endif //CL_VERSION_1_2
	}
	if(!piglit_cl_check_error(errNo, CL_SUCCESS)) {
		printf("Can't create image.\n");
		return PIGLIT_FAIL;
	}

	/*** Normal usage ***/
	for(i = 0; i < num_image_infos; i++) {
		printf("%s ", piglit_cl_get_enum_name(image_infos[i]));

		if(!piglit_cl_check_error(clGetImageInfo(image,
		                                         image_infos[i],
		                                         0,
		                                         NULL,
		                                         &param_value_size),
		                          CL_SUCCESS)) {
			piglit_merge_result(&result, PIGLIT_FAIL);
			continue;
		}

		param_value = malloc(param_value_size);
		if(!piglit_cl_check_error(clGetImageInfo(image,
		                                         image_infos[i],
		                                         param_value_size,
		                                         param_value,
		                                         NULL),
		                          CL_SUCCESS)) {
			piglit_merge_result(&result, PIGLIT_FAIL);
		}

		//TODO: output returned values
		printf("\n");
		free(param_value);
	}

	/*** Errors ***/

	/*
	 * CL_INVALID_VALUE if param_name is not one of the supported
	 * values or if size in bytes specified by param_value_size is
	 * less than size of return type and param_value is not a NULL
	 * value.
	 */
	if(!piglit_cl_check_error(clGetImageInfo(image,
	                                         CL_DEVICE_NAME,
	                                         0,
	                                         NULL,
	                                         &param_value_size),
	                          CL_INVALID_VALUE)) {
		piglit_merge_result(&result, PIGLIT_FAIL);
	}

	if(!piglit_cl_check_error(clGetImageInfo(image,
	                                         CL_IMAGE_FORMAT,
	                                         1,
	                                         param_value,
	                                         NULL),
	                          CL_INVALID_VALUE)) {
		piglit_merge_result(&result, PIGLIT_FAIL);
	}
	
	/*
	 * CL_INVALID_MEM_OBJECT if image is a not a valid image object.
	 */
	if(!piglit_cl_check_error(clGetImageInfo(NULL,
	                                         CL_IMAGE_FORMAT,
	                                         0,
	                                         NULL,
	                                         &param_value_size),
	                          CL_INVALID_MEM_OBJECT)) {
		piglit_merge_result(&result, PIGLIT_FAIL);
	}

	clReleaseMemObject(image);

	return result;
}
