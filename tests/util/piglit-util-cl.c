/*
 * Copyright (c) Piglit 2012
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
 *
 * Authors:
 *    Blaž Tomažič <blaz.tomazic@gmail.com>
 *
 */

#include "piglit-util-cl.h"


const char* piglit_cl_get_error_name(cl_int error) {
#define CASE(x) case x: return #x;

    switch (error) {
	CASE(CL_SUCCESS)                                  // 0
	CASE(CL_DEVICE_NOT_FOUND)                         // -1
	CASE(CL_DEVICE_NOT_AVAILABLE)                     // -2
	CASE(CL_COMPILER_NOT_AVAILABLE)                   // -3
	CASE(CL_MEM_OBJECT_ALLOCATION_FAILURE)            // -4
	CASE(CL_OUT_OF_RESOURCES)                         // -5
	CASE(CL_OUT_OF_HOST_MEMORY)                       // -6
	CASE(CL_PROFILING_INFO_NOT_AVAILABLE)             // -7
	CASE(CL_MEM_COPY_OVERLAP)                         // -8
	CASE(CL_IMAGE_FORMAT_MISMATCH)                    // -9
	CASE(CL_IMAGE_FORMAT_NOT_SUPPORTED)               // -10
	CASE(CL_BUILD_PROGRAM_FAILURE)                    // -11
	CASE(CL_MAP_FAILURE)                              // -12
#ifdef CL_VERSION_1_1
	CASE(CL_MISALIGNED_SUB_BUFFER_OFFSET)             // -13
	CASE(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST) // -14
#endif //CL_VERSION_1_1
#ifdef CL_VERSION_1_2
	CASE(CL_COMPILE_PROGRAM_FAILURE)                  // -15
	CASE(CL_LINKER_NOT_AVAILABLE)                     // -16
	CASE(CL_LINK_PROGRAM_FAILURE)                     // -17
	CASE(CL_DEVICE_PARTITION_FAILED)                  // -18
	CASE(CL_KERNEL_ARG_INFO_NOT_AVAILABLE)            // -19
#endif //CL_VERSION_1_2

	CASE(CL_INVALID_VALUE)                            // -30
	CASE(CL_INVALID_DEVICE_TYPE)                      // -31
	CASE(CL_INVALID_PLATFORM)                         // -32
	CASE(CL_INVALID_DEVICE)                           // -33
	CASE(CL_INVALID_CONTEXT)                          // -34
	CASE(CL_INVALID_QUEUE_PROPERTIES)                 // -35
	CASE(CL_INVALID_COMMAND_QUEUE)                    // -36
	CASE(CL_INVALID_HOST_PTR)                         // -37
	CASE(CL_INVALID_MEM_OBJECT)                       // -38
	CASE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)          // -39
	CASE(CL_INVALID_IMAGE_SIZE)                       // -40
	CASE(CL_INVALID_SAMPLER)                          // -41
	CASE(CL_INVALID_BINARY)                           // -42
	CASE(CL_INVALID_BUILD_OPTIONS)                    // -43
	CASE(CL_INVALID_PROGRAM)                          // -44
	CASE(CL_INVALID_PROGRAM_EXECUTABLE)               // -45
	CASE(CL_INVALID_KERNEL_NAME)                      // -46
	CASE(CL_INVALID_KERNEL_DEFINITION)                // -47
	CASE(CL_INVALID_KERNEL)                           // -48
	CASE(CL_INVALID_ARG_INDEX)                        // -49
	CASE(CL_INVALID_ARG_VALUE)                        // -50
	CASE(CL_INVALID_ARG_SIZE)                         // -51
	CASE(CL_INVALID_KERNEL_ARGS)                      // -52
	CASE(CL_INVALID_WORK_DIMENSION)                   // -53
	CASE(CL_INVALID_WORK_GROUP_SIZE)                  // -54
	CASE(CL_INVALID_WORK_ITEM_SIZE)                   // -55
	CASE(CL_INVALID_GLOBAL_OFFSET)                    // -56
	CASE(CL_INVALID_EVENT_WAIT_LIST)                  // -57
	CASE(CL_INVALID_EVENT)                            // -58
	CASE(CL_INVALID_OPERATION)                        // -59
	CASE(CL_INVALID_GL_OBJECT)                        // -60
	CASE(CL_INVALID_BUFFER_SIZE)                      // -61
	CASE(CL_INVALID_MIP_LEVEL)                        // -62
	CASE(CL_INVALID_GLOBAL_WORK_SIZE)                 // -63
#ifdef CL_VERSION_1_1
	CASE(CL_INVALID_PROPERTY)                         // -64
#endif //CL_VERSION_1_1
#ifdef CL_VERSION_1_2
	CASE(CL_INVALID_IMAGE_DESCRIPTOR)                 // -65
	CASE(CL_INVALID_COMPILER_OPTIONS)                 // -66
	CASE(CL_INVALID_LINKER_OPTIONS)                   // -67
	CASE(CL_INVALID_DEVICE_PARTITION_COUNT)           // -68
#endif //CL_VERSION_1_2

    default:
        return "(unrecognized error)";
    }

#undef CASE
}

void piglit_cl_expect_error(cl_int error, cl_int expected_error, enum piglit_result result)
{
	if (error == expected_error) {
		return;
	}

	/*
	 * If the lookup of the error's name is successful, then print
	 *     Unexpected CL error: NAME 0xHEX
	 * Else, print
	 *     Unexpected CL error: 0xHEX
	 */
	printf("Unexpected CL error: %s %d\n",
               piglit_cl_get_error_name(error), error);

	/* Print the expected error, but only if an error was really expected. */
	if (expected_error != CL_SUCCESS) {
		printf("Expected CL error: %s %d\n",
		       piglit_cl_get_error_name(expected_error),
		       expected_error);
	}

	piglit_report_result(result);
}

int piglit_cl_get_version(cl_platform_id platform) {
	char* version_string;
	const char *version_number_string;
	int scanf_count;
	int major;
	int minor;
	
	/*
	 * Returned format:
	 *   OpenCL<space><major_version.minor_version><space><platform-specific information>
	 */
	version_string = piglit_cl_get_platform_info(platform, CL_PLATFORM_VERSION);

	/* skip to version number */
	version_number_string = version_string + 6;

	/* Interpret version number */
	scanf_count = sscanf(version_number_string, "%i.%i", &major, &minor);
	if (scanf_count != 2) {
		printf("Unable to interpret CL_PLATFORM_VERSION string: %s\n",
		       version_string);
		piglit_report_result(PIGLIT_FAIL);
		exit(1);
	}
	return 10*major+minor;
}

void piglit_cl_require_version(cl_platform_id platform, int required_version_times_10)
{
	if (piglit_cl_get_version(platform) < required_version_times_10) {
		printf("Test requires OpenCL version %g\n",
		       required_version_times_10 / 10.0);
		piglit_report_result(PIGLIT_SKIP);
		exit(1);
	}
}

void* piglit_cl_get_platform_info(cl_platform_id platform, cl_platform_info param) {
	cl_int errNo;
	size_t param_size;
	/* All params are of type char[] */
	char* param_str = NULL;

	/* get param size */
	errNo = clGetPlatformInfo(platform, param, 0, NULL, &param_size);
	if(errNo == CL_SUCCESS) {
		param_str = calloc(param_size, sizeof(char));
		
		/* retrieve param */
		errNo = clGetPlatformInfo(platform, param, param_size, param_str, NULL);
		if(errNo != CL_SUCCESS) {
			free(param_str);
			param_str = NULL;
		}
	}

	if(param_str == NULL) {
		printf("Unable to get %s platform information (Error: %s)\n",
		       piglit_cl_get_enum_name(param),
		       piglit_cl_get_error_name(errNo));
		piglit_report_result(PIGLIT_FAIL);
		exit(1);
	}

	return param_str;
}

void* piglit_cl_get_device_info(cl_device_id device, cl_device_info param) {
	cl_int errNo;
	size_t param_size;
	void *param_data = NULL;

	/* get param size */
	errNo = clGetDeviceInfo(device, param, 0, NULL, &param_size);
	if(errNo == CL_SUCCESS) {
		param_data = calloc(param_size, sizeof(char));
		
		/* retrieve param */
		errNo = clGetDeviceInfo(device, param, param_size, param_data, NULL);
		if(errNo != CL_SUCCESS) {
			free(param_data);
			param_data = NULL;
		}
	}

	if(param_data == NULL) {
		printf("Unable to get %s device information (Error: %s)\n",
		       piglit_cl_get_enum_name(param),
		       piglit_cl_get_error_name(errNo));
		piglit_report_result(PIGLIT_FAIL);
		exit(1);
	}

	return param_data;
}

void* piglit_cl_get_context_info(cl_context context, cl_context_info param) {
	cl_int errNo;
	size_t param_size;
	void *param_data = NULL;

	/* get param size */
	errNo = clGetContextInfo(context, param, 0, NULL, &param_size);
	if(errNo == CL_SUCCESS) {
		param_data = calloc(param_size, sizeof(char));
		
		/* retrieve param */
		errNo = clGetContextInfo(context, param, param_size, param_data, NULL);
		if(errNo != CL_SUCCESS) {
			free(param_data);
			param_data = NULL;
		}
	}

	if(param_data == NULL) {
		printf("Unable to get %s context information (Error: %s)\n",
		       piglit_cl_get_enum_name(param),
		       piglit_cl_get_error_name(errNo));
		piglit_report_result(PIGLIT_FAIL);
		exit(1);
	}

	return param_data;
}

bool piglit_cl_is_platform_extension_supported(cl_platform_id platform, const char *name)
{
	char* extensions = piglit_cl_get_platform_info(platform, CL_PLATFORM_EXTENSIONS);
	bool supported = piglit_is_extension_in_string(extensions, name);

	free(extensions);

	return supported;
}

void piglit_cl_require_platform_extension(cl_platform_id platform, const char *name)
{
	if (!piglit_cl_is_platform_extension_supported(platform, name)) {
		printf("Test requires %s platform extension\n", name);
		piglit_report_result(PIGLIT_SKIP);
		exit(1);
	}
}

void piglit_cl_require_not_platform_extension(cl_platform_id platform, const char *name)
{
	if (piglit_cl_is_platform_extension_supported(platform, name)) {
		printf("Test requires absence of %s\n platform extension", name);
		piglit_report_result(PIGLIT_SKIP);
		exit(1);
	}
}

bool piglit_cl_is_device_extension_supported(cl_device_id device, const char *name)
{
	char* extensions = piglit_cl_get_device_info(device, CL_DEVICE_EXTENSIONS);
	bool supported = piglit_is_extension_in_string(extensions, name);

	free(extensions);

	return supported;
}

void piglit_cl_require_device_extension(cl_device_id device, const char *name)
{
	if (!piglit_cl_is_device_extension_supported(device, name)) {
		printf("Test requires %s device extension\n", name);
		piglit_report_result(PIGLIT_SKIP);
		exit(1);
	}
}

void piglit_cl_require_not_device_extension(cl_device_id device, const char *name)
{
	if (piglit_cl_is_device_extension_supported(device, name)) {
		printf("Test requires absence of %s device extension\n", name);
		piglit_report_result(PIGLIT_SKIP);
		exit(1);
	}
}
