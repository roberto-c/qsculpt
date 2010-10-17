#version 110

/* Uniform inputs */
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float exponent;

/* Inputs <- Vertex program */
varying vec3 lightVector, eyeVector, normal;

void main() {
	/* Interpolated directions need to be re-normalized */
	vec3 nNormal = normalize(normal);
	vec3 nEyeVector = normalize(eyeVector);
	vec3 nLightVector = normalize(lightVector);
	vec4 colorValue = vec4(0.05); /* Some ambient */

	float nDotL = dot(nNormal, nLightVector);

	if (nDotL > 0.0) {
		vec3 reflected = reflect(-nLightVector, nNormal);
		float specular = pow(max(dot(reflected, nEyeVector), 0.0), exponent);
                colorValue += gl_Color * nDotL + specularColor * specular;
	}
    //colorValue = diffuseColor;
	colorValue.a = 1.0;
	gl_FragColor = colorValue;
}

