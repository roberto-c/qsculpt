#version 330 core

/* Inputs */
in vec4 glVertex;
in vec4 glColor;

/* Uniform Inputs */
uniform mat4 glProjectionMatrix;
uniform mat4 glModelViewMatrix;
uniform mat4 objectTransform;


/* Outputs -> fragment program */
out vec4 color;
//out vec4 gl_Position;

vec4 ftransform() {
	return glProjectionMatrix * glModelViewMatrix * objectTransform * glVertex;
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
    
    color = glColor;
}

