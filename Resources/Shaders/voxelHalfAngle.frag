#version 420 core

/*
	Half angle method
*/

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragOpacity;
in vec2 texCoords;

layout (binding = 0) uniform sampler3D voxels;
layout (binding = 1) uniform sampler2D colorAttenuationTransfer;
layout (binding = 2) uniform sampler2D enterTexture;
layout (binding = 3) uniform sampler2D exitTexture;
layout (binding = 4) uniform sampler2D opacityTexture;
layout (binding = 5) uniform samplerCube skybox;
layout (binding = 6) uniform sampler2D targetDepthTexture;

layout (binding = 7) uniform sampler2D lightExitTexture0;
layout (binding = 8) uniform sampler2D lightExitTexture1;
layout (binding = 9) uniform sampler2D lightExitTexture2;
layout (binding = 10) uniform sampler2D lightExitTexture3;
layout (binding = 11) uniform sampler2D lightExitTexture4;
layout (binding = 12) uniform sampler2D lightExitTexture5;
layout (binding = 13) uniform sampler2D lightExitTexture6;

uniform vec3 resolution;
uniform mat4 modelMatrix;
uniform mat4 invModelMatrix;

uniform float shininess;
uniform vec3 specularColor;
uniform vec3 ambientColor;

uniform vec3 slicePosition;
uniform vec3 halfway;
uniform float delta;

struct Camera {
	mat4 viewProjMatrix;
};
uniform Camera camera;

struct Light {
	vec4 position;
	vec3 powerDensity;
	mat4 viewProjMatrix;
};
uniform Light lights[16];
uniform unsigned int lightCount;

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 n) {
	const vec2 d = vec2(0.0, 1.0);
  vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

vec3 intersectPlane(vec3 linePos, vec3 lineDir, vec3 planePos, vec3 planeNormal) {
	float t = dot(planePos - linePos, planeNormal) / dot(lineDir, planeNormal);
	return linePos + lineDir * t;
}

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


vec3 calculateLightLevel(
	vec3 beamOffset, 
	vec3 modelPos, 
	vec3 diffuseColor, 
	vec3 specularColor, 
	float shininess, 
	Light light, 
	vec3 gradient, 
	vec3 modelEyePos, 
	vec3 lightDiff,
	float opacity
) {
	vec3 lightDir = normalize(lightDiff + beamOffset);
	float lightDistance = length(lightDiff);
	if (light.position.w == 0.0) {
		lightDistance = 1.0;
	}
	vec3 normal = normalize(-gradient);
	vec3 viewDir = normalize(modelEyePos - modelPos);
	vec3 reflectionDir = reflect(-viewDir, normal);
	vec3 localHalfway = normalize(viewDir + lightDir);
	float specularCos = pow(max(dot(normal, localHalfway), 0.0), shininess);
	float diffuseCos = max(dot(normal, lightDir), 0.0);
	vec3 intensity = max((1 - opacity), 0) * light.powerDensity / lightDistance / lightDistance;
	return diffuseColor * intensity * diffuseCos 
		+ specularColor * intensity * specularCos * length(gradient);
}

vec4 calculateColor(vec3 enter, vec3 exit) {
	
	vec3 position = intersectPlane(enter, normalize(exit - enter), slicePosition, halfway);
	
	//	Calculating depth
	vec4 camSpace = camera.viewProjMatrix * modelMatrix * vec4(position, 1);
	camSpace = camSpace / camSpace.w;
	float depth = 0.5 * (camSpace.z + 1);
	float targetDepth = texture(targetDepthTexture, texCoords).x;

	//float opacity = texture(opacityTexture, texCoords).x;
	vec4 lightModelSpacePos = invModelMatrix * lights[0].position;
	float lightDistance;
	vec3 lightDiff;
	if (lightModelSpacePos.w == 0.0) {
		lightDiff = lightModelSpacePos.xyz - position * lights[0].position.w;
		lightDistance = 1.0;
	}
	else {
		lightModelSpacePos /= lightModelSpacePos.w;
		lightDiff = lightModelSpacePos.xyz - position * lights[0].position.w;
		lightDistance = length(lightDiff);
	}
	vec3 lightDir = normalize(lightDiff);
	vec3 opacitySampleModelSpacePos = intersectPlane(position, lightDir, position + delta * halfway, halfway);
	vec4 opacitySampleCamSpacePos = camera.viewProjMatrix * modelMatrix * vec4(opacitySampleModelSpacePos, 1.0);
	opacitySampleCamSpacePos /= opacitySampleCamSpacePos.w;
	float lightDirAttenuation = texture(opacityTexture, opacitySampleCamSpacePos.xy * 0.5 + vec2(0.5)).a;
	vec4 color = vec4(0.0);

	if (length(exit - enter) > 0.00000001 
		&& dot(normalize(position - enter), normalize(exit - enter)) > 0.0 
		&& dot(normalize(position - exit), normalize(enter - exit)) > 0.0 
		&& depth < targetDepth) {	// Test position validity
		vec4 gradientIntesity = resampleGradientAndDensity(position, trilinearInterpolation(position));
		vec4 colorAttenuation = texture(colorAttenuationTransfer, vec2(gradientIntesity.w, length(gradientIntesity.xyz)));
		vec3 lightLevel = vec3(0.0);
		vec2 p = position.xy;
		lightLevel += calculateLightLevel(vec3(noise(p), noise(p), noise(p)), position, colorAttenuation.rgb, specularColor, shininess, lights[0], gradientIntesity.xyz, enter, lightDiff, lightDirAttenuation);
		lightLevel += calculateLightLevel(vec3(noise(p), noise(p), noise(p)), position, colorAttenuation.rgb, specularColor, shininess, lights[0], gradientIntesity.xyz, enter, lightDiff, lightDirAttenuation);
		lightLevel += calculateLightLevel(vec3(noise(p), noise(p), noise(p)), position, colorAttenuation.rgb, specularColor, shininess, lights[0], gradientIntesity.xyz, enter, lightDiff, lightDirAttenuation);
		lightLevel /= 3.0;
		vec3 viewDir = normalize(enter - exit);
		float cosHalfway = dot(viewDir, halfway);
		color.rgb = lightLevel * delta / cosHalfway;
		color.a = colorAttenuation.a * delta / cosHalfway;
		gl_FragDepth = depth;
		lightDirAttenuation += color.a;
	}
	FragOpacity = vec4(0, 0, 0, lightDirAttenuation);
	return color;
}

void main() {
	vec3 enter = texture(enterTexture, texCoords).xyz;
	vec3 exit = texture(exitTexture, texCoords).xyz;
	FragColor = calculateColor(enter, exit);
}