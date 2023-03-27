#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

class Ball {
public:
	Ball() : position(glm::vec2()), radius(0), color(glm::vec3()), VAO(0) {}
	Ball(glm::vec2, float, glm::vec3);

	glm::vec2 position;
	float radius;
	glm::vec3 color;
	
	void Render(GLFWwindow*);

private:
	GLuint VAO;
	Shader shader;
};

Ball::Ball(glm::vec2 pos, float r, glm::vec3 c) : position(pos), radius(r), color(c) {
	this->shader = Shader("Shaders/ballVertex.glsl", "Shaders/ballFragment.glsl");

	GLfloat vertices[8] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f, 1.0f,
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

void Ball::Render(GLFWwindow* window) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glm::mat4 ortho = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(this->position, 0.0f));
	transform = glm::scale(transform, glm::vec3(glm::vec2(this->radius / 2.0f), 1.0f));
	transform = ortho * transform;

	this->shader.Use();

	int TransformUniformLocation = glGetUniformLocation(this->shader.ID, "vTransform");
	int ColorUniformLocation = glGetUniformLocation(this->shader.ID, "vColor");
	int RadiusUniformLocation = glGetUniformLocation(this->shader.ID, "vRadius");

	glUniformMatrix4fv(TransformUniformLocation, 1, GL_FALSE, &transform[0][0]);
	glUniform3f(ColorUniformLocation, color.r, color.g, color.b);
	glUniform1f(RadiusUniformLocation, radius);

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}