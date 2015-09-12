#include <GL/glew.h>

#include "matrix.h"
#include "program.h"

static GLuint vao, vbo[2];
static float matrix[16] = { 0 };

void
model_init (void)
{
	struct point {
		float x;
		float y;
		float z;
	};

	// Corner vertices of a unit cube:
	struct point cube[8] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f },
	};

	// Colors per vertex:
	struct point vcolors[8];
	for (int i = 0; i < 8; i++) {
		vcolors[i].x = cube[i].x * 0.8 + 0.1;
		vcolors[i].y = cube[i].y * 0.8 + 0.1;
		vcolors[i].z = cube[i].z * 0.8 + 0.1;
	}

	// Triangles in a unit cube:
	int triangles[12][3] = {
		{ 0, 2, 1 },
		{ 0, 2, 3 },
		{ 0, 5, 4 },
		{ 0, 5, 1 },
		{ 5, 2, 6 },
		{ 5, 2, 1 },
		{ 2, 7, 6 },
		{ 2, 7, 3 },
		{ 4, 3, 7 },
		{ 4, 3, 0 },
		{ 6, 4, 7 },
		{ 6, 4, 5 },
	};

	// Generate an array of vertices:
	float vertices[12 * 3 * 3];
	float *cur = vertices;

	// Translate points to center cube on the origin:
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 3; j++) {
			*cur++ = cube[triangles[i][j]].x - 0.5;
			*cur++ = cube[triangles[i][j]].y - 0.5;
			*cur++ = cube[triangles[i][j]].z - 0.5;
		}
	}

	// Generate an array of colors:
	float colors[12 * 3 * 3];
	cur = colors;

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 3; j++) {
			*cur++ = vcolors[triangles[i][j]].x * 0.8 + 0.1;
			*cur++ = vcolors[triangles[i][j]].y * 0.8 + 0.1;
			*cur++ = vcolors[triangles[i][j]].z * 0.8 + 0.1;
		}
	}

	// Generate empty buffers:
	glGenBuffers(2, vbo);

	// Generate empty vertex array object:
	glGenVertexArrays(1, &vao);

	// Set as current vertex array:
	glBindVertexArray(vao);

	// Add vertex data to first buffer:
	glEnableVertexAttribArray(program_vertex_loc());
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(program_vertex_loc(), 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Add color data to second buffer:
	glEnableVertexAttribArray(program_vcolor_loc());
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(program_vcolor_loc(), 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void
model_draw (void)
{
	static float angle = 0.0f;

	// Rotate slightly:
	angle += 0.01;

	// Setup rotation matrix:
	mat_rotate(matrix, angle);

	// Use our own shaders:
	program_use();

	// Draw all the triangles in the buffer:
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

float *
model_matrix (void)
{
	return matrix;
}
