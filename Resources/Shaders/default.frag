#version 420 core


// Outputs colors in RGBA
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

uniform bool useTexture;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NO_OF_POINT_LIGHTS 4
uniform PointLight pointLights[NO_OF_POINT_LIGHTS];

struct SpotLight {
	vec3 position;
	vec3 direction;
	//TODO
};
uniform SpotLight spotLight;
uniform float shininess;

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;

// Gets the position of the camera from the main function
uniform vec3 camPos;

uniform vec3 camDir;

vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir)
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = light.position - fragPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float attenuation = 1.0 / (light.quadratic * dist * dist + light.linear * dist + light.constant);

	// ambient lighting
	vec3 ambient = light.ambient;

	// diffuse lighting
	vec3 lightDir = normalize(lightVec);
	float diffAmount = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = diffAmount * light.diffuse;
	if (useTexture) {
		diffuse *= texture(diffuse0, texCoord).xyz;;
	}
	else {
		diffuse *= color;
	}
	 
	// specular lighting
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
	vec3 specular = specAmount * light.specular;
	if (useTexture) {
		specular *= texture(specular0, texCoord).r;
	}
	else {
		specular *= 1;
	}

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}


vec3 calculateDirectionalLight(DirLight light, vec3 fragPos, vec3 normal, vec3 viewDir)
{
	// ambient lighting
	vec3 ambient = light.ambient;

	// diffuse lighting
	vec3 lightDir = light.direction;
	float diffAmount = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = diffAmount * light.diffuse * color/*texture(diffuse0, texCoord).xyz*/;

	// specular lighting
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
	vec3 specular = specAmount * light.specular * 1/*texture(specular0, texCoord).r*/;

	return ambient + diffuse + specular;
}



/*
vec4 calculateSpotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}
*/

float near = 0.1f;
float far = 200.0f;

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
	vec3 viewDir = normalize(camPos - crntPos);
	vec3 lightSum = vec3(0, 0, 0);

	//lightSum += calculateDirectionalLight(dirLight, crntPos, normal, viewDir);

	for (int i = 0; i < NO_OF_POINT_LIGHTS; i++) {
		lightSum += calculatePointLight(pointLights[i], crntPos, normal, viewDir);
	}
	//float depth = logisticDepth(gl_FragCoord.z, 0.5, 5.0);
	//FragColor = vec4(lightSum * (1 - depth) + depth * vec3(0.07, 0.13, 0.17), 1.0f);
	FragColor = vec4(lightSum, 1.0f);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));	// Convert to grayscale.
	if (brightness > 1.0) {
		BrightColor = vec4(FragColor.rgb, 1.0);
	}
	else {
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}