#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 aTexCoords;

uniform mat4 vOrtho;

void main(){
	gl_Position = vOrtho * vec4(vertex.xy, 0.0, 1.0);
	aTexCoords = vertex.zw;
}