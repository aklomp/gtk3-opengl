#include <math.h>
#include <GL/glew.h>

#include "matrix.h"
#include "program.h"

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
	struct corner corner[4];
	struct point normal;
	struct triangle tri[2];
} __attribute__((packed));

// Each cube has six faces:
struct cube {
	struct face face[6];
} __attribute__((packed));

static GLuint vao, vbo;
static float matrix[16] = { 0 };

// Mouse movement:
static struct {
	int x;
	int y;
} pan;

// Cube rotation axis:
static struct point rot = {
	.x = 0.0f,
	.y = 1.0f,
	.z = 0.0f,
};

// Return the cross product of two vectors:
static void
cross (struct point *result, const struct point *a, const struct point *b)
{
	result->x = a->y * b->z - a->z * b->y;
	result->y = a->z * b->x - a->x * b->z;
	result->z = a->x * b->y - a->y * b->x;
}

// Initialize the model:
void
model_init (void)
{
	// Define our cube:
	struct cube cube =
	{ .face[0].corner =
	  { { 0, 1, 0 }
	  , { 1, 0, 0 }
	  , { 0, 0, 0 }
	  , { 1, 1, 0 }
	  }
	, .face[1].corner =
	  { { 0, 0, 0 }
	  , { 1, 0, 1 }
	  , { 0, 0, 1 }
	  , { 1, 0, 0 }
	  }
	, .face[2].corner =
	  { { 1, 0, 0 }
	  , { 1, 1, 1 }
	  , { 1, 0, 1 }
	  , { 1, 1, 0 }
	  }
	, .face[3].corner =
	  { { 1, 1, 0 }
	  , { 0, 1, 1 }
	  , { 1, 1, 1 }
	  , { 0, 1, 0 }
	  }
	, .face[4].corner =
	  { { 0, 1, 0 }
	  , { 0, 0, 1 }
	  , { 0, 1, 1 }
	  , { 0, 0, 0 }
	  }
	, .face[5].corner =
	  { { 0, 1, 1 }
	  , { 1, 0, 1 }
	  , { 1, 1, 1 }
	  , { 0, 0, 1 }
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

	// Face normals are cross product of two ribs:
	for (int f = 0; f < 6; f++) {
		struct face *face = &cube.face[f];

		// First rib is (corner 3 - corner 0):
		struct point a = {
			.x = face->corner[3].pos.x - face->corner[0].pos.x,
			.y = face->corner[3].pos.y - face->corner[0].pos.y,
			.z = face->corner[3].pos.z - face->corner[0].pos.z,
		};

		// Second rib is (corner 2 - corner 0):
		struct point b = {
			.x = face->corner[2].pos.x - face->corner[0].pos.x,
			.y = face->corner[2].pos.y - face->corner[0].pos.y,
			.z = face->corner[2].pos.z - face->corner[0].pos.z,
		};

		// Face normal is cross product of these two ribs:
		cross(&face->normal, &a, &b);
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

	// Add vertex, color and normal data to buffers:
	struct {
		enum LocCube	 loc;
		const void	*ptr;
	}
	map[] = {
		{ .loc = LOC_CUBE_VERTEX
		, .ptr = &((struct vertex *)0)->pos
		} ,
		{ .loc = LOC_CUBE_VCOLOR
		, .ptr = &((struct vertex *)0)->color
		} ,
		{ .loc = LOC_CUBE_NORMAL
		, .ptr = &((struct vertex *)0)->normal
		} ,
	};

	for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
		GLint loc = program_cube_loc(map[i].loc);
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), map[i].ptr);
	}

	// Upload vertex data:
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
}

void
model_draw (void)
{
	static float angle = 0.0f;

	// Rotate slightly:
	angle += 0.01f;

	// Setup rotation matrix:
	mat_rotate(matrix, rot.x, rot.y, rot.z, angle);

	// Use our own shaders:
	program_cube_use();

	// Don't clip against background:
	glClear(GL_DEPTH_BUFFER_BIT);

	// Draw all the triangles in the buffer:
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

const float *
model_matrix (void)
{
	return matrix;
}

void
model_pan_start (int x, int y)
{
	pan.x = x;
	pan.y = y;
}

void
model_pan_move (int x, int y)
{
	int dx = pan.x - x;
	int dy = pan.y - y;

	// Rotation vector is perpendicular to (dx, dy):
	rot.x = dy;
	rot.y = -dx;
}
