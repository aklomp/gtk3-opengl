#include "matrix.h"

static struct {
	float matrix[16];
	float width;
	float height;
	float z;
}
state = {
	.z = 2.0f,
};

const float *
view_matrix (void)
{
	return state.matrix;
}

static void
view_recalc (void)
{
	float aspect_ratio = state.width / state.height;
	float matrix_frustum[16];
	float matrix_translate[16];

	// Create frustum matrix:
	mat_frustum(matrix_frustum, 0.7, aspect_ratio, 0.5, 6);

	// Create frustum translation matrix:
	mat_translate(matrix_translate, 0, 0, state.z);

	// Combine into perspective matrix:
	mat_multiply(state.matrix, matrix_frustum, matrix_translate);
}

void
view_set_window (int width, int height)
{
	state.width  = width;
	state.height = height;
	view_recalc();
}

void
view_z_decrease (void)
{
	if (state.z > 1.5f) {
		state.z -= 0.1f;
		view_recalc();
	}
}

void
view_z_increase (void)
{
	if (state.z < 5.0f) {
		state.z += 0.1f;
		view_recalc();
	}
}
