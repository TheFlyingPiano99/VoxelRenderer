#version 420 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding=0) uniform sampler3D voxels;
layout(binding=1) uniform sampler1D colorAttenuationTransfer;
layout(binding=2) uniform sampler2D enterTexture;
layout(binding=3) uniform sampler2D exitTexture;

uniform vec3 resolution;
uniform mat4 invModelMatrix;

uniform float exposure;
uniform float gamma;

uniform unsigned int shadowSamples;


struct Light {
	vec3 position;
	vec3 intensity;
};
uniform Light light1;


float trilinearInterpolation(vec3 currentPos, out vec3 grad) {
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

	grad = vec3(1,0,0) * sample100 + vec3(0,1,0) * sample010 + vec3(0,0,1) * sample001 
		+ vec3(1,1,0) * sample110 + vec3(0,1,1) * sample011 + vec3(1,0,1) * sample101 + vec3(1,1,1) * sample111;
	grad /= (sample000 + sample100 + sample010 + sample001 + sample110 + sample011 + sample101 + sample111);
	return (
				(sample000 * (1.0 - inVoxelPos.z) + sample001 * inVoxelPos.z) * (1.0 - inVoxelPos.y)
				+ (sample010 * (1.0 - inVoxelPos.z) + sample011 * inVoxelPos.z) * inVoxelPos.y
			) * (1.0 - inVoxelPos.x)
		 + (
				(sample100 * (1.0 - inVoxelPos.z) + sample101 * inVoxelPos.z) * (1.0 - inVoxelPos.y)
				+ (sample110 * (1.0 - inVoxelPos.z) + sample111 * inVoxelPos.z) * inVoxelPos.y
		   ) * inVoxelPos.x;
}


vec3 calculateLightLevel(vec3 currentPos, vec3 toLight, vec3 sourceIntensity) {
	float delta = length(toLight) / float(shadowSamples) / 100.0f;
	vec3 lightDir = normalize(toLight);
	vec3 samplePos = currentPos + lightDir * delta;
	float opacity = 1.0;
	for (int i = 0; i < shadowSamples; i++) {
		float intensity = texture(voxels, samplePos / resolution).r;
		opacity *= pow(1 - texture(colorAttenuationTransfer, intensity).a, delta);
		samplePos += lightDir * delta;
		delta *= 1.01;
	}
	return 0.9 * opacity / dot(toLight, toLight) * sourceIntensity + vec3(0.1);
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
		vec3 gradient;
		vec3 lightPos = vec4(invModelMatrix * vec4(light1.position, 1.0)).xyz;
		while (distanceTravelled < rayLength) {
			float intensity = trilinearInterpolation(currentPos, gradient);
			vec4 colorAttenuation = texture(colorAttenuationTransfer, intensity);
			vec3 lightLevel = calculateLightLevel(currentPos, lightPos - currentPos, light1.intensity);
			color += delta * colorAttenuation.rgb * opacity * lightLevel;	// Sum color
			opacity *= pow(1 - colorAttenuation.a, delta);	// Product opacity
			currentPos += cameraRayDirection * delta;
			distanceTravelled += delta;
			iterations++;
		}
	}
	else {
		return vec3(0, 0.1, 0.1);	// Background outside bounding cuboid
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