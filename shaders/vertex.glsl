#version 330

uniform mat4 view_matrix;
uniform mat4 model_matrix;

in vec3 vertex;
in vec3 vcolor;
in vec3 normal;

out vec3 fcolor;
out vec3 fpos;
out float fdot;

void main (void)
{
	vec4 modelspace = model_matrix * vec4(vertex, 1.0);

	gl_Position = view_matrix * modelspace;
	fcolor = vcolor;

	/* Sight vector is straight down in world coords: (0, 0, -1) */
	vec4 sight = vec4(0, 0, -1.0, 0.0);

	/* Transform vertex normal to world coordinates: */
	vec4 wnormal = model_matrix * vec4(normal, 0.0);

	/* Get cosine of the angle between sight and normal: */
	fdot = dot(sight, wnormal);

	/* Feed position to fragment shader: */
	fpos = modelspace.xyz;
}
