#version 420 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding=0) uniform sampler3D voxels;

uniform int windowWidth;
uniform int windowHeight;

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

vec3 calculateIntensity(vec3 cameraRayStart, vec3 cameraRayDirection) {
	vec3 intensity = vec3(0.0);
	vec3 currentPos = cameraRayStart;
	ivec3 generalDirection;
	if (0.0 < cameraRayDirection.x) {generalDirection.x = 1;}
	else if (0.0 > cameraRayDirection.x) {generalDirection.x = -1;}
	else {generalDirection.x = 0;}

	if (0.0 < cameraRayDirection.y) {generalDirection.y = 1;}
	else if (0.0 > cameraRayDirection.y) {generalDirection.y = -1;}
	else {generalDirection.y = 0;}

	if (0.0 < cameraRayDirection.z) {generalDirection.z = 1;}
	else if (0.0 > cameraRayDirection.z) {generalDirection.z = -1;}
	else {generalDirection.z = 0;}
	vec3 resolution = vec3(100, 100, 100);

	for (int i = 0; i < 1000; i++) {
		currentPos += cameraRayDirection * 1.0;
		if (currentPos.x > 0 && currentPos.x < resolution.x 
		&& currentPos.y > 0 && currentPos.y < resolution.y
		&& currentPos.z > 0 && currentPos.z < resolution.z) {
			intensity += interpolateVoxels(voxels, currentPos, resolution);
		}
	}
	//if (0.0 < length(intensity)) {
	//	intensity = normalize(intensity);
	//}
	return intensity * 0.01;
}

void main() {
	vec3 cameraRayStart;
	vec3 cameraRayDirection;
	calculateRayStart(texCoords * 2 - 1, cameraRayStart, cameraRayDirection);
	vec3 intensity = calculateIntensity(cameraRayStart, cameraRayDirection);

	FragColor = vec4(intensity, 1.0f);

}