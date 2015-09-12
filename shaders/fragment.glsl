#version 330

in vec3 fcolor;

void main (void)
{
	gl_FragColor = vec4(fcolor, 1.0);
}
