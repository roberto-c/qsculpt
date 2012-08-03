//#version 110

/* Uniform Inputs */
uniform vec4 lightPosition;
uniform vec4 eyePosition;
uniform vec4 color;

/* Outputs -> fragment program */
varying vec3 lightVector, eyeVector, normal, halfVector;
//varying vec3 eyeVector, normal, halfVector;
varying vec4 shadowTexCoord;

void main() {
	/* The following statement is almost equivalent to writing
     "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;".
     Reasons for preferring ftransform() over the above are 
     that this may a) be faster and b) is guaranteed to return 
     values that are numerically identical to the fixed-function
     pipeline results.
     */
    
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
    
	/* Normal in world coordinates */
	normal = normalize(gl_NormalMatrix * gl_Normal);
    
	/* Vertex->Light in world coordinates */
	vec4 worldSpacePos = gl_ModelViewMatrix * gl_Vertex;
	lightVector = (lightPosition - worldSpacePos).xyz;
    
	/* Vertex->Eye in world coordinates */
	eyeVector = (eyePosition - worldSpacePos).xyz;
	
	halfVector = normalize(lightVector+eyeVector);
	
	shadowTexCoord = gl_TextureMatrix[0] * worldSpacePos;
	
}