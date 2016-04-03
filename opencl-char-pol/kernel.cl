
__kernel void matrixMul(__global float* C, __global float* A, __global float* B, int n)
{
	int tx = get_global_id(0);
	int ty = get_global_id(1);

	float value = 0;
	for (int k = 0; k < n; ++k)
	{
		float elementA = A[ty * n + k];
		float elementB = B[k * n + tx];
		value += elementA * elementB;
	}

	C[ty * n + tx] = value;

}
