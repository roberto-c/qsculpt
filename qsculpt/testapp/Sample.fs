//#version 120

/* Uniform inputs */
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float exponent;
uniform float bias;
uniform float sShadow;
uniform int nShadow;   // light width

uniform sampler2D shadowmap;
//uniform sampler2DShadow shadowmap;

/* Inputs <- Vertex program */
varying vec3 lightVector, eyeVector, normal, halfVector;
varying vec4 shadowTexCoord;

float blockerSearch(vec4 shadowLookup){
	float sumDepth = 0.0;
	int shadowCnt = 0;
	int end = int(nShadow/2);
	int start = -end;
	for(int j = start; j <= end; j++){
        for(int i = start; i <= end; i++){
            vec2 coord = vec2(shadowLookup.x + sShadow * float(i),
                              shadowLookup.y + sShadow * float(j));
            float depth = texture2D(shadowmap, coord).z;
            if(shadowLookup.w > 0.0){
                if(depth < shadowLookup.z - bias){
                    shadowCnt++;
                    sumDepth += depth;
                }
            }
        }} // end for(i,j)
	
	float avgDepth = 0.0;
	if(shadowCnt > 0){
		avgDepth = sumDepth / float(shadowCnt);
	}
	return avgDepth;
}

float linearZ(float z){
	float a = 10000.0; // farZ / nearZ
	return z /(a - (a-1.0)*z);
}

float penumbraEst(float depthR, float depthB) {
	float ldepthR = linearZ(depthR);
	float ldepthB = linearZ(depthB);
	return (ldepthR - ldepthB) * float(sShadow) / ldepthB; 
}

//float pcfShadow(vec4 shadowLookup, float kernel){
float pcfShadow(vec4 shadowLookup, float penumbra){
	int shadowCnt = 0;
	int end = int(nShadow/2);
	int start = -end;
	for(int j = start; j <= end; j++){
        for(int i = start; i <= end; i++){
            vec2 coord = vec2(shadowLookup.x + penumbra * float(i),
                              shadowLookup.y + penumbra * float(j));
            //vec2 coord = vec2(shadowLookup.x + sShadow * float(i),
            //				  shadowLookup.y + sShadow * float(j));
            float depth = texture2D(shadowmap, coord).z;
            if(shadowLookup.w > 0.0){
                if(depth < shadowLookup.z - bias){
                    shadowCnt++;
                }
            }
        }} // end for(i,j)
	float sampleSize = float(nShadow*nShadow);
	//if(sampleSize == 0.0){
	//	sampleSize = 1.0;
	//}
	return float(sampleSize-float(shadowCnt)) / sampleSize;
}

void main() {
	/* Interpolated directions need to be re-normalized */
	vec3 nNormal = normalize(normal);
	vec3 nEyeVector = normalize(eyeVector);
	vec3 nLightVector = normalize(lightVector);
	vec3 nHalfVector = normalize(halfVector);
	vec4 colorValue = vec4(0.0);
	
	/* shadow map */
	vec4 shadowLookup = shadowTexCoord / shadowTexCoord.w;
	float avgDepth = blockerSearch(shadowLookup);
	float shadow = 1.0;
	if(avgDepth > 0.0){
		float penumbra = penumbraEst(shadowLookup.z, avgDepth);
		shadow = pcfShadow(shadowLookup, penumbra);
	}
	
	/* compute shading */
//	if( shadow > 0.0 ){
		float nDotL = dot(nNormal, nLightVector);
		if( nDotL > 0.0 ){
			float specular = pow(max(dot(nHalfVector, nNormal), 0.0), exponent);
			colorValue += diffuseColor * nDotL + specularColor * specular;
		}
//	}
	gl_FragColor = colorValue * shadow + vec4(0.05); /* add ambient term */
}

