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
 * @file create-context.c
 *
 * Test API function:
 *
 *   cl_context clCreateContext(cl_context_properties *properties,
 *                              cl_uint num_devices,
 *                              const cl_device_id *devices,
 *                              void pfn_notify (
 *                                  const char *errinfo,
 *                                  const void *private_info,
 *                                  size_t cb,
 *                                  void *user_data
 *                              ),
 *                              void *user_data,
 *                              cl_int *errcode_ret)
 */

#include "piglit-framework-cl-api.h"


PIGLIT_CL_API_TEST_CONFIG_BEGIN

	config.name = "clCreateContext";
	config.version_min = 10;

	config.run_per_platform = true;

PIGLIT_CL_API_TEST_CONFIG_END


static void
test(cl_context_properties *properties,
     cl_uint num_devices,
     const cl_device_id *devices,
     void pfn_notify (
         const char *errinfo,
         const void *private_info,
         size_t cb,
         void *user_data
     ),
     void *user_data,
     cl_int expected_error,
     enum piglit_result* result) {
	cl_int errNo;
	cl_context cl_ctx;

	/* with errNo */
	cl_ctx = clCreateContext(properties,
	                         num_devices,
	                         devices,
	                         pfn_notify,
	                         user_data,
	                         &errNo);
	
	if(!piglit_cl_check_error(errNo, expected_error)) {
		piglit_merge_result(result, PIGLIT_FAIL);
		return;
	};
	if(expected_error == CL_SUCCESS) {
		if(cl_ctx == NULL) {
			printf("Expecting non-NULL cl_context\n");
			piglit_merge_result(result, PIGLIT_FAIL);
			return;
		}
		clReleaseContext(cl_ctx);
	} else if(cl_ctx != NULL) {
		printf("Expecting non-NULL cl_context\n");
		piglit_merge_result(result, PIGLIT_FAIL);
		return;
	}

	/* without errNo */
	cl_ctx = clCreateContext(properties,
	                         num_devices,
	                         devices,
	                         pfn_notify,
	                         user_data,
	                         NULL);
	
	if(expected_error == CL_SUCCESS) {
		if(cl_ctx == NULL) {
			printf("Expecting non-NULL cl_context\n");
			piglit_merge_result(result, PIGLIT_FAIL);
			return;
		}
		clReleaseContext(cl_ctx);
	} else if(cl_ctx != NULL) {
		printf("Expecting non-NULL cl_context\n");
		piglit_merge_result(result, PIGLIT_FAIL);
		return;
	}
}

enum piglit_result
piglit_cl_test(const int argc,
               const char** argv,
               const struct piglit_cl_api_test_config* config,
               const struct piglit_cl_api_test_env* env)
{
	enum piglit_result result = PIGLIT_PASS;

	int i;
	cl_uint num_devices;
	cl_device_id* devices;
	
	//TODO: test also CL_CONTEXT_INTEROP_USER_SYNC
	cl_context_properties context_properties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)env->platform_id,
		0
	};
	cl_context_properties invalid_context_properties[] = {
		CL_DEVICE_NAME, (cl_context_properties)env->platform_id,
		0
	};
	cl_context_properties invalid_platform_context_properties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)NULL,
		0
	};
	cl_context_properties multiple_platform_context_properties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)env->platform_id,
		CL_CONTEXT_PLATFORM, (cl_context_properties)env->platform_id,
		0
	};

	/*** Normal usage ***/

	/* get device ids */
	num_devices = piglit_cl_get_device_ids(env->platform_id, &devices);

	/*
	 * Create context.
	 * Try creating context from 1 to num_devices devices.
	 */
	for(i = 1; i <= num_devices; i++) {
		test(context_properties, i, devices, NULL, NULL,
		     CL_SUCCESS, &result);
		//TODO: test callback functions
	}
	
	/*** Errors ***/
	
	/*
	 * CL_INVALID_PLATFORM if properties is NULL and no platform 
	 * could be selected or if platform value specified in properties
	 * is not a valid platform.
	 *
	 * Note: Can not test implementation-defined behaviour on
	 *       NULL context_properties.
	 */
	test(invalid_platform_context_properties, num_devices, devices, NULL, NULL,
	     CL_INVALID_PLATFORM, &result);

	/*
	 * CL_INVALID_VALUE if context property name in properties is
	 * not a supported property name; if devices is NULL; if
	 * num_devices is equal to zero; or if pfn_notify is NULL but
	 * user_data is not NULL.
	 *
	 * Version: 1.0
	 *
	 * CL_INVALID_VALUE if devices is NULL; if num_devices is equal
	 * to zero; or if pfn_notify is NULL but user_data is not NULL.
	 *
	 * Version: 1.1
	 */
	if(env->version <= 10) {
		test(invalid_context_properties, num_devices, devices, NULL, NULL,
		     CL_INVALID_VALUE, &result);
	}
	test(context_properties, num_devices, NULL, NULL, NULL,
	     CL_INVALID_VALUE, &result);
	test(context_properties, num_devices, devices, NULL, &context_properties,
	     CL_INVALID_VALUE, &result);
	test(context_properties, 0, devices, NULL, NULL,
	     CL_INVALID_VALUE, &result);

	/*
	 * CL_INVALID_PROPERTY if context property name in properties
	 * is not a supported property name, if the value specified for
	 * a supported property name is not valid, or if the same
	 * property name is specified more than once.
	 *
	 * Version: 1.1
	 */
#if defined(CL_VERSION_1_1)
	if(env->version >= 11) {
		test(invalid_context_properties, num_devices, devices, NULL, NULL,
		     CL_INVALID_PROPERTY, &result);
		test(multiple_platform_context_properties, num_devices, devices, NULL, NULL,
		     CL_INVALID_PROPERTY, &result);
	}
#endif

	/*
	 * CL_INVALID_DEVICE if devices contains an invalid device
	 * or are not associated with the specified platform.
	 * 
	 * TODO: implement
	 */
	/*
	 * CL_DEVICE_NOT_AVAILABLE if a device in devices is currently
	 * not available even though the device was returned by clGetDeviceIDs.
	 *
	 * Note: Can not test
	 */

	free(devices);

	return result;
}
