#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Bar {
public:
	Bar() : position(glm::vec2()), size(glm::vec2()), color(glm::vec3()), VAO(0) {}
	Bar(glm::vec2 pos, glm::vec2 s, glm::vec3 c);

	glm::vec2 position;
	glm::vec2 size;
	glm::vec3 color;

	void Render(GLFWwindow*);

private:
	GLuint VAO;

	Shader shader;
};

Bar::Bar(glm::vec2 pos, glm::vec2 s, glm::vec3 c) : position(pos), size(s), color(c){
	this->shader = Shader("Shaders/vertex.glsl", "Shaders/fragment.glsl");

	GLfloat vertices[8] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	GLuint indices[6] = {
		0, 1, 2,
		3, 2, 1
	};

	GLuint VBO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &this->VAO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
}

void Bar::Render(GLFWwindow* window) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 ortho = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(this->position, 0.0f));
	transform = glm::scale(transform, glm::vec3(this->size, 1.0f));
	transform = ortho * transform;

	this->shader.Use();

	int TransformUniformLocation = glGetUniformLocation(this->shader.ID, "vTransform");
	int ColorUniformLocation = glGetUniformLocation(this->shader.ID, "vColor");

	glUniformMatrix4fv(TransformUniformLocation, 1, GL_FALSE, &transform[0][0]);
	glUniform3f(ColorUniformLocation, color.r, color.g, color.b);

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}