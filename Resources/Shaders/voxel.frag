#version 420 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding=0) uniform sampler3D voxels;
layout(binding=1) uniform sampler2D colorAttenuationTransfer;
layout(binding=2) uniform sampler2D enterTexture;
layout(binding=3) uniform sampler2D exitTexture;
layout(binding=4) uniform sampler2D lightTexture;

uniform vec3 resolution;
uniform mat4 invModelMatrix;

uniform float exposure;
uniform float gamma;

uniform unsigned int shadowSamples;

struct Light {
	vec3 position;
	vec3 intensity;
	mat4 viewProjMatrix;
};
uniform Light light1;

/*
float trilinearInterpolation(vec3 currentPos, out vec3 gradient) {
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

	float sampleGNegX = texture(voxels, (currentVoxel + vec3(-1,0,0)) / resolution).r;
	float sampleGNegY = texture(voxels, (currentVoxel + vec3(0,-1,0)) / resolution).r;
	float sampleGNegZ = texture(voxels, (currentVoxel + vec3(0,0,-1)) / resolution).r;

	gradient = (vec3(1.0) - inVoxelPos) * (vec3(sample000) - vec3(sampleGNegX, sampleGNegY, sampleGNegZ)) / 2.0
	+ inVoxelPos * (vec3(sample100, sample010,sample001) - vec3(sample000)) / 2.0;
	return filtered;
}
*/

vec4 resampleGradientAndDensity(vec3 position)
{
	vec3 normPos = position / resolution;
	float intensity = texture(voxels, normPos).r;
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
	return vec4(normalize(sample1 - sample0), intensity);
}


vec3 calculateLightLevel(vec3 currentPos, Light light, vec3 gradient, vec3 modelEyePos) {
	vec4 lightCameraSpaceCoord = light.viewProjMatrix * vec4(currentPos, 1);
	vec3 lightBeamEndPos = texture(lightTexture, lightCameraSpaceCoord.xy / lightCameraSpaceCoord.w / 2.0 + vec2(0.5)).xyz;
	vec3 toLightBeamEnd = lightBeamEndPos - currentPos;
	float delta = length(toLightBeamEnd) / float(shadowSamples) / 10.0f;
	vec3 lightDir = normalize(toLightBeamEnd);
	vec3 samplePos = currentPos + lightDir * delta;
	float distanceTravelled = 0.0;
	vec3 lightSourceModelPos = vec4(invModelMatrix * vec4(light1.position, 1.0)).xyz;
	vec3 toLightSource = lightSourceModelPos - currentPos;
	float disanceToLightSource = length(toLightSource);
	float opacity = 1.0;
	for (int i = 0; i < shadowSamples; i++) {
		float intensity = texture(voxels, samplePos / resolution).r;
		opacity *= (distanceTravelled / disanceToLightSource) + pow(1 - texture(colorAttenuationTransfer, vec2(intensity, length(gradient))).a, delta)
		* (1 - distanceTravelled / disanceToLightSource);
		samplePos += lightDir * delta;
		distanceTravelled += delta;
		delta *= 1.01;
	}
	vec3 normal = normalize(-gradient);
	vec3 halfway = (normalize(modelEyePos - currentPos) + lightDir) / 2.0;
	float specular = pow(max(dot(normal, halfway), 0.0), 10);
	float diffuse = max(dot(normal, lightDir), 0.0);
	vec3 intensity = opacity / disanceToLightSource / disanceToLightSource * light.intensity;
	float ka = 0.1;
	float kd = 1.0;
	float ks = 1.0;
	return kd * diffuse * intensity 
	+ ks * specular
	+ ka * vec3(1.0);
}

vec3 calculateColor(vec3 cameraRayStart, vec3 cameraRay) {
	float rayLength = length(cameraRay);
	vec3 color = vec3(0.0);
	if (0.00001 < rayLength) {
		vec3 cameraRayDirection = normalize(cameraRay);
		float distanceTravelled = 0.0;
		vec3 currentPos = cameraRayStart;
		int iterations = 0;

		float delta = rayLength / 100.0;
		float opacity = 1.0;
		while (distanceTravelled < rayLength) {
			vec4 gradientIntesity = resampleGradientAndDensity(currentPos);
			vec4 colorAttenuation = texture(colorAttenuationTransfer, vec2(gradientIntesity.w, length(gradientIntesity.xyz)));
			vec3 lightLevel = calculateLightLevel(currentPos, light1, gradientIntesity.xyz, cameraRayStart);
			color += delta * colorAttenuation.rgb * opacity * lightLevel;	// Sum color
			opacity *= pow(1 - colorAttenuation.a, delta);	// Product opacity
			currentPos += cameraRayDirection * delta;
			distanceTravelled += delta;
			iterations++;
		}
	}
	else {
		return vec3(1, 1, 1);	// Background outside bounding cuboid
	}
	return color;
}


void main() {
	//vec3 cameraRayStart;
	//vec3 cameraRayDirection;
	//calculateRayStart(vec2(texCoords.x, -texCoords.y) * 2 - 1, cameraRayStart, cameraRayDirection);
	
	vec3 start = texture(enterTexture, texCoords).xyz;
	vec3 ray = texture(exitTexture, texCoords).xyz - start;
	vec3 hdrColor = calculateColor(start, ray);
	// HDR Tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	// GAMMA CORRECTION (OPTIONAL)
    result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(result, 1.0f);
}