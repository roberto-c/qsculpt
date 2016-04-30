#version 330 core
precision highp float;

uniform vec4 outlineColor;
uniform vec4 fillColor;

/* Inputs <- Vertex program */
smooth in  vec2 barycentricCoords;

out vec4 glFragColor;

void main() {
	/* Interpolated directions need to be re-normalized */
	glFragColor = fsFillColor;
}

