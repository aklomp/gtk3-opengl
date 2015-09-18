#version 150

in vec2 vertex;
in vec2 texture;

out vec2 ftex;

void main (void)
{
	ftex = texture;
	gl_Position = vec4(vertex, 0.5, 1.0);
}
