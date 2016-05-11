#version 330 core

precision highp float;

/* Inputs <- Vertex program */
//in vec4 lineColor;
smooth in vec4 lineColor;
out vec4 glFragColor;

void main() {
	/* Interpolated directions need to be re-normalized */
    vec4 colorValue = lineColor;
	colorValue.a = 1.0;
	glFragColor = colorValue;
}

