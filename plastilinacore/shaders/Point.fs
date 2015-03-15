#version 330 core
precision highp float;

/* Inputs <- Vertex program */
in vec4 color;
out vec4 glFragColor;

void main() {
	/* Interpolated directions need to be re-normalized */
	glFragColor = color;
	glFragColor.a = 1.0f;
}

