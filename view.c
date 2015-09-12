#include "matrix.h"

static float matrix[16];

float *
view_matrix (void)
{
	return matrix;
}

void
view_recalc (int width, int height)
{
	float aspect_ratio = (float)width / (float)height;
	float matrix_frustum[16];
	float matrix_translate[16];

	// Create frustum matrix:
	mat_frustum(matrix_frustum, 0.7, aspect_ratio, 1, 3);

	// Create frustum translation matrix:
	mat_translate(matrix_translate, 0, 0, 2);

	// Combine into perspective matrix:
	mat_multiply(matrix, matrix_frustum, matrix_translate);
}
