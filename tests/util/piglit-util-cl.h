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

#pragma once

#include <CL/opencl.h>

#include "piglit-util.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Runtime independent */

/**
 * \brief Convert a CL enum to a string.
 *
 * For example, given CL_PLATFORM_NAME, return "CL_PLATFORM_NAME".
 *
 * Return "(unrecognized enum)" if the enum is not recognized.
 *
 * \warning Not all enums are recognized. Bitfield and non-unique
 * enums will return "(unrecognized enum)"
 */
const char *piglit_cl_get_enum_name(cl_uint param);

/**
 * \brief Convert an CL error to a string.
 *
 * For example, given CL_DEVICE_NOT_FOUND, return "CL_DEVICE_NOT_FOUND".
 *
 * Return "(unrecognized error)" if the error is not recognized.
 */

const char* piglit_cl_get_error_name(cl_int error);

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
 * Returned version is multiplied by 10 to make it an integer.  So for
 * example, if the CL version is 1.1, the return value is 11.
 */
int piglit_cl_get_version(cl_platform_id platform);

/**
 * \brief Check for required OpenCL version and possibly terminate the test.
 *
 * \c required_version_times_10 mus be an OpenCL version multiplied by 10.
 * So for example, if the required CL version is 1.1, then the
 * \c required_version_times_10 should be 11.
 */
void piglit_cl_require_version(cl_platform_id platform, int required_version_times_10);

/**
 * \brief Get platform information.
 *
 * Returned data must be freed by the caller.
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
 * Returned data must be freed by the caller.
 */
void* piglit_cl_get_device_info(cl_device_id device, cl_device_info param);

/**
 * \brief Get context information.
 *
 * Returned data must be freed by the caller.
 */
void* piglit_cl_get_context_info(cl_context context, cl_context_info param);

/* Extensions */

/**
 * \brief Check if platform extension is supported
 *
 * \precondition name is not null
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
 * \precondition name is not null
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

#ifdef __cplusplus
} /* end extern "C" */
#endif
