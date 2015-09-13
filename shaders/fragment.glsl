#version 330

in vec3 fcolor;
in float fdot;

void main (void)
{
	if (!gl_FrontFacing)
		return;

	/* Get gamma-corrected (linear) color values */
	vec3 linear = pow(fcolor, vec3(1.0 / 2.2));

	/* Scale these by fdot: */
	vec3 scaled = linear * vec3(fdot);

	/* Restore gamma and output this color: */
	gl_FragColor = vec4(pow(scaled, vec3(2.2)), 0.0);
}
