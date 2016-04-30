#version 330 core

/* Inputs */
in vec4 glVertex;
in vec2 glTexCoord;

/* Uniform Inputs */
uniform mat4 mvpTransform;
uniform vec4 outlineColor;
uniform vec4 fillColor;

/* Outputs -> fragment program */
smooth out vec2 barycentricCoords;

vec4 ftransform() {
	return mvpTransform * glVertex;
}

void main() {
    
	/* The following statement is almost equivalent to writing
     "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;".
     Reasons for preferring ftransform() over the above are
     that this may a) be faster and b) is guaranteed to return
     values that are numerically identical to the fixed-function
     pipeline results.
     */
	gl_Position = mvpTransform * glVertex;
    
    fsOutlineColor = outlineColor;
	fsFillColor = fillColor
	barycentricCoords = glTexCoord;
}

