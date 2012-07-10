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

#include "piglit-util-cl.h"


bool piglit_cl_check_error(cl_int error, cl_int expected_error)
{
	if (error == expected_error) {
		return true;
	}

	/*
	 * If the lookup of the error's name is successful, then print
	 *     Unexpected CL error: NAME DEC
	 * Else, print
	 *     Unexpected CL error: DEC
	 */
	printf("Unexpected CL error: %s %d\n",
	       piglit_cl_get_error_name(error), error);

	/* Print the expected error, but only if an error was really expected. */
	if (expected_error != CL_SUCCESS) {
		printf("Expected CL error: %s %d\n",
		        piglit_cl_get_error_name(expected_error),
		        expected_error);
	}

	return false;
}

void piglit_cl_expect_error(cl_int error, cl_int expected_error, enum piglit_result result)
{
	if(!piglit_cl_check_error(error, expected_error)) {
		piglit_report_result(result);
	}
}

int piglit_cl_get_platform_version(cl_platform_id platform)
{
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

void piglit_cl_require_platform_version(cl_platform_id platform, int required_version_times_10)
{
	if (piglit_cl_get_platform_version(platform) < required_version_times_10) {
		printf("Test requires OpenCL version %g\n",
		       required_version_times_10 / 10.0);
		piglit_report_result(PIGLIT_SKIP);
		exit(1);
	}
}

struct _program_build_info_args {
	cl_program program;
	cl_device_id device;
};
struct _kernel_work_group_info_args {
	cl_kernel kernel;
	cl_device_id device;
};

static void* piglit_cl_get_info(void* fn_ptr, void* obj, cl_uint param) {
	cl_int errNo;
	size_t param_size;
	void* param_ptr = NULL;

	/* get param size */
	if(fn_ptr == clGetPlatformInfo) {
		errNo = clGetPlatformInfo(*(cl_platform_id*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetDeviceInfo) {
		errNo = clGetDeviceInfo(*(cl_device_id*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetContextInfo) {
		errNo = clGetContextInfo(*(cl_context*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetCommandQueueInfo) {
		errNo = clGetCommandQueueInfo(*(cl_command_queue*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetMemObjectInfo) {
		errNo = clGetMemObjectInfo(*(cl_mem*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetImageInfo) {
		errNo = clGetImageInfo(*(cl_mem*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetSamplerInfo) {
		errNo = clGetSamplerInfo(*(cl_sampler*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetProgramInfo) {
		errNo = clGetProgramInfo(*(cl_program*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetProgramBuildInfo) {
		errNo = clGetProgramBuildInfo(((struct _program_build_info_args*)obj)->program,
		                              ((struct _program_build_info_args*)obj)->device,
		                              param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetKernelInfo) {
		errNo = clGetKernelInfo(*(cl_kernel*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetKernelWorkGroupInfo) {
		errNo = clGetKernelWorkGroupInfo(((struct _kernel_work_group_info_args*)obj)->kernel,
		                                 ((struct _kernel_work_group_info_args*)obj)->device,
		                                 param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetEventInfo) {
		errNo = clGetEventInfo(*(cl_event*)obj, param, 0, NULL, &param_size);
	} else if(fn_ptr == clGetEventProfilingInfo) {
		errNo = clGetEventProfilingInfo(*(cl_event*)obj, param, 0, NULL, &param_size);
	} else {
		printf("Trying to get %s information from undefined function.\n",
		       piglit_cl_get_enum_name(param));
		piglit_report_result(PIGLIT_FAIL);
		exit(1);
	}

	if(errNo == CL_SUCCESS) {
		param_ptr = calloc(param_size, sizeof(char));
		
		/* retrieve param */
		if(fn_ptr == clGetPlatformInfo) {
			errNo = clGetPlatformInfo(*(cl_platform_id*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetDeviceInfo) {
			errNo = clGetDeviceInfo(*(cl_device_id*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetContextInfo) {
			errNo = clGetContextInfo(*(cl_context*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetCommandQueueInfo) {
			errNo = clGetCommandQueueInfo(*(cl_command_queue*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetMemObjectInfo) {
			errNo = clGetMemObjectInfo(*(cl_mem*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetImageInfo) {
			errNo = clGetImageInfo(*(cl_mem*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetSamplerInfo) {
			errNo = clGetSamplerInfo(*(cl_sampler*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetProgramInfo) {
			errNo = clGetProgramInfo(*(cl_program*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetProgramBuildInfo) {
			errNo = clGetProgramBuildInfo(((struct _program_build_info_args*)obj)->program,
			                              ((struct _program_build_info_args*)obj)->device,
			                              param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetKernelInfo) {
			errNo = clGetKernelInfo(*(cl_kernel*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetKernelWorkGroupInfo) {
			errNo = clGetKernelWorkGroupInfo(((struct _kernel_work_group_info_args*)obj)->kernel,
			                                 ((struct _kernel_work_group_info_args*)obj)->device,
			                                 param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetEventInfo) {
			errNo = clGetEventInfo(*(cl_event*)obj, param, param_size, param_ptr, NULL);
		} else if(fn_ptr == clGetEventProfilingInfo) {
			errNo = clGetEventProfilingInfo(*(cl_event*)obj, param, param_size, param_ptr, NULL);
		}

		if(errNo != CL_SUCCESS) {
			free(param_ptr);
			param_ptr = NULL;
		}
	}

	if(param_ptr == NULL) {
		printf("Unable to get %s information (Error: %s)\n",
		       piglit_cl_get_enum_name(param),
		       piglit_cl_get_error_name(errNo));
		piglit_report_result(PIGLIT_FAIL);
		exit(1);
	}

	return param_ptr;
}

void* piglit_cl_get_platform_info(cl_platform_id platform, cl_platform_info param) {
	return piglit_cl_get_info(clGetPlatformInfo, &platform, param);
}

void* piglit_cl_get_device_info(cl_device_id device, cl_device_info param) {
	return piglit_cl_get_info(clGetDeviceInfo, &device, param);
}

void* piglit_cl_get_context_info(cl_context context, cl_context_info param) {
	return piglit_cl_get_info(clGetContextInfo, &context, param);
}

void* piglit_cl_get_command_queue_info(cl_command_queue command_queue, cl_command_queue_info param) {
	return piglit_cl_get_info(clGetCommandQueueInfo, &command_queue, param);
}

void* piglit_cl_get_mem_object_info(cl_mem mem_obj, cl_mem_info param) {
	return piglit_cl_get_info(clGetMemObjectInfo, &mem_obj, param);
}

void* piglit_cl_get_image_info(cl_mem image, cl_image_info param) {
	return piglit_cl_get_info(clGetImageInfo, &image, param);
}

void* piglit_cl_get_sampler_info(cl_sampler sampler, cl_sampler_info param) {
	return piglit_cl_get_info(clGetSamplerInfo, &sampler, param);
}

void* piglit_cl_get_program_info(cl_program program, cl_program_info param) {
	return piglit_cl_get_info(clGetProgramInfo, &program, param);
}

void* piglit_cl_get_program_build_info(cl_program program, cl_device_id device, cl_program_build_info param) {
	struct _program_build_info_args args = {
		.program = program,
		.device = device
	};
	
	return piglit_cl_get_info(clGetProgramBuildInfo, &args, param);
}

void* piglit_cl_get_kernel_info(cl_kernel kernel, cl_mem_info param) {
	return piglit_cl_get_info(clGetKernelInfo, &kernel, param);
}

void* piglit_cl_get_kernel_work_group_info(cl_kernel kernel, cl_device_id device, cl_mem_info param) {
	struct _kernel_work_group_info_args args = {
		.kernel = kernel,
		.device = device
	};
	
	return piglit_cl_get_info(clGetKernelWorkGroupInfo, &args, param);
}

void* piglit_cl_get_event_info(cl_event event, cl_event_info param) {
	return piglit_cl_get_info(clGetEventInfo, &event, param);
}

void* piglit_cl_get_event_profiling_info(cl_event event, cl_profiling_info param) {
	return piglit_cl_get_info(clGetEventProfilingInfo, &event, param);
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

unsigned int piglit_cl_get_platform_ids(cl_platform_id** platform_ids)
{
	cl_int errNo;
	cl_uint num_platform_ids;

	/* get number of platforms */
	errNo = clGetPlatformIDs(0, NULL, &num_platform_ids);
	if(errNo != CL_SUCCESS) {
		printf("Could not get number of platforms: %s\n",
		       piglit_cl_get_error_name(errNo));
		piglit_report_result(PIGLIT_FAIL);
	}

	/* get platform list */
	if(platform_ids != NULL && num_platform_ids > 0) {
		*platform_ids = malloc(num_platform_ids * sizeof(cl_platform_id));
		errNo = clGetPlatformIDs(num_platform_ids, *platform_ids, NULL);
		if(errNo != CL_SUCCESS) {
			printf("Could not get get platform list: %s\n",
			       piglit_cl_get_error_name(errNo));
			piglit_report_result(PIGLIT_FAIL);
		}
	}

	return num_platform_ids;
}

unsigned int piglit_cl_get_device_ids(cl_platform_id platform_id, cl_device_id** device_ids)
{
	cl_int errNo;
	cl_uint num_device_ids;
	cl_uint num_platform_ids;
	cl_platform_id *platform_ids;
	int i;

	/* get platform_ids */
	num_platform_ids = piglit_cl_get_platform_ids(&platform_ids);

	/* find the right platform */
	for(i = 0; i < num_platform_ids; i++) {
		if(platform_ids[i] == platform_id) {
			/* get number of devices */
			errNo = clGetDeviceIDs(platform_id,
			                       CL_DEVICE_TYPE_ALL,
			                       0,
			                       NULL,
			                       &num_device_ids);
			if(errNo != CL_SUCCESS) {
				free(platform_ids);
				printf("Could not get number of devices: %s\n",
				       piglit_cl_get_error_name(errNo));
				piglit_report_result(PIGLIT_FAIL);
			}
		
			/* get device list */
			if(device_ids != NULL && num_device_ids > 0) {
				*device_ids = malloc(num_device_ids * sizeof(cl_device_id));
				errNo = clGetDeviceIDs(platform_id,
				                       CL_DEVICE_TYPE_ALL,
				                       num_device_ids,
				                       *device_ids,
				                       NULL);
				if(errNo != CL_SUCCESS) {
					free(platform_ids);
					printf("Could not get get device list: %s\n",
					       piglit_cl_get_error_name(errNo));
					piglit_report_result(PIGLIT_FAIL);
				}
			}

			free(platform_ids);

			return num_device_ids;
		}
	}

	free(platform_ids);

	/* received invalid platform_id */
	printf("Trying to get a device from invalid platform_id\n");
	piglit_report_result(PIGLIT_FAIL);

	/* UNREACHED */
	return 0;
}

struct piglit_cl_context
piglit_cl_create_context(cl_platform_id platform_id, const cl_device_id device_ids[], unsigned int num_devices)
{
	int i;
	cl_int errNo;
	struct piglit_cl_context context;
	cl_context_properties cl_ctx_properties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id,
		0
	};

	/* assign platform */
	context.platform_id = platform_id;

	/* assign devices */
	context.num_devices = num_devices;
	context.device_ids = malloc(num_devices * sizeof(cl_device_id));
	memcpy(context.device_ids, device_ids, num_devices * sizeof(cl_device_id));

	/* create and assign context */
	context.cl_ctx = clCreateContext(cl_ctx_properties,
	                                 context.num_devices,
	                                 context.device_ids,
	                                 NULL,
	                                 NULL,
	                                 &errNo);
	if(errNo != CL_SUCCESS) {
		printf("Could not create context: %s\n",
		       piglit_cl_get_error_name(errNo));
		piglit_report_result(PIGLIT_FAIL);
	}

	/* create and assing command queues */
	context.command_queues = malloc(num_devices * sizeof(cl_command_queue));
	for(i = 0; i < num_devices; i++) {
		context.command_queues[i] = clCreateCommandQueue(context.cl_ctx,
		                                                 context.device_ids[i],
		                                                 0,
		                                                 &errNo);
		if(errNo != CL_SUCCESS) {
			printf("Could not create command queue: %s\n",
			       piglit_cl_get_error_name(errNo));
			piglit_report_result(PIGLIT_FAIL);
		}
	}

	return context;
}

void piglit_cl_release_context(struct piglit_cl_context context)
{
	int i;

	/* release command queues */
	for(i = 0; i < context.num_devices; i++) {
		if(clReleaseCommandQueue(context.command_queues[i]) != CL_SUCCESS) {
			printf("Command queue already released\n");
		}
	}

	/* free devices array */
	free(context.device_ids);

	/* release context */
	if(clReleaseContext(context.cl_ctx) != CL_SUCCESS) {
		printf("Context already released\n");
	}
}

cl_program
piglit_cl_build_program_with_source(struct piglit_cl_context context, cl_uint count, char** strings, const char* options)
{
	cl_int errNo;
	cl_program program;

	program = clCreateProgramWithSource(context.cl_ctx,
	                                    count,
	                                    (const char**)strings,
	                                    NULL,
	                                    &errNo);
	if(errNo != CL_SUCCESS) {
		printf("Could not create program with source: %s\n",
		       piglit_cl_get_error_name(errNo));
		piglit_report_result(PIGLIT_FAIL);
	}
	
	errNo = clBuildProgram(program,
	                       context.num_devices,
	                       context.device_ids,
	                       NULL,
	                       NULL,
	                       NULL);
	if(errNo != CL_SUCCESS) {
		int i;
		
		printf("Could not build program: %s\n",
		       piglit_cl_get_error_name(errNo));

		printf("Source:\n");
		for(i = 0; i < count; i++) {
			printf("%s\n", strings[i]);
		}

		for(i = 0; i < context.num_devices; i++) {
			char* device_name = piglit_cl_get_device_info(context.device_ids[i], CL_DEVICE_NAME);
			char* log = piglit_cl_get_program_build_info(program, context.device_ids[i], CL_PROGRAM_BUILD_LOG);
			
			printf("Build log for %s:\n ---- \n%s\n ---- \n",
			       device_name,
			       log);
			
			free(device_name);
			free(log);
		}
		
		piglit_report_result(PIGLIT_FAIL);
	}

	return program;
}
