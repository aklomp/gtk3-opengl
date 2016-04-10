#version 150

uniform sampler2D tex;
in vec2 ftex;

out vec4 fragcolor;

void main (void)
{
	fragcolor = texture(tex, ftex);
}
