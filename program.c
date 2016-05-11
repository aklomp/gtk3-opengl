#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

#include "model.h"
#include "view.h"
#include "program.h"

// Define inline data:
#define DATA_DEF(x)							\
	extern const uint8_t _binary_shaders_## x ##_glsl_start[];	\
	extern const uint8_t _binary_shaders_## x ##_glsl_end[];

#define SHADER(x)					\
	{ .buf = _binary_shaders_ ## x ## _glsl_start	\
	, .end = _binary_shaders_ ## x ## _glsl_end	\
	}

// Inline data definitions:
DATA_DEF (bkgd_vertex)
DATA_DEF (bkgd_fragment)
DATA_DEF (cube_vertex)
DATA_DEF (cube_fragment)

// Shader structure:
struct shader {
	const uint8_t	*buf;
	const uint8_t	*end;
	GLuint		 id;
};

// Location definitions:
enum loc_type {
	UNIFORM,
	ATTRIBUTE,
};

struct loc {
	const char	*name;
	enum loc_type	 type;
	GLint		 id;
};

static struct loc loc_bkgd[] = {
	[LOC_BKGD_VERTEX]  = { "vertex",	ATTRIBUTE },
	[LOC_BKGD_TEXTURE] = { "texture",	ATTRIBUTE },
};

static struct loc loc_cube[] = {
	[LOC_CUBE_VIEW]   = { "view_matrix",	UNIFORM   },
	[LOC_CUBE_MODEL]  = { "model_matrix",	UNIFORM   },
	[LOC_CUBE_VERTEX] = { "vertex",		ATTRIBUTE },
	[LOC_CUBE_VCOLOR] = { "vcolor",		ATTRIBUTE },
	[LOC_CUBE_NORMAL] = { "normal",		ATTRIBUTE },
};

// Programs:
enum {
	BKGD,
	CUBE,
};

// Program structure:
static struct program {
	struct {
		struct shader vert;
		struct shader frag;
	} shader;
	struct loc *loc;
	size_t nloc;
	GLuint id;
}
programs[] = {
	[BKGD] = {
		.shader.vert = SHADER (bkgd_vertex),
		.shader.frag = SHADER (bkgd_fragment),
		.loc         = loc_bkgd,
		.nloc        = sizeof (loc_bkgd) / sizeof (loc_bkgd[0]),
	},
	[CUBE] = {
		.shader.vert = SHADER (cube_vertex),
		.shader.frag = SHADER (cube_fragment),
		.loc         = loc_cube,
		.nloc        = sizeof (loc_cube) / sizeof (loc_cube[0]),
	},
};

static void
check_compile (GLuint shader)
{
	GLint length;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

	if (length <= 1)
		return;

	GLchar *log = calloc(length, sizeof(GLchar));
	glGetShaderInfoLog(shader, length, NULL, log);
	fprintf(stderr, "glCompileShader failed:\n%s\n", log);
	free(log);
}

static void
check_link (GLuint program)
{
	GLint status, length;

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_FALSE)
		return;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
	GLchar *log = calloc(length, sizeof(GLchar));
	glGetProgramInfoLog(program, length, NULL, log);
	fprintf(stderr, "glLinkProgram failed: %s\n", log);
	free(log);
}

static void
create_shader (struct shader *shader, GLenum type)
{
	const GLchar *buf = shader->buf;
	GLint len = shader->end - shader->buf;

	shader->id = glCreateShader(type);
	glShaderSource(shader->id, 1, &buf, &len);
	glCompileShader(shader->id);

	check_compile(shader->id);
}

static void
program_init (struct program *p)
{
	struct shader *vert = &p->shader.vert;
	struct shader *frag = &p->shader.frag;

	create_shader(vert, GL_VERTEX_SHADER);
	create_shader(frag, GL_FRAGMENT_SHADER);

	p->id = glCreateProgram();

	glAttachShader(p->id, vert->id);
	glAttachShader(p->id, frag->id);

	glLinkProgram(p->id);
	check_link(p->id);

	glDetachShader(p->id, vert->id);
	glDetachShader(p->id, frag->id);

	glDeleteShader(vert->id);
	glDeleteShader(frag->id);

	for (size_t j = 0; j < p->nloc; j++) {
		struct loc *l = &p->loc[j];

		switch (l->type)
		{
		case UNIFORM:
			l->id = glGetUniformLocation(p->id, l->name);
			break;

		case ATTRIBUTE:
			l->id = glGetAttribLocation(p->id, l->name);
			break;
		}
	}
}

void
programs_init (void)
{
	for (size_t i = 0; i < sizeof(programs) / sizeof(programs[0]); i++)
		program_init(&programs[i]);
}

void
program_cube_use (void)
{
	glUseProgram(programs[CUBE].id);

	glUniformMatrix4fv(loc_cube[LOC_CUBE_VIEW ].id, 1, GL_FALSE, view_matrix());
	glUniformMatrix4fv(loc_cube[LOC_CUBE_MODEL].id, 1, GL_FALSE, model_matrix());
}

void
program_bkgd_use (void)
{
	glUseProgram(programs[BKGD].id);

	glUniform1i(glGetUniformLocation(programs[BKGD].id, "tex"), 0);
}

GLint
program_bkgd_loc (const enum LocBkgd index)
{
	return loc_bkgd[index].id;
}

GLint
program_cube_loc (const enum LocCube index)
{
	return loc_cube[index].id;
}
