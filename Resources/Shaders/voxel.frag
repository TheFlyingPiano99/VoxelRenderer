#version 420 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding=0) uniform sampler3D voxels;
layout(binding=1) uniform sampler1D colorAttenuationTransfer;

uniform int windowWidth;
uniform int windowHeight;

uniform float maxIntensity;
uniform float maxAttenuation;

float widthOffset = 1.0 / 2000.0f;
float heightOffset = 1.0 / 2000.0f;

#define USED_KERNEL_SIZE 9
#define NUMBER_OF_STARS 750

uniform vec3 stars[NUMBER_OF_STARS];
uniform vec3 starColor;

// Itt volt compile error 
// Eddig az init így nézett ki: vec2 myVec2[n] = { ... };
// A fix: vec2 myVec2[n] = vec2[]( ... );
vec2 adjacentOffset[9] = vec2[](
	vec2(-widthOffset, -heightOffset),	vec2(0, -heightOffset), vec2(widthOffset, -heightOffset),
	vec2(-widthOffset, 0),				vec2(0, 0),				vec2(widthOffset, 0),
	vec2(-widthOffset, heightOffset),	vec2(0, heightOffset),	vec2(widthOffset, heightOffset)
);

vec2 surroundingOffset[25] = vec2[](
	2 * vec2(-widthOffset, -heightOffset),	2 * vec2(-widthOffset, -heightOffset),	2 * vec2(0, -heightOffset),		vec2(widthOffset, -heightOffset),	2 * vec2(widthOffset, -heightOffset),
	2 * vec2(-widthOffset, -heightOffset),		vec2(-widthOffset, -heightOffset),		vec2(0, -heightOffset),		vec2(widthOffset, -heightOffset),	2 * vec2(widthOffset, -heightOffset),
	2 * vec2(-widthOffset, 0),					vec2(-widthOffset, 0),					vec2(0, 0),					vec2(widthOffset, 0),				2 * vec2(widthOffset, 0),
	2 * vec2(-widthOffset, heightOffset),		vec2(-widthOffset, heightOffset),		vec2(0, heightOffset),		vec2(widthOffset, heightOffset),	2 * vec2(widthOffset, heightOffset),
	2 * vec2(-widthOffset, heightOffset),	2 * vec2(-widthOffset, heightOffset),	2 * vec2(0, heightOffset),	2 * vec2(widthOffset, heightOffset),	2 * vec2(widthOffset, heightOffset)
);


float blurKernel[9] = float[](
	1/9.0,		1/9.0,		1/9.0,
	1/9.0,		1/9.0,		1/9.0,
	1/9.0,		1/9.0,		1/9.0
);

float greaterBlurKernel[25] = float[](
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0,
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0,
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0,
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0,
	1/25.0,		1/25.0,		1/25.0,		1/25.0,		1/25.0
);

float outlineKernel[9] = float[](
	1, 1, 1,
	1, -8, 1,
	1, 1, 1
);

struct Camera {
	vec3 eye;
	vec3 center;
	vec3 up;
	vec3 right;
	float FOVrad;
	float aspectRatio;
	mat4 Mat;
	mat4 invMat;
};
uniform Camera camera;
uniform Camera lightCamera;

struct Atmosphere {
	vec3 center;
	float radius;
	float planetRadius;
	

	vec3 rayleighScattering;
	float mieScattering;
	float heightOfAverageDensity;


	vec3 quadraticAbsorption;
	vec3 linearAbsorption;
	vec3 constantAbsorption;
	vec3 quadraticScattering;
	vec3 linearScattering;
	vec3 constantScattering;
	vec3 quadratiReflectiveness;
	vec3 linearReflectiveness;
	vec3 constantReflectiveness;
	float quadratiDensity;
	float linearDensity;
	float constantDensity;
};
uniform Atmosphere atmosphere;

struct Sun {
	vec3 position;
	vec3 color;
};
uniform Sun sun;

uniform float exposure;
uniform float gamma;

float near = 0.1f;
float far = 200.0f;

float linearizeDepth(float depth) {
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness, float offset) {
	float zVal = linearizeDepth(depth);
	return 1 / (1 + exp(-steepness * (zVal - offset)));
}

vec3 decodeLocation(mat4 invMat, float depthValue, vec2 texCoord)
{
  vec4 clipSpaceLocation;
  clipSpaceLocation.xy = texCoord * 2.0f - 1.0f;
  clipSpaceLocation.z = depthValue * 2.0f - 1.0f;
  clipSpaceLocation.w = 1.0f;
  vec4 homogenousLocation = invMat * clipSpaceLocation;
  return homogenousLocation.xyz / homogenousLocation.w;
}

vec2 calculateLightCameraTexCoord(vec3 point) {
	vec4 transformed = lightCamera.Mat * vec4(point, 1.0);
	return vec2(transformed.xy / transformed.w) / 2.0f + 0.5f;
}

vec3 postprocess(vec2 offset[USED_KERNEL_SIZE], float kernel[USED_KERNEL_SIZE], sampler2D sampleTexture, vec2 texCoord) {
	vec3 color = vec3(0.0);
	for (int i = 0; i < USED_KERNEL_SIZE; i++) {
		vec2 offsettedTexCoord = vec2(max(min((texCoord + offset[i]).x, 1), 0), max(min((texCoord + offset[i]).y, 1), 0));
		color += (texture(sampleTexture, offsettedTexCoord).xyz) * kernel[i];
	}
	return color;
}

uniform float gaussWeights[10] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216, 0.005, 0.004, 0.003, 0.002, 0.001);

void calculateRayStart(vec2 normalCameraCoord, out vec3 rayStart, out vec3 rayDir) {
	float scale = tan(camera.FOVrad / 2.0);
	rayStart = camera.center
			+ camera.right * camera.aspectRatio * scale * normalCameraCoord.x
			+ camera.up * scale * normalCameraCoord.y;
	rayDir = normalize(rayStart - camera.eye);
	rayStart = camera.eye;
}

bool solveQuadratic(float a, float b, float c, out float x1, out float x2) {
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0.0) {
		return false;
	}
	x1 = (-b + sqrt(discriminant)) / (2.0 * a);
	x2 = (-b - sqrt(discriminant)) / (2.0 * a);
	return true;
}

bool intersectSphere(vec3 rayDir, vec3 rayPos, vec3 sphereCenter, float sphereRadius, out float shortDist, out float longDist){
	float a = dot(rayDir, rayDir);
	float b = 2.0 * (dot(rayDir, rayPos - sphereCenter)); 
	float c = dot(rayPos - sphereCenter, rayPos - sphereCenter) - sphereRadius * sphereRadius;
	return solveQuadratic(a, b, c, longDist, shortDist);
}

bool intersectCone(vec3 rayDir, vec3 rayPos, vec3 coneTip, vec3 coneDir, float coneHalfAngle, out float shortDist, out float longDist){
	float DdotV = dot(rayDir, coneDir);
	float cosTheta =cos(coneHalfAngle);
	float cosThetaSquare = cosTheta * cosTheta;
	vec3 CO = rayPos - coneTip;
	float a = DdotV * DdotV - cosThetaSquare;
	float b = 2.0 * (DdotV * dot(CO, coneDir) - dot(rayDir, CO * cosThetaSquare)); 
	float c = dot(CO, coneDir) * dot(CO, coneDir) - dot(CO, CO * cosThetaSquare);
	return solveQuadratic(a, b, c, longDist, shortDist);
}

vec3 interpolateVoxels(sampler3D voxelsToInterpolate, vec3 currentPos, vec3 resolution) {
	vec3 offsetKernel[6] = {
		vec3(1, 0, 0),
		vec3(-1, 0, 0),
		vec3(0, 1, 0),
		vec3(0, -1, 0),
		vec3(0, 0, 1),
		vec3(0, 0, -1)
	};
	vec3 centerOfVoxel = vec3(int(currentPos.x), int(currentPos.y), int(currentPos.z)) + vec3(0.5);
	float r = length(currentPos - centerOfVoxel);
	vec3 sum = texture(voxelsToInterpolate, centerOfVoxel / resolution).xyz * (sqrt(0.75) - r) / sqrt(0.75);
	for (int i = 0; i < 6; i++) {
		vec3 centerOfOffsetted = centerOfVoxel + offsetKernel[i];
		if (0.0 < centerOfOffsetted.x && resolution.x > centerOfOffsetted.x
		&& 0.0 < centerOfOffsetted.y && resolution.y > centerOfOffsetted.y
		&& 0.0 < centerOfOffsetted.z && resolution.z > centerOfOffsetted.z) {
			float w = length(currentPos - centerOfOffsetted);
			sum += texture(voxelsToInterpolate, centerOfOffsetted / resolution).xyz * length(2 - w) / 2;
		}
	}
	return sum;
}

float rectangleIntersection(vec3 p, vec3 sideA, vec3 sideB, vec3 rayStart, vec3 rayDir) {
	vec3 norm = normalize(cross(sideA, sideB));
	float t = dot(norm, p - rayStart) / dot(norm, rayDir);
	if (0.0 > t) {
		return -1.0;
	}
	vec3 intersection = rayStart + t * rayDir;
	if (dot(sideA, intersection - p) > 0.0
	&& dot(sideB, intersection - p) > 0.0
	&& dot(-sideA, intersection - (p + sideA + sideB)) > 0.0
	&& dot(-sideB, intersection - (p + sideA + sideB)) > 0.0) {
		return t;
	}
	else {
		return -1.0;
	}
}

void updateT(float t, out float tNear, out float tFar) {
	if (0.0 < t) {
		if (t < tNear) {
			tNear = t;
		}
		if (t > tFar) {
			tFar = t;
		}
	}
}

void cubeIntersection(vec3 p, vec3 dimensions, vec3 rayStart, vec3 rayDir, out float tNear, out float tFar) {
	tNear = -1.0;
	tFar = -1.0;
	float t = rectangleIntersection(p, vec3(1, 0, 0) * dimensions, vec3(0, 1, 0) * dimensions, rayStart, rayDir);
	if (0.0 < t) {
		tNear = t;
		tFar = t;
	}
	t = rectangleIntersection(p, vec3(0, 1, 0) * dimensions, vec3(0, 0, 1) * dimensions, rayStart, rayDir);
	if (0.0 < t) {
		if (t < tNear) {
			tNear = t;
		}
		if (t > tFar) {
			tFar = t;
		}
	}	t = rectangleIntersection(p, vec3(0, 0, 1) * dimensions, vec3(1, 0, 0) * dimensions, rayStart, rayDir);
	if (0.0 < t) {
		if (t < tNear) {
			tNear = t;
		}
		if (t > tFar) {
			tFar = t;
		}
	}	t = rectangleIntersection(dimensions, vec3(0, 0, -1) * dimensions, vec3(0, -1, 0) * dimensions, rayStart, rayDir);
	if (0.0 < t) {
		if (t < tNear) {
			tNear = t;
		}
		if (t > tFar) {
			tFar = t;
		}
	}	t = rectangleIntersection(dimensions, vec3(-1, 0, 0) * dimensions, vec3(0, 0, -1) * dimensions, rayStart, rayDir);
	if (0.0 < t) {
		if (t < tNear) {
			tNear = t;
		}
		if (t > tFar) {
			tFar = t;
		}
	}	t = rectangleIntersection(dimensions, vec3(-1, 0, 0) * dimensions, vec3(0, -1, 0) * dimensions, rayStart, rayDir);
	if (0.0 < t) {
		if (t < tNear) {
			tNear = t;
		}
		if (t > tFar) {
			tFar = t;
		}
	}}

float calculateStep(out int prevCoord, out vec3 stepSizes, vec3 scalars, vec3 currentPos, ivec3 generalDir, out ivec3 voxel) {
	vec3 inVoxelOffset = currentPos - vec3(voxel);
	vec3 delta = vec3(0.0);
	if (prevCoord == -1) {	//First time
		if (generalDir.x > 0) {
			delta.x = 1 - inVoxelOffset.x;
			stepSizes.x = scalars.x * delta.x;
		}
		else if (generalDir.x < 0) {
			delta.x = inVoxelOffset.x;
			stepSizes.x = scalars.x * delta.x;
		}

		if (generalDir.y > 0) {
			delta.y = 1 - inVoxelOffset.y;
			stepSizes.y = scalars.y * delta.y;
		}
		else if (generalDir.y < 0) {
			delta.y = inVoxelOffset.y;
			stepSizes.y = scalars.y * delta.y;
		}

		if (generalDir.z > 0) {
			delta.z = 1 - inVoxelOffset.z;
			stepSizes.z = scalars.z * delta.z;
		}
		else if (generalDir.z < 0) {
			delta.z = inVoxelOffset.z;
			stepSizes.z = scalars.z * delta.z;
		}
	}
	else {
		if (generalDir[prevCoord] > 0) {
			delta[prevCoord] = 1 - inVoxelOffset[prevCoord];
		}
		else if (generalDir[prevCoord] < 0) {
			delta[prevCoord] = inVoxelOffset[prevCoord];
		}
		stepSizes[prevCoord] = scalars[prevCoord] * delta[prevCoord];
	}
	//Find minimal step axis:
	float minimalStep = stepSizes[0];
	int minIdx = 0;
	for (int i = 1; i < 3; i++) {
		if (minimalStep > stepSizes[i]) {
			minimalStep = stepSizes[i];
			minIdx = i;
		}
	}
	prevCoord = minIdx;
	voxel[prevCoord] += generalDir[prevCoord];
	//return stepSizes[prevCoord];
	return 1.0;
}

vec3 calculateColor(vec3 cameraRayStart, vec3 cameraRayDirection) {
	ivec3 generalDirection;
	if (0.0 < cameraRayDirection.x) {generalDirection.x = 1;}
	else if (0.0 > cameraRayDirection.x) {generalDirection.x = -1;}

	if (0.0 < cameraRayDirection.y) {generalDirection.y = 1;}
	else if (0.0 > cameraRayDirection.y) {generalDirection.y = -1;}

	if (0.0 < cameraRayDirection.z) {generalDirection.z = 1;}
	else if (0.0 > cameraRayDirection.z) {generalDirection.z = -1;}
	vec3 resolution = vec3(100, 100, 100);


	//Calculate bounding cube intersection:
	float tNear, tFar;
	cubeIntersection(vec3(0.0), resolution, cameraRayStart, cameraRayDirection, tNear, tFar);
	vec3 color = vec3(0.0);
	if (0.0 < tFar) {
		if (tFar - tNear < 0.001) {
			tNear = 0.0;
		}
		float distanceTravelled = tNear;
		vec3 currentPos = cameraRayStart + tNear * cameraRayDirection;
		ivec3 currentVoxel = ivec3(currentPos);
		float delta;
		vec3 scalar = 1.0 / cameraRayDirection;
		int prevCoord = -1;
		vec3 stepSizes = vec3(-1);
		int iterations = 0;
		float attenuation = 0.0;
		while (distanceTravelled < tFar && iterations < 1000) {
			delta = calculateStep(prevCoord, stepSizes, scalar, currentPos, generalDirection, currentVoxel);
			if (currentPos.x >= 0 && currentPos.y >= 0 && currentPos.z >= 0.0
			&& currentPos.x < resolution.x && currentPos.y < resolution.y && currentPos.z < resolution.z) {
				float intensity = texture(voxels, currentPos / resolution).x;
				attenuation += texture(colorAttenuationTransfer, intensity).a;
				if (attenuation > 1.0) {
					break;
				}
				color += delta * texture(colorAttenuationTransfer, intensity).rgb * (1 - attenuation);
			}
			currentPos += cameraRayDirection * delta;
			distanceTravelled += delta;
			iterations++;
		}
	}
	else {
		return vec3(0, 0.1, 0.1);	// Background outside bounding cube
	}
	return color;
}


void main() {
	vec3 cameraRayStart;
	vec3 cameraRayDirection;
	calculateRayStart(texCoords * 2 - 1, cameraRayStart, cameraRayDirection);
	FragColor = vec4(calculateColor(cameraRayStart, cameraRayDirection), 1.0f);
}