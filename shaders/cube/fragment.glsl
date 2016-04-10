#version 330

in vec3 fcolor;
in vec3 fpos;
in float fdot;

out vec4 fragcolor;

void main (void)
{
	if (!gl_FrontFacing)
		return;

	/* Get gamma-corrected (linear) color values */
	vec3 linear = pow(fcolor, vec3(1.0 / 2.2));

	/* Get distance-related light falloff factor: */
	float dst = distance(vec3(0, 0, 2), fpos) * 0.4;

	/* Scale these by fdot: */
	vec3 scaled = linear * vec3(fdot * dst);

	/* Restore gamma and output this color: */
	fragcolor = vec4(pow(scaled, vec3(2.2)), 0.0);
}
