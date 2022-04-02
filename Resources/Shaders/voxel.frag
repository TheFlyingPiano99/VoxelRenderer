#version 420 core

out vec4 FragColor;
in vec2 texCoords;

layout (binding = 0) uniform sampler3D voxels;
layout (binding = 1) uniform sampler2D colorAttenuationTransfer;
layout (binding = 2) uniform sampler2D enterTexture;
layout (binding = 3) uniform sampler2D exitTexture;

layout (binding = 4) uniform sampler2D lightExitTexture0;
layout (binding = 5) uniform sampler2D lightExitTexture1;
layout (binding = 6) uniform sampler2D lightExitTexture2;
layout (binding = 7) uniform sampler2D lightExitTexture3;
layout (binding = 8) uniform sampler2D lightExitTexture4;
layout (binding = 9) uniform sampler2D lightExitTexture5;
layout (binding = 10) uniform sampler2D lightExitTexture6;
layout (binding = 11) uniform sampler2D lightExitTexture7;

uniform vec3 resolution;
uniform mat4 modelMatrix;
uniform mat4 invModelMatrix;

uniform float exposure;
uniform float gamma;

uniform float shininess;
uniform vec3 specularColor;
uniform vec3 ambientColor;

uniform unsigned int shadowSamples;
uniform float depthLimit;

struct Light {
	vec4 position;
	vec3 powerDensity;
	mat4 viewProjMatrix;
};
uniform Light lights[16];
uniform unsigned int lightCount;


float trilinearInterpolation(vec3 currentPos) {
	vec3 currentVoxel = vec3(ivec3(currentPos));
	vec3 inVoxelPos = currentPos - currentVoxel;
	float sample000 = texture(voxels, (currentVoxel + vec3(0,0,0)) / resolution).r;
	float sample100 = texture(voxels, (currentVoxel + vec3(1,0,0)) / resolution).r;
	float sample010 = texture(voxels, (currentVoxel + vec3(0,1,0)) / resolution).r;
	float sample001 = texture(voxels, (currentVoxel + vec3(0,0,1)) / resolution).r;
	float sample110 = texture(voxels, (currentVoxel + vec3(1,1,0)) / resolution).r;
	float sample101 = texture(voxels, (currentVoxel + vec3(1,0,1)) / resolution).r;
	float sample011 = texture(voxels, (currentVoxel + vec3(0,1,1)) / resolution).r;
	float sample111 = texture(voxels, (currentVoxel + vec3(1,1,1)) / resolution).r;

	float filtered = (
				(sample000 * (1.0 - inVoxelPos.z) + sample001 * inVoxelPos.z) * (1.0 - inVoxelPos.y)
				+ (sample010 * (1.0 - inVoxelPos.z) + sample011 * inVoxelPos.z) * inVoxelPos.y
			) * (1.0 - inVoxelPos.x)
		 + (
				(sample100 * (1.0 - inVoxelPos.z) + sample101 * inVoxelPos.z) * (1.0 - inVoxelPos.y)
				+ (sample110 * (1.0 - inVoxelPos.z) + sample111 * inVoxelPos.z) * inVoxelPos.y
		   ) * inVoxelPos.x;

	return filtered;
}


vec4 resampleGradientAndDensity(vec3 position, float intensity)
{
	vec3 normPos = position / resolution;
	vec3 stepSize = 1.0 / resolution;
	vec3 sample0, sample1;
	sample0.x = texture(voxels,
	vec3(normPos.x - stepSize.x, normPos.y, normPos.z)).r;
	sample1.x = texture(voxels,
	vec3(normPos.x + stepSize.x, normPos.y, normPos.z)).r;
	sample0.y = texture(voxels,
	vec3(normPos.x, normPos.y - stepSize.y, normPos.z)).r;
	sample1.y = texture(voxels,
	vec3(normPos.x, normPos.y + stepSize.y, normPos.z)).r;
	sample0.z = texture(voxels,
	vec3(normPos.x, normPos.y, normPos.z - stepSize.z)).r;
	sample1.z = texture(voxels,
	vec3(normPos.x, normPos.y, normPos.z + stepSize.z)).r;
	vec3 scaledPosition = position * resolution - 0.5;
	vec3 fraction = scaledPosition - floor(scaledPosition);
	vec3 correctionPolynomial = (fraction * (fraction - 1.0)) / 2.0;
	intensity += dot((sample0 - intensity * 2.0 + sample1),
	correctionPolynomial);
	return vec4(sample1 - sample0, intensity);
}


vec3 calculateLightLevel(vec3 modelPos, vec3 diffuseColor, vec3 specularColor, float shininess, Light light, vec3 gradient, vec3 modelEyePos) {
	vec4 lightCameraSpaceCoord = light.viewProjMatrix * modelMatrix * vec4(modelPos, 1);
	vec2 tex = (lightCameraSpaceCoord.xy / lightCameraSpaceCoord.w + vec2(1, 1)) / vec2(2.0, 2.0);
	
	/*
	if (tex.y > 0.5) {
		if (tex.x < 0.5) {
			return	vec3(0, 10, 0);
		}
		return	vec3(10, 0, 0);
	}
	*/
	
	vec3 lightBeamEndPos = texture(lightExitTexture0, tex).xyz;
	float distanceToBeamEndPos = length(lightBeamEndPos - modelPos);
	float delta = distanceToBeamEndPos / float(shadowSamples + 1);
	float distanceTravelled = delta;
	vec3 lightSourceModelPos = vec4(invModelMatrix * light.position).xyz;
	vec3 toLightSource = lightSourceModelPos - modelPos;
	vec3 lightDir = normalize(toLightSource);
	vec3 samplePos = modelPos + lightDir * delta;
	float disanceToLightSource = length(toLightSource);
	float opacity = 1.0;
	int i = 0;
	while(distanceTravelled < distanceToBeamEndPos) {
		vec4 gradientIntesity = resampleGradientAndDensity(samplePos, texture(voxels, samplePos).a);
		opacity *= distanceTravelled / disanceToLightSource + pow(1 - texture(colorAttenuationTransfer, vec2(gradientIntesity.w, length(gradientIntesity.xyz))).a, delta) 
			* (1 - distanceTravelled / disanceToLightSource);
		samplePos += lightDir * delta;
		distanceTravelled += delta;
		i++;
	}
	vec3 normal = normalize(-gradient);
	vec3 halfway = normalize(normalize(modelEyePos - modelPos) + lightDir);
	float specularCos = pow(max(dot(normal, halfway), 0.0), shininess);
	float diffuseCos = max(dot(normal, lightDir), 0.0);
	vec3 intensity = opacity / disanceToLightSource / disanceToLightSource * light.powerDensity;
	
	return diffuseColor * intensity * diffuseCos 
		+ specularColor * intensity * specularCos;
}

vec4 calculateColor(vec3 cameraRayStart, vec3 cameraRay) {
	float rayLength = length(cameraRay);
	vec4 color = vec4(0.0);
	if (0.000001 < rayLength) {
		vec3 cameraRayDirection = normalize(cameraRay);
		float distanceTravelled = 0.0;
		vec3 currentPos = cameraRayStart;
		int iterations = 0;

		float delta = 0.95f;
		float opacity = 1.0;
		while (distanceTravelled < rayLength * depthLimit) {
			vec4 gradientIntesity = resampleGradientAndDensity(currentPos, trilinearInterpolation(currentPos));
			vec4 colorAttenuation = texture(colorAttenuationTransfer, vec2(gradientIntesity.w, length(gradientIntesity.xyz)));
			vec3 lightLevel = vec3(0.0);
			for (int i = 0; i < lightCount; i++) {
				lightLevel += calculateLightLevel(currentPos, colorAttenuation.rgb, specularColor, shininess, lights[i], gradientIntesity.xyz, cameraRayStart);
			}
			lightLevel += ambientColor;
			color.rgb += delta * opacity * lightLevel;	// Sum color
			color.a += colorAttenuation.a * delta;
			opacity *= pow(1 - colorAttenuation.a, delta);	// Product opacity
			currentPos += cameraRayDirection * delta;
			distanceTravelled += delta;
			iterations++;
		}
	}
	else {
		return vec4(0, 0, 0, 0);	// Background outside bounding cuboid
	}
	return color;
}


void main() {
	//vec3 cameraRayStart;
	//vec3 cameraRayDirection;
	//calculateRayStart(vec2(texCoords.x, -texCoords.y) * 2 - 1, cameraRayStart, cameraRayDirection);
	
	vec3 start = texture(enterTexture, texCoords).xyz;
	vec3 ray = texture(exitTexture, texCoords).xyz - start;
	vec4 hdrColor = calculateColor(start, ray);
	// HDR Tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor.rgb * exposure);
	// GAMMA CORRECTION (OPTIONAL)
    result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(result, hdrColor.a);
}