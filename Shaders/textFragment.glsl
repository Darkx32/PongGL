#version 330 core
in vec2 aTexCoords;
out vec4 FragColor;

uniform sampler2D vText;
uniform vec3 vTextColor;

void main(){
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(vText, aTexCoords).r);
	FragColor = vec4(vTextColor, 1.0) * sampled;
}