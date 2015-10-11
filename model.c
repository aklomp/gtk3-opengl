#include <math.h>
#include <GL/glew.h>

#include "matrix.h"
#include "program.h"

static GLuint vao, vbo;
static float matrix[16] = { 0 };
static int pan_x, pan_y;
static float rot_x = 0.0f;
static float rot_y = 1.0f;
static float rot_z = 0.0f;

void
model_init (void)
{
	struct point {
		float x;
		float y;
		float z;
	} __attribute__((packed));

	struct color {
		float r;
		float g;
		float b;
	} __attribute__((packed));

	// Each vertex has position, normal and color:
	struct vertex {
		struct point pos;
		struct point normal;
		struct color color;

	} __attribute__((packed));

	// Each triangle has three vertices:
	struct triangle {
		struct vertex vert[3];
	} __attribute__((packed));

	// Each corner point has a position and a color:
	struct corner {
		struct point pos;
		struct color color;
	} __attribute__((packed));

	// Each face has a single normal, four corner points,
	// and two triangles:
	struct face {
		struct point normal;
		struct corner corner[4];
		struct triangle tri[2];
	} __attribute__((packed));

	// Each cube has six faces:
	struct cube {
		struct face face[6];
	} __attribute__((packed));

	// Define our cube:
	struct cube cube =
	{
	  {
	    {				// Face 0
	      { 0, 0, -1 }		// Normal
	    , { { { 0, 1, 0 } }		// Corners
	      , { { 1, 0, 0 } }
	      , { { 0, 0, 0 } }
	      , { { 1, 1, 0 } }
	      }
	    }
	  ,
	    {				// Face 1
	      { 0, -1, 0 }		// Normal
	    , { { { 0, 0, 0 } }		// Corners
	      , { { 1, 0, 1 } }
	      , { { 0, 0, 1 } }
	      , { { 1, 0, 0 } }
	      }
	    }
	  ,
	    {				// Face 2
	      { 1, 0, 0 }		// Normal
	    , { { { 1, 0, 0 } }		// Corners
	      , { { 1, 1, 1 } }
	      , { { 1, 0, 1 } }
	      , { { 1, 1, 0 } }
	      }
	    }
	  ,
	    {				// Face 3
	      { 0, 1, 0 }		// Normal
	    , { { { 1, 1, 0 } }		// Corners
	      , { { 0, 1, 1 } }
	      , { { 1, 1, 1 } }
	      , { { 0, 1, 0 } }
	      }
	    }
	  ,
	    {				// Face 4
	      { -1, 0, 0 }		// Normal
	    , { { { 0, 1, 0 } }		// Corners
	      , { { 0, 0, 1 } }
	      , { { 0, 1, 1 } }
	      , { { 0, 0, 0 } }
	      }
	    }
	  ,
	    {				// Face 5
	      { 0, 0, 1 }		// Normal
	    , { { { 0, 1, 1 } }		// Corners
	      , { { 1, 0, 1 } }
	      , { { 1, 1, 1 } }
	      , { { 0, 0, 1 } }
	      }
	    }
	  }
	} ;

	// Generate colors for each corner based on its position:
	for (int f = 0; f < 6; f++) {
		struct face *face = &cube.face[f];
		for (int c = 0; c < 4; c++) {
			struct corner *corner = &face->corner[c];
			corner->color.r = corner->pos.x * 0.8f + 0.1f;
			corner->color.g = corner->pos.y * 0.8f + 0.1f;
			corner->color.b = corner->pos.z * 0.8f + 0.1f;
		}
	}

	// Center cube on the origin by translating corner points:
	for (int f = 0; f < 6; f++) {
		struct face *face = &cube.face[f];
		for (int c = 0; c < 4; c++) {
			struct corner *corner = &face->corner[c];
			corner->pos.x -= 0.5f;
			corner->pos.y -= 0.5f;
			corner->pos.z -= 0.5f;
		}
	}

	// Create two triangles for each face:
	for (int f = 0; f < 6; f++) {
		struct face *face = &cube.face[f];

		// Corners to compose triangles of, chosen in
		// such a way that both triangles rotate CCW:
		int index[2][3] = { { 0, 2, 1 }, { 1, 3, 0 } };

		for (int t = 0; t < 2; t++) {
			for (int v = 0; v < 3; v++) {
				int c = index[t][v];
				struct corner *corner = &face->corner[c];
				struct vertex *vertex = &face->tri[t].vert[v];

				vertex->pos = corner->pos;
				vertex->normal = face->normal;
				vertex->color = corner->color;
			}
		}
	}

	// Copy vertices into separate array for drawing:
	struct vertex vertex[6 * 2 * 3];
	struct vertex *cur = vertex;

	for (int f = 0; f < 6; f++) {
		struct face *face = &cube.face[f];
		for (int t = 0; t < 2; t++) {
			struct triangle *tri = &face->tri[t];
			for (int v = 0; v < 3; v++) {
				*cur++ = tri->vert[v];
			}
		}
	}

	// Generate empty buffer:
	glGenBuffers(1, &vbo);

	// Generate empty vertex array object:
	glGenVertexArrays(1, &vao);

	// Set as current vertex array:
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Add vertex data to first buffer:
	glEnableVertexAttribArray(program_vertex_loc());
	glVertexAttribPointer(program_vertex_loc(), 3, GL_FLOAT, GL_FALSE,
		sizeof(struct vertex),
		(void *)(&((struct vertex *)0)->pos.x));

	// Add color data to second buffer:
	glEnableVertexAttribArray(program_vcolor_loc());
	glVertexAttribPointer(program_vcolor_loc(), 3, GL_FLOAT, GL_FALSE,
		sizeof(struct vertex),
		(void *)(&((struct vertex *)0)->color.r));

	// Add normal data to third buffer:
	glEnableVertexAttribArray(program_normal_loc());
	glVertexAttribPointer(program_normal_loc(), 3, GL_FLOAT, GL_FALSE,
		sizeof(struct vertex),
		(void *)(&((struct vertex *)0)->normal.x));

	// Upload vertex data:
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
}

void
model_draw (void)
{
	static float angle = 0.0f;

	// Rotate slightly:
	angle += 0.01;

	// Setup rotation matrix:
	mat_rotate(matrix, rot_x, rot_y, rot_z, angle);

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

void
model_pan_start (int x, int y)
{
	pan_x = x;
	pan_y = y;
}

void
model_pan_move (int x, int y)
{
	int dx = pan_x - x;
	int dy = pan_y - y;

	// Rotation vector is perpendicular to (dx, dy):
	rot_x = dy;
	rot_y = -dx;
}
