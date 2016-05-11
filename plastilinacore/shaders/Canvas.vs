#version 330 core

#define USE_DIFF_TEXTURE

/* Inputs */
in vec4 vertex;
in vec4 color;

/* Uniform Inputs */
uniform mat4 glProjectionMatrix;
uniform mat4 glModelViewMatrix;

/* Outputs -> fragment program */
smooth out vec4 lineColor;
out vec4 gl_Position;

vec4 ftransform() {
    return glProjectionMatrix * glModelViewMatrix * vertex;
}

void main() {
    
	/* The following statement is almost equivalent to writing
     "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;".
     Reasons for preferring ftransform() over the above are
     that this may a) be faster and b) is guaranteed to return
     values that are numerically identical to the fixed-function
     pipeline results.
     */
	gl_Position = ftransform();
        
    lineColor = color;
}

