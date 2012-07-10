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

#pragma once
#ifndef PIGLIT_UTIL_CL_H
#define PIGLIT_UTIL_CL_H

#include <CL/opencl.h>

#include "piglit-util.h"
#include "piglit-util-cl-enum.h"


/* Define with which version of OpenCL Piglit was compiled */

#if defined(CL_VERSION_1_2)
#  define PIGLIT_CL_VERSION 12
#elif defined(CL_VERSION_1_1)
#  define PIGLIT_CL_VERSION 11
#else
#  define PIGLIT_CL_VERSION 10
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Runtime independent */

/**
 * \brief Check for unexpected GL error and report it.
 *
 * If \c error is other than \c expected_error, then print a diagnostic.
 *
 * If you expect no error, then set \code expected_error = CL_SUCCESS \endcode.
 *
 * Returns true if \c error and \c expected_error are the same,
 * else it returns false.
 */
bool piglit_cl_check_error(cl_int error, cl_int expected_error);

/**
 * \brief Check for unexpected GL error and possibly terminate the test.
 *
 * If \c error is other than \c expected_error, then print a diagnostic
 * and terminate the test with the given \c result.
 *
 * If you expect no error, then set \code expected_error = CL_SUCCESS \endcode.
 */
void piglit_cl_expect_error(cl_int error, cl_int expected_error, enum piglit_result result);

/* Runtime dependent */

/* Platform, device and context info */

/**
 * \brief Get version of OpenCL API for \c platform.
 *
 * Returned version is multiplied by 10 to make it an integer. For
 * example, if the CL version is 1.1, the returned ivalue is 11.
 */
int piglit_cl_get_platform_version(cl_platform_id platform);

/**
 * \brief Check for required OpenCL version and possibly terminate the test.
 *
 * \c required_version_times_10 must be an OpenCL version multiplied by 10.
 * For example, if the required CL version is 1.1, then the
 * \c required_version_times_10 should be 11.
 */
void piglit_cl_require_platform_version(cl_platform_id platform, int required_version_times_10);

/**
 * \brief Get platform information.
 *
 * \warning Returned data must be freed by the caller.
 *
 * \note
 * Although the returned types of all params are of type char all through
 * the last version of OpenCL (1.2 as of time of writing), the return type
 * of this function is void* for future compatibility.
 */
void* piglit_cl_get_platform_info(cl_platform_id platform, cl_platform_info param);

/**
 * \brief Get device information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_device_info(cl_device_id device, cl_device_info param);

/**
 * \brief Get context information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_context_info(cl_context context, cl_context_info param);

/**
 * \brief Get command queue information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_command_queue_info(cl_command_queue command_queue, cl_command_queue_info param);

/**
 * \brief Get memory object information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_mem_object_info(cl_mem mem_obj, cl_mem_info param);

/**
 * \brief Get image information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_image_info(cl_mem image, cl_image_info param);

/**
 * \brief Get sampler information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_sampler_info(cl_sampler sampler, cl_sampler_info param);

/**
 * \brief Get program information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_program_info(cl_program program, cl_program_info param);

/**
 * \brief Get program build information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_program_build_info(cl_program program, cl_device_id device, cl_program_build_info param);

/**
 * \brief Get kernel information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_kernel_info(cl_kernel kernel, cl_mem_info param);

/**
 * \brief Get kernel work group information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_kernel_work_group_info(cl_kernel kernel, cl_device_id device, cl_mem_info param);

/**
 * \brief Get event information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_event_info(cl_event event, cl_event_info param);

/**
 * \brief Get profiling information.
 *
 * \warning Returned data must be freed by the caller.
 */
void* piglit_cl_get_event_profiling_info(cl_event event, cl_profiling_info param);

/* Extensions */

/**
 * \brief Check if platform extension is supported
 *
 * \pre name is not null
 */
bool piglit_cl_is_platform_extension_supported(cl_platform_id platform, const char *name);

/**
 * \brief Check for required OpenCL platform extension and possibly
 * terminate the test.
 */
void piglit_cl_require_platform_extension(cl_platform_id platform, const char *name);

/**
 * \brief Check for not required OpenCL platform extension and possibly
 * terminate the test.
 */
void piglit_cl_require_not_platform_extension(cl_platform_id platform, const char *name);

/**
 * \brief Check if device extension is supported
 *
 * \pre name is not null
 */
bool piglit_cl_is_device_extension_supported(cl_device_id device, const char *name);

/**
 * \brief Check for required OpenCL device extension and possibly
 * terminate the test.
 */
void piglit_cl_require_device_extension(cl_device_id device, const char *name);

/**
 * \brief Check for not required OpenCL device extension and possibly
 * terminate the test.
 */
void piglit_cl_require_not_device_extension(cl_device_id device, const char *name);

/**
 * \brief Get all available platforms.
 *
 * \warning Caller must free the allocated platform array.
 *
 * @param platform_ids  Address to store a pointer to platform ids list.
 * @return              Number of stored platform ids.
 */
unsigned int piglit_cl_get_platform_ids(cl_platform_id** platform_ids);

/**
 * \brief Get all available devices on platform \c platform_id.
 *
 * \warning Caller must free the allocated device array.
 *
 * @param platform_id  Platform from which to get platforms.
 * @param device_ids   Address to store a pointer to device ids list.
 * @return             Number of stored device ids.
 */
unsigned int piglit_cl_get_device_ids(cl_platform_id platform_id, cl_device_id** device_ids);

/* Contexts */

/**
 * \brief Helper context.
 *
 * Helper context struct for easier OpenCL context manipulation.
 */
struct piglit_cl_context {
	cl_platform_id platform_id; /**< Platform used to create context. */
	cl_context cl_ctx; /**< OpenCL context. */

	unsigned int num_devices; /**< Number of members in \c device_ids and \c command_queues */

	cl_device_id* device_ids; /**< Device ids available in the context. */
	cl_command_queue* command_queues; /**< Command queues available in the context.
	                                       Each command queue is assigned to device id in \c device_ids with the same index. */
};

/**
 * \brief Create \c piglit_cl_context
 *
 * Create a helper context from platform id \c platform_id and
 * device ids \c device_ids.
 *
 * @param platform_id  Platform from which to create context.
 * @param device_ids   Device ids to add to context.
 * @param num_devices  Number of members in \c device_ids.
 * @return             New piglit_cl_context.
 */
struct piglit_cl_context
piglit_cl_create_context(cl_platform_id platform_id, const cl_device_id device_ids[], unsigned int num_devices);

/**
 * \brief Release \c piglit_cl_context
 *
 * Free memory used by \c context and release the generated context
 * and memory queues.
 *
 * @param context  Helper context to release.
 */
void
piglit_cl_release_context(struct piglit_cl_context context);

/**
 * \brief Create and build program
 *
 * Create and build program for all devices in \c piglit_cl_context.
 *
 * @param context      Context on which to create and build program.
 * @param count        Number of strings in \c strings.
 * @param string       Array of pointers to NULL-terminated source strings.
 * @param options      NULL-terminated string that describes build options.
 * @return             New piglit_cl_context.
 */
cl_program
piglit_cl_build_program_with_source(struct piglit_cl_context context, cl_uint count, char** strings, const char* options);


#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif //PIGLIT_UTIL_CL_H
