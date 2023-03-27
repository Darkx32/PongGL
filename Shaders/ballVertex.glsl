#version 330 core
layout (location = 0) in vec2 position;

out vec4 aColor;
out vec2 aPosition;
out float aRadius;

uniform mat4 vTransform;
uniform vec3 vColor;
uniform float vRadius;

void main(){
	gl_Position = vTransform * vec4(position, 0.0, 1.0);

	aColor = vec4(vColor, 1.0);
	aPosition = position;
	aRadius = vRadius;
}