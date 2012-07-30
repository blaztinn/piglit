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

#include "piglit-framework-cl-api.h"

/* Function to run on each test */
//extern piglit_cl_test_t piglit_cl_test;

/* Default test configuration values */
const struct piglit_cl_api_test_config PIGLIT_CL_DEFAULT_API_TEST_CONFIG = {
	.version_min = 0,
	.version_max = 0,

	.create_context = false,

	.program_source = NULL,
	.build_options = NULL
};

/* Return default values for test configuration */
const void*
piglit_cl_get_empty_api_test_config()
{
	return &PIGLIT_CL_DEFAULT_API_TEST_CONFIG;
}

/* Run by piglit_cl_framework_run() */
enum piglit_result
piglit_cl_api_test_run(const int argc,
                       const char** argv,
                       void* void_config,
                       int version,
                       cl_platform_id platform_id,
                       cl_device_id device_id)
{
	enum piglit_result result;

	struct piglit_cl_api_test_config* config = void_config;
	struct piglit_cl_api_test_env env;

	struct piglit_cl_context context;
	cl_program program = NULL;

	/* Check that config is valid */
	// version_min
	if(config->version_min <= 0) {
		printf("Invalid configuration, version_min is %d.\n",
		       config->version_min);
		return PIGLIT_SKIP;
	}
	if(config->version_min > PIGLIT_CL_VERSION) {
		printf("Piglit was compiled with lower OpenCL version (%d.%d) than version_min: %d.\n",
		       PIGLIT_CL_VERSION/10, PIGLIT_CL_VERSION%10,
		       config->version_min);
		return PIGLIT_SKIP;
	}
	// version_max
	if(config->version_max < 0) {
		printf("Invalid configuration, version_max is %d.\n",
		       config->version_max);
		return PIGLIT_SKIP;
	}
	// create_context
	if(config->create_context && !(config->run_per_device || config->run_per_platform)) {
		printf("Invalid configuration, create_context can only be used with run_per_platform or run_per_device.\n");
		return PIGLIT_SKIP;
	}
	// program
	if(config->program_source != NULL && !(config->run_per_device || config->run_per_platform)) {
		printf("Invalid configuration, program_source can only be used with run_per_platform or run_per_device.\n");
		return PIGLIT_SKIP;
	}
	if(config->program_source != NULL && !config->create_context) {
		config->create_context = true;
	}
	// build options
	if(config->build_options != NULL && config->program_source == NULL) {
		printf("Invalid configuration, build_options can only be used with program_source.\n");
		return PIGLIT_SKIP;
	}

	/* Check version to test against */
	if(version < config->version_min) {
		printf("Trying to run test with version (%d.%d) lower than version_min: %d\n",
		       version/10, version%10,
		       config->version_min);
		return PIGLIT_SKIP;
	}
	if(config->version_max > 0 && version > config->version_max) {
		/*
		 * If version was not provided on the command line
		 * lower it to version_max.
		 */
		if(piglit_cl_get_version_arg(argc, argv) == 0) {
			printf("Lowering version to %d.%d because of version_max.\n",
			       config->version_max/10, config->version_max%10);
			version = config->version_max > config->version_max;
		} else {
			printf("Trying to run test with version (%d.%d) higher than version_max: %d\n",
			       version/10, version%10,
			       config->version_max);
			return PIGLIT_SKIP;
		}
	}

	/* Create context */
	if(config->create_context) {
		bool success;

		if(config->run_per_platform) {
			unsigned int num_devices;
			cl_device_id* device_ids;

			num_devices = piglit_cl_get_device_ids(platform_id, CL_DEVICE_TYPE_ALL, &device_ids);

			success = piglit_cl_create_context(&context, platform_id, device_ids, num_devices);

			free(device_ids);
		} else { // config->run_per_device
			success = piglit_cl_create_context(&context, platform_id, &device_id, 1);
		}

		if(!success) {
			return PIGLIT_FAIL;
		}
	}
	
	/* Create and build program */
	if(config->program_source != NULL) {
		if(config->build_options != NULL) {
			program = piglit_cl_build_program_with_source(context, 1, &config->program_source, config->build_options);
		} else {
			program = piglit_cl_build_program_with_source(context, 1, &config->program_source, "");
		}
	}


	/* Set environment */
	env.platform_id = platform_id;
	env.device_id = device_id;
	env.context = context;
	env.version = version;
	env.program = program;


	/* Run the actual test */
	result = config->_api_test(argc, argv, config, &env);


	/* Release program */
	if(config->program_source != NULL) {
		clReleaseProgram(program);
	}
	
	/* Release context */
	if(config->create_context) {
		piglit_cl_release_context(&context);
	}

	return result;
}
