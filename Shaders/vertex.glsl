#version 330 core
layout (location = 0) in vec2 position;

out vec3 aColor;

uniform mat4 vTransform;
uniform vec3 vColor;

void main(){
	gl_Position = vTransform * vec4(position, 0.0, 1.0);
	aColor = vColor;
}