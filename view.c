#include "matrix.h"

static float matrix[16];

static float width;
static float height;
static float z = 2.0f;

float *
view_matrix (void)
{
	return matrix;
}

static void
view_recalc (void)
{
	float aspect_ratio = (float)width / (float)height;
	float matrix_frustum[16];
	float matrix_translate[16];

	// Create frustum matrix:
	mat_frustum(matrix_frustum, 0.7, aspect_ratio, 0.5, 6);

	// Create frustum translation matrix:
	mat_translate(matrix_translate, 0, 0, z);

	// Combine into perspective matrix:
	mat_multiply(matrix, matrix_frustum, matrix_translate);
}

void
view_set_window (int _width, int _height)
{
	width = _width;
	height = _height;
	view_recalc();
}

void
view_z_decrease (void)
{
	if (z > 1.5f) {
		z -= 0.1f;
		view_recalc();
	}
}

void
view_z_increase (void)
{
	if (z < 5.0f) {
		z += 0.1f;
		view_recalc();
	}
}
