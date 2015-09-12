#include <math.h>

void
mat_frustum (float *matrix, float angle_of_view, float aspect_ratio, float z_near, float z_far)
{
	matrix[0] = 1.0f / tanf(angle_of_view);
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;
	matrix[4] = 0.0f;
	matrix[5] = aspect_ratio / tanf(angle_of_view);
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;
	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = (z_far + z_near) / (z_far - z_near);
	matrix[11] = 1.0f;
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = -2.0f * z_far * z_near / (z_far - z_near);
	matrix[15] = 0.0f;
}

void
mat_translate (float *matrix, float dx, float dy, float dz)
{
	matrix[0] = 1;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = 0;
	matrix[5] = 1;
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = 0;
	matrix[9] = 0;
	matrix[10] = 1;
	matrix[11] = 0;
	matrix[12] = dx;
	matrix[13] = dy;
	matrix[14] = dz;
	matrix[15] = 1;
}

void
mat_rotate (float *matrix, float theta)
{
	matrix[0] = 1.0;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = cos(theta);
	matrix[6] = sin(theta);
	matrix[7] = 0.0;
	matrix[8] = 0.0;
	matrix[9] = -sin(theta);
	matrix[10] = cos(theta);
	matrix[11] = 0.0;
	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = 0.0;
	matrix[15] = 1.0;
}

void
mat_multiply (float *matrix, float *a, float *b)
{
	float result[16];
	for (int c = 0; c < 4; c++) {
		for (int r = 0; r < 4; r++) {
			int index = c * 4 + r;
			float total = 0;
			for (int i = 0; i < 4; i++) {
				int p = i * 4 + r;
				int q = c * 4 + i;
				total += a[p] * b[q];
			}
			result[index] = total;
		}
	}
	for (int i = 0; i < 16; i++)
		matrix[i] = result[i];
}
