#version 330

uniform mat4 view_matrix;
uniform mat4 model_matrix;

in vec3 vertex;
in vec3 vcolor;

out vec3 fcolor;

void main (void)
{
	gl_Position = view_matrix * model_matrix * vec4(vertex, 1.0);
	fcolor = vcolor;
}
