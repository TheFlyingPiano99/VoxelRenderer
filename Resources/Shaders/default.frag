#version 420 core

layout (location = 0) out vec4 FragColor;

// Imports the world position from the Vertex Shader
in vec4 worldPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

uniform bool useTexture;

uniform sampler2D colorTexture;

struct Light {
	vec4 position;
	vec3 powerDensity;
	mat4 viewProjMatrix;
};

uniform Light lights[16];
uniform unsigned int lightCount;

struct Material {
	float shininess;
	vec3 specularColor;
};
uniform Material material;

struct Camera {
	vec3 position;
	mat4 viewProjMatrix;
	mat4 invViewProjMatrix;
};
uniform Camera camera;


vec3 calculateLight(Light light, vec3 fragPos, vec3 normal, vec3 viewDir)
{
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightDiff = light.position.xyz - fragPos * light.position.w;

	// intensity of light with respect to distance
	float dist = length(lightDiff);
	float attenuation = 1.0 / (dist * dist);

	// diffuse lighting
	vec3 lightDir = normalize(lightDiff);
	float diffuseCos = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = diffuseCos * light.powerDensity * attenuation;
	if (useTexture) {
		diffuse *= texture(colorTexture, texCoord).xyz;
	}
	else {
		diffuse *= color;
	}

	// specular lighting
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularCos = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = specularCos  * material.specularColor * light.powerDensity * attenuation;

	return diffuse + specular;
}



float near = 1.0;
float far = 1000.0;

float linearizeDepth(float depth) {
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness, float offset) {
	float zVal = linearizeDepth(depth);
	return 1 / (1 + exp(-steepness * (zVal - offset)));
}

void main()
{
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(camera.position - worldPos.xyz);
	vec3 lightSum = vec3(0, 0, 0);
	for (int i = 0; i < lightCount; i++) {
		lightSum += calculateLight(lights[i], worldPos.xyz, normal, viewDir);
	}
	FragColor = vec4(lightSum, 1.0f);
}