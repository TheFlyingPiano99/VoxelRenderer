#version 420 core

layout(binding=0) uniform sampler2D sourceColorSamplerUnit;
layout(binding=1) uniform sampler2D sourceDepthSamplerUnit;
layout(binding=6) uniform sampler2D tagetDepthSamplerUnit;

in vec2 texCoords;

layout (location = 0) out vec4 FragColor;

void main()
{
	float sourceDepth = texture(sourceDepthSamplerUnit, texCoords).x;
	if (sourceDepth < texture(tagetDepthSamplerUnit, texCoords).x) {
		gl_FragDepth = sourceDepth;
		FragColor = texture(sourceColorSamplerUnit, texCoords);
	}
}