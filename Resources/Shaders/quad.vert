#version 420 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 texCoords;

uniform vec2 scale;
uniform vec2 offset;

void main() {
	vec2 normPos = inPos / 2.0 + vec2(0.5);
	gl_Position = vec4((normPos * scale + offset) * 2.0 - vec2(1.0), 0.0, 1.0);
	texCoords = inTexCoords * scale + offset;
}
