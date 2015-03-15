#version 330 core
precision highp float;

/* Uniform inputs */
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform vec4 ambientColor;
uniform float exponent;
/* Uniform Inputs */
uniform vec4 lightPosition;
uniform vec4 eyePosition;
uniform sampler2D textureSampler;

/* Inputs <- Vertex program */
in vec3 lightVector, eyeVector, normal;
in vec4 color;
in vec2 texCoord;
out vec4 glFragColor;

void main() {
	/* Interpolated directions need to be re-normalized */
	vec3 nNormal = normalize(normal);
	vec3 nEyeVector = normalize(eyeVector);
	vec3 nLightVector = normalize(lightVector);
	vec4 colorValue = ambientColor; /* Some ambient */
    
	float nDotL = dot(nNormal, nLightVector);
    vec4 diffColor = texture(textureSampler, texCoord);
	if (nDotL > 0.0) {
		vec3 reflected = reflect(-nLightVector, nNormal);
		float specular = pow(max(dot(reflected, nEyeVector), 0.0), exponent);
        colorValue += diffColor * nDotL + specularColor * specular;
	}
    colorValue = diffColor;
	colorValue.a = 1.0;
	glFragColor = colorValue;
}

