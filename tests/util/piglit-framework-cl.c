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

#include "piglit-framework-cl.h"

/* Default test header configuration values */
const struct piglit_cl_test_config_header PIGLIT_CL_DEFAULT_TEST_CONFIG_HEADER = {
	._filename = "",
	.name = NULL,

	.run_per_platform = false,
	.run_per_device = false,
};


/* Print test configuration */
static void
print_test_info(const struct piglit_cl_test_config_header* config,
                int version,
                const cl_platform_id platform_id,
                const cl_device_id device_id) {
	if(config->run_per_platform || config->run_per_device) {
		char* platform_name;

		platform_name = piglit_cl_get_platform_info(platform_id, CL_PLATFORM_NAME);

		printf("Running on:\n  Platform: %s\n",
		       platform_name);

		if(config->run_per_device) {
			char* device_name = piglit_cl_get_device_info(device_id, CL_DEVICE_NAME);

			printf("  Device: %s\n", device_name);

			free(device_name);
		}

		printf("  Version: %d.%d\n", version/10, version%10);

		free(platform_name);
	} else {
		printf("Running test:\n");
	}
}


int piglit_cl_framework_run(int argc, char** argv)
{
	enum piglit_result result = PIGLIT_SKIP;
	int version = 0;
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;

	/* Get test configuration */
	struct piglit_cl_test_config_header *config =
	    piglit_cl_get_test_config(&PIGLIT_CL_DEFAULT_TEST_CONFIG_HEADER);

	/* Print test name and file */
	if(config->name != NULL) {
		printf("Test: %s (%s)\n", config->name, config->_filename);
	} else {
		printf("Test: (%s)\n", config->_filename);
	}

	/* Check that config is valid */
	// run_per_platform, run_per_device
	if(config->run_per_platform && config->run_per_device) {
		printf("Invalid configuration, only one of run_per_platform and run_per_device can be true.\n");
		piglit_report_result(PIGLIT_SKIP);
	}
	
	/* Get version to test against */
	version = piglit_cl_get_version_arg(argc, (const char **)argv);
	if(version > 0) {
		if(version > PIGLIT_CL_VERSION) {
			printf("Piglit was compiled with lower OpenCL version (%d.%d) than version argument: %d.%d.\n",
			       PIGLIT_CL_VERSION/10, PIGLIT_CL_VERSION%10,
			       version/10, version%10);
			piglit_report_result(PIGLIT_SKIP);
		}
	} else {
		/*
		 * If version was not provided on the command line, set it to
		 * the version against which Piglit was compiled (PIGLIT_CL_VERSION)
		 */
		version = PIGLIT_CL_VERSION;
	}

	/* Run the actual test */
	if(!(config->run_per_platform || config->run_per_device)) {
		print_test_info(config, version, NULL, NULL);
		result = config->_test_run(argc, (const char**)argv, (void*)config, version, NULL, NULL);
	} else {
		/* Run tests per platform or device */
		int i;

		bool platform_defined;
		bool device_defined;

		unsigned int num_platforms;
		cl_platform_id* platform_ids;

		/* check for command-line platform argument */
		platform_defined = piglit_cl_get_platform_arg(argc, (const char**)argv, &platform_id);

		/* generate platforms list */
		if(platform_defined) {
			/* use platform defined by command-line argument */
			num_platforms = 1;
			platform_ids = malloc(sizeof(cl_platform_id));
			platform_ids[0] = platform_id;
		} else {
			/* use all available platforms */
			num_platforms = piglit_cl_get_platform_ids(&platform_ids);
		}

		/* execute test for each platform in platforms list */
		for(i = 0; i < num_platforms; i++) {
			int platform_version;

			platform_id = platform_ids[i];
			
			/* Check platform version */
			platform_version = piglit_cl_get_platform_version(platform_id);
			if(platform_version < version) {
				printf("  Platform supporting only version %d.%d. Running test on that version.\n",
				       platform_version/10, platform_version%10);
				version = platform_version;
			}

			/* check for command-line device argument */
			device_defined = piglit_cl_get_device_arg(argc, (const char**)argv, platform_id, &device_id);

			if(device_defined) {
				/* use device defined by command-line argument */
				print_test_info(config, version, platform_id, device_id);
				piglit_merge_result(&result,
				                    config->_test_run(argc, (const char**)argv, (void*)config, version, platform_id, device_id));
			} else {
				/* use all available devices */
				unsigned int num_devices;
				cl_device_id* device_ids;

				/* get devices list available on platform */
				num_devices = piglit_cl_get_device_ids(platform_id, &device_ids);

				if(config->run_per_device) {
					unsigned int j;
					/* run tests per each device */
					for(j = 0; j < num_devices; j++) {
						device_id = device_ids[j];

						print_test_info(config, version, platform_id, device_id);
						piglit_merge_result(&result,
						                    config->_test_run(argc, (const char**)argv, (void*)config, version, platform_id, device_id));
					}
				} else {
					/* run test per each platform */
					print_test_info(config, version, platform_id, NULL);
					piglit_merge_result(&result,
					                    config->_test_run(argc, (const char**)argv, (void*)config, version, platform_id, NULL));
				}

				free(device_ids);
			}
		}

		free(platform_ids);
	}

	/* Report merged result */
	piglit_report_result(result);

	/* UNREACHED */
	return 1;
}


const char*
piglit_cl_get_arg_value(const int argc, const char *argv[], const char* arg)
{
	int i;
	char* full_arg = calloc(strlen(arg) + 2, sizeof(char));
	full_arg = strcpy(full_arg, "-");
	full_arg = strcat(full_arg, arg);

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], full_arg)) {
			if ((i+1) >= argc) {
				fprintf(stderr,
				        "Argument error: %s requires an argument\n",
				        full_arg);
				free(full_arg);
				piglit_report_result(PIGLIT_FAIL);
			} else {
				free(full_arg);
				return argv[i+1];
			}
		}
	}

	free(full_arg);
	return NULL;
}

bool
piglit_cl_is_arg_defined(const int argc, const char *argv[], const char* arg)
{
	int i;
	char* full_arg = calloc(strlen(arg) + 2, sizeof(char));
	full_arg = strcpy(full_arg, "-");
	full_arg = strcat(full_arg, arg);

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], full_arg)) {
			free(full_arg);
			return true;
		}
	}

	free(full_arg);
	return false;
}

int piglit_cl_get_version_arg(int argc, const char** argv)
{
	int version_major = 0;
	int version_minor = 0;

	const char* version_str = piglit_cl_get_arg_value(argc, argv, "version");

	if(version_str != NULL) {
		if(sscanf(version_str, "%i.%i", &version_major, &version_minor) != 2) {
			version_major = 0;
			version_minor = 0;
		}
	}

	return version_major*10 + version_minor;
}

bool piglit_cl_get_platform_arg(const int argc, const char** argv, cl_platform_id* platform_id)
{
	int i;
	const char* arg_value = piglit_cl_get_arg_value(argc, argv, "platform");

	if(arg_value != NULL) {
		unsigned int num_platforms;
		cl_platform_id* platform_ids;

		num_platforms = piglit_cl_get_platform_ids(&platform_ids);

		for(i = 0; i < num_platforms; i++) {
			char* platform_name = piglit_cl_get_platform_info(platform_ids[i], CL_PLATFORM_NAME);

			if(!strncmp(arg_value, platform_name, strlen(arg_value))) {
				*platform_id = platform_ids[i];

				free(platform_ids);
				free(platform_name);
				return true;
			}

			free(platform_name);
		}

		free(platform_ids);
		fprintf(stderr,
		        "Could not find platform: %s\n",
		        arg_value);
		piglit_report_result(PIGLIT_SKIP);
	}

	return false;
}

bool piglit_cl_get_device_arg(const int argc, const char** argv, cl_platform_id platform_id, cl_device_id* device_id)
{
	int i;
	const char* arg_value = piglit_cl_get_arg_value(argc, argv, "device");

	if(arg_value != NULL) {
		unsigned int num_devices;
		cl_device_id* device_ids;

		num_devices = piglit_cl_get_device_ids(platform_id, &device_ids);

		for(i = 0; i < num_devices; i++) {
			char* device_name = piglit_cl_get_device_info(device_ids[i], CL_DEVICE_NAME);

			if(!strncmp(arg_value, device_name, strlen(arg_value))) {
				*device_id = device_ids[i];

				free(device_ids);
				free(device_name);
				return true;
			}

			free(device_name);
		}

		free(device_ids);
		fprintf(stderr,
		        "Could not find device: %s\n",
		        arg_value);
		piglit_report_result(PIGLIT_SKIP);
	}

	return false;
}
