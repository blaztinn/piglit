// [config]
// expect_result: fail
// glsl_version: 1.00
// [end config]
//
// From section 4.3.5 of the GLSL 1.00 spec:
//     A fragment shader can not write to a varying variable.

#version 100

varying float x;

void g() {
    x = 0.0;
}
