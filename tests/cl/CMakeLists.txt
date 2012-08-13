include_directories(
	${OPENCL_INCLUDE_PATH}
	${piglit_SOURCE_DIR}/tests/util
)

link_libraries (
	piglitutil_${piglit_target_api}
	${OPENCL_opencl_LIBRARY}
)

function(piglit_cl_add_test)
    add_executable(${ARGV})
endfunction(piglit_cl_add_test)

function(piglit_cl_add_api_test name)

    list(REMOVE_AT ARGV 0)
    piglit_cl_add_test(
        cl-api-${name}
        ${ARGV}
    )

endfunction(piglit_cl_add_api_test)

add_subdirectory (api)
