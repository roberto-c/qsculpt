//*#version 120

/* Uniform Inputs */
uniform vec4 lightPosition;
uniform vec4 eyePosition;

/* Outputs -> fragment program */
varying vec3 lightVector, eyeVector, normal;
varying vec4 color;

void main() {

	/* The following statement is almost equivalent to writing
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;".
	   Reasons for preferring ftransform() over the above are 
	   that this may a) be faster and b) is guaranteed to return 
	   values that are numerically identical to the fixed-function
	   pipeline results.
	*/
	gl_Position = ftransform();

	/* Normal in world coordinates */
	normal = normalize(gl_NormalMatrix * gl_Normal);

	/* Vertex->Light in world coordinates */
	vec4 worldSpacePos = gl_ModelViewMatrix * gl_Vertex;

	lightVector = (lightPosition - worldSpacePos).xyz;

	/* Vertex->Eye in world coordinates */
	eyeVector = (eyePosition - worldSpacePos).xyz;
    
    color = gl_Color;
}

