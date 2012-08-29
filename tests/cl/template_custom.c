#include "piglit-framework-cl-custom.h"

PIGLIT_CL_CUSTOM_TEST_CONFIG_BEGIN

	config.name = "";

PIGLIT_CL_CUSTOM_TEST_CONFIG_END

enum piglit_result
piglit_cl_test(const int argc,
               const char** argv,
               const struct piglit_cl_custom_test_config* config,
               const struct piglit_cl_custom_test_env* env)
{
	enum piglit_result result = PIGLIT_PASS;

	return result;
}
