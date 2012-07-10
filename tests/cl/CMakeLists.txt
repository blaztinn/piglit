include_directories(
	${OPENCL_INCLUDE_PATH}
	${piglit_SOURCE_DIR}/tests/util
	${piglit_SOURCE_DIR}/tests/cl/util
)

link_libraries (
	piglitutil_${piglit_target_api}
	${OPENCL_opencl_LIBRARY}
)

function(piglit_cl_add_test name source)

    list(REMOVE_AT ARGV 0)
    add_executable(
        cl-api-${name}
        ${ARGV}
    )

endfunction(piglit_cl_add_test)

add_subdirectory (api)
