#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

#include "model.h"
#include "view.h"

// Inline data definitions:
extern const uint8_t _binary_shaders_vertex_glsl_start[];
extern const uint8_t _binary_shaders_vertex_glsl_end[];

extern const uint8_t _binary_shaders_fragment_glsl_start[];
extern const uint8_t _binary_shaders_fragment_glsl_end[];

static GLuint program;

// Attribute/uniform locations:
static struct {
	struct {
		GLint view;
		GLint model;
	} matrix;
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

static GLuint
create_shader (const uint8_t *_buf, const uint8_t *_end, GLenum type)
{
	const GLchar *buf = _buf;
	const GLchar *end = _end;
	GLint len = end - buf;

	GLuint s = glCreateShader(type);
	glShaderSource(s, 1, &buf, &len);
	glCompileShader(s);

	check_compile(s);

	return s;
}

void
program_init (void)
{
	GLuint vert = create_shader
		( _binary_shaders_vertex_glsl_start
		, _binary_shaders_vertex_glsl_end
		, GL_VERTEX_SHADER
		) ;

	GLuint frag = create_shader
		( _binary_shaders_fragment_glsl_start
		, _binary_shaders_fragment_glsl_end
		, GL_FRAGMENT_SHADER
		) ;

	program = glCreateProgram();

	glAttachShader(program, vert);
	glAttachShader(program, frag);

	glLinkProgram(program);

	check_link(program);

	glDetachShader(program, vert);
	glDetachShader(program, frag);

	glDeleteShader(vert);
	glDeleteShader(frag);

	loc.matrix.view  = glGetUniformLocation(program, "view_matrix");
	loc.matrix.model = glGetUniformLocation(program, "model_matrix");
	loc.vertex = glGetAttribLocation(program, "vertex");
	loc.vcolor = glGetAttribLocation(program, "vcolor");
	loc.normal = glGetAttribLocation(program, "normal");
}

void
program_use (void)
{
	glUseProgram(program);

	glUniformMatrix4fv(loc.matrix.view, 1, GL_FALSE, view_matrix());
	glUniformMatrix4fv(loc.matrix.model, 1, GL_FALSE, model_matrix());
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
