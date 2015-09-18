#version 150

uniform sampler2D tex;
in vec2 ftex;

void main (void)
{
	gl_FragColor = texture(tex, ftex);
}
