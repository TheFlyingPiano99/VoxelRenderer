#version 420 core

layout(binding=0) uniform sampler2D sourceColorSamplerUnit;
layout(binding=1) uniform sampler2D sourceDepthSamplerUnit;
layout(binding=2) uniform sampler2D tagetDepthSamplerUnit;

in vec2 texCoords;

layout (location = 0) out vec4 FragColor;

void main()
{
	if (texture(sourceDepthSamplerUnit, texCoords).x < texture(tagetDepthSamplerUnit, texCoords).x) {
		FragColor = texture(sourceColorSamplerUnit, texCoords);
	}
}