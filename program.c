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

static GLint vertex_loc;
static GLint view_matrix_loc;
static GLint model_matrix_loc;
static GLint vcolor_loc;
static GLint normal_loc;

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

	view_matrix_loc  = glGetUniformLocation(program, "view_matrix");
	model_matrix_loc = glGetUniformLocation(program, "model_matrix");
	vertex_loc = glGetAttribLocation(program, "vertex");
	vcolor_loc = glGetAttribLocation(program, "vcolor");
	normal_loc = glGetAttribLocation(program, "normal");
}

void
program_use (void)
{
	glUseProgram(program);

	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, view_matrix());
	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix());
}

GLint program_vcolor_loc (void)
{
	return vcolor_loc;
}

GLint program_vertex_loc (void)
{
	return vertex_loc;
}

GLint program_normal_loc (void)
{
	return normal_loc;
}
