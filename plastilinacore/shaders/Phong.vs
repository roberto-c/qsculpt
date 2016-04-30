#version 330 core

#define USE_DIFF_TEXTURE

/* Inputs */
in vec4 glVertex;
in vec4 glNormal;
in vec4 glColor;

/* Uniform Inputs */
uniform vec4 lightPosition;
uniform vec4 eyePosition;
uniform mat4 glProjectionMatrix;
uniform mat4 glModelViewMatrix;
uniform mat4 objectTransform;

/* Outputs -> fragment program */
out vec3 lightVector, eyeVector, normal;
out vec4 color;
out vec4 gl_Position;

#ifdef USE_DIFF_TEXTURE
in vec2	glTexCoord;
smooth out vec2 texCoord;
#endif

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
    
	/* Normal in world coordinates */
    mat4 glNormalMatrix = transpose(inverse(glModelViewMatrix));
    vec4 n = normalize(glNormalMatrix * glNormal);
	normal = n.xyz;
    
	/* Vertex->Light in world coordinates */
	vec4 worldSpacePos = glModelViewMatrix * glVertex;
    
	lightVector = (lightPosition - worldSpacePos).xyz;
    
	/* Vertex->Eye in world coordinates */
	eyeVector = (eyePosition - worldSpacePos).xyz;
    
    color = glColor;
#ifdef USE_DIFF_TEXTURE
	texCoord = glTexCoord;
#endif
}

