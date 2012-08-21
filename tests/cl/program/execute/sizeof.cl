/*!

[config]
name: Sizeof operator
clc_version_min: 10

dimensions: 1
global_size: 1 0 0

## sizeof ##

[test]
name: sizeof
kernel_name: so
arg_out: 0 buffer int[46] 1  1  2  2  4  4   8   8  4  2 \
                          2  2  4  4  8  8  16  16  8    \
                          4  4  8  8 16 16  32  32 16    \
                          8  8 16 16 32 32  64  64 32    \
                         16 16 32 32 64 64 128 128 64

!*/

kernel void so(global int* out) {
	out[0] = sizeof(char);
	out[1] = sizeof(uchar);
	out[2] = sizeof(short);
	out[3] = sizeof(ushort);
	out[4] = sizeof(int);
	out[5] = sizeof(uint);
	out[6] = sizeof(long);
	out[7] = sizeof(ulong);
	out[8] = sizeof(float);
	out[9] = sizeof(half);

	out[10] = sizeof(char2);
	out[11] = sizeof(uchar2);
	out[12] = sizeof(short2);
	out[13] = sizeof(ushort2);
	out[14] = sizeof(int2);
	out[15] = sizeof(uint2);
	out[16] = sizeof(long2);
	out[17] = sizeof(ulong2);
	out[18] = sizeof(float2);

	out[19] = sizeof(char4);
	out[20] = sizeof(uchar4);
	out[21] = sizeof(short4);
	out[22] = sizeof(ushort4);
	out[23] = sizeof(int4);
	out[24] = sizeof(uint4);
	out[25] = sizeof(long4);
	out[26] = sizeof(ulong4);
	out[27] = sizeof(float4);

	out[28] = sizeof(char8);
	out[29] = sizeof(uchar8);
	out[30] = sizeof(short8);
	out[31] = sizeof(ushort8);
	out[32] = sizeof(int8);
	out[33] = sizeof(uint8);
	out[34] = sizeof(long8);
	out[35] = sizeof(ulong8);
	out[36] = sizeof(float8);

	out[37] = sizeof(char16);
	out[38] = sizeof(uchar16);
	out[39] = sizeof(short16);
	out[40] = sizeof(ushort16);
	out[41] = sizeof(int16);
	out[42] = sizeof(uint16);
	out[43] = sizeof(long16);
	out[44] = sizeof(ulong16);
	out[45] = sizeof(float16);
}
