#version 420 core

out vec4 FragColor;
in vec2 texCoords;

layout(binding=0) uniform sampler3D voxels;
layout(binding=1) uniform sampler1D colorAttenuationTransfer;
layout(binding=2) uniform sampler2D enterTexture;
layout(binding=3) uniform sampler2D exitTexture;

uniform int windowWidth;
uniform int windowHeight;
uniform vec3 resolution;


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

struct Plane {
	vec3 point;
	vec3 normal;
};
uniform Plane intersectionPlane;

uniform float exposure;
uniform float gamma;

float near = 0.1f;
float far = 200.0f;

struct Light {
	vec3 direction;
};
uniform Light light1;

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

//uniform float gaussWeights[10] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216, 0.005, 0.004, 0.003, 0.002, 0.001);

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

float planeIntersection(Plane plane, vec3 rayStart, vec3 rayDir) {
	return dot(plane.normal, plane.point - rayStart) / dot(plane.normal, rayDir);
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

float calculateShade(vec3 currentPos, Light light) {
	float delta = 5.0;
	vec3 samplePos = currentPos + light.direction * delta;
	float opacity = 1.0;
	for (int i = 0; i < 3; i++) {
		if (samplePos.x > 0.0 && samplePos.x < resolution.x
		&& samplePos.y > 0.0 && samplePos.y < resolution.y
		&& samplePos.z > 0.0 && samplePos.z < resolution.z
		&& dot(samplePos - intersectionPlane.point, intersectionPlane.normal) < 0.0) {
			float intensity = texture(voxels, samplePos / resolution).r;
			opacity *= (1 - texture(colorAttenuationTransfer, intensity).a);
		}
		samplePos += light.direction * delta;
	}
	return opacity * 0.8 + 0.2;
}

vec3 calculateColor(vec3 cameraRayStart, vec3 cameraRay) {
	vec3 cameraRayDirection = normalize(cameraRay);
	float rayLength = length(cameraRay);
	//float tIntersectionPlane = planeIntersection(intersectionPlane, cameraRayStart, cameraRayDirection);
	vec3 color = vec3(0.0);
	if (0.00001 < rayLength) {
	/*
		if (0.0 < tIntersectionPlane && dot(cameraRayDirection, intersectionPlane.normal) < 0.0) {
			tNear = tIntersectionPlane;
		}
	*/
		float distanceTravelled = 0.0;
		vec3 currentPos = cameraRayStart;
		int iterations = 0;

		float delta = rayLength / 50.0;
		float opacity = 1.0;
		float shade = 1.0;
		vec3 gradient;
		while (distanceTravelled < rayLength) {
			float intensity = trilinearInterpolation(currentPos, gradient);
			vec4 colorAttenuation = texture(colorAttenuationTransfer, intensity);
			shade = calculateShade(currentPos, light1);
			color += delta * colorAttenuation.rgb * opacity * shade;	// Sum color
			opacity *= pow(1 - colorAttenuation.a, delta);	// Product opacity
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