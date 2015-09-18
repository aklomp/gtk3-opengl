#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

#include "model.h"
#include "view.h"

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
	GLuint id;
}
programs[] = {
	[BKGD] = {
		.shader.vert = SHADER (bkgd_vertex),
		.shader.frag = SHADER (bkgd_fragment),
	},
	[CUBE] = {
		.shader.vert = SHADER (cube_vertex),
		.shader.frag = SHADER (cube_fragment),
	},
};

// Attribute/uniform locations:
static struct {
	struct {
		GLint view;
		GLint model;
	} matrix;
	struct {
		GLint vertex;
		GLint texture;
	} bkgd;
	GLint vertex;
	GLint vcolor;
	GLint normal;
} loc;

static void
check_compile (GLuint shader)
{
	GLint status, length;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_FALSE)
		return;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
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

void
program_init (void)
{
	for (size_t i = 0; i < sizeof (programs) / sizeof (programs[0]); i++) {
		struct program *p = &programs[i];
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
	}

	loc.matrix.view  = glGetUniformLocation(programs[CUBE].id, "view_matrix");
	loc.matrix.model = glGetUniformLocation(programs[CUBE].id, "model_matrix");
	loc.vertex = glGetAttribLocation(programs[CUBE].id, "vertex");
	loc.vcolor = glGetAttribLocation(programs[CUBE].id, "vcolor");
	loc.normal = glGetAttribLocation(programs[CUBE].id, "normal");
	loc.bkgd.vertex = glGetAttribLocation(programs[BKGD].id, "vertex");
	loc.bkgd.texture = glGetAttribLocation(programs[BKGD].id, "texture");
}

void
program_use (void)
{
	glUseProgram(programs[CUBE].id);

	glUniformMatrix4fv(loc.matrix.view, 1, GL_FALSE, view_matrix());
	glUniformMatrix4fv(loc.matrix.model, 1, GL_FALSE, model_matrix());
}

void
program_bkgd_use (void)
{
	glUseProgram(programs[BKGD].id);

	glUniform1i(glGetUniformLocation(programs[BKGD].id, "tex"), 0);
}

GLint program_vcolor_loc (void)
{
	return loc.vcolor;
}

GLint program_vertex_loc (void)
{
	return loc.vertex;
}

GLint program_normal_loc (void)
{
	return loc.normal;
}

GLint program_bkgd_vertex_loc (void)
{
	return loc.bkgd.vertex;
}

GLint program_bkgd_texture_loc (void)
{
	return loc.bkgd.texture;
}
