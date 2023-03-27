#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <cstdlib>

#include "Ball.h"
#include "Bar.h"
#include "Text.h"

class Game {
public:
	Game();

	void Run() {
		double LastTime = glfwGetTime();
		std::chrono::microseconds delay = std::chrono::microseconds(1000);
		while (!glfwWindowShouldClose(window)) {
			Update();
			
			while (glfwGetTime() < LastTime + 1.0 / 60.0) {
				std::this_thread::sleep_for(delay);
				Render();
			}
			LastTime += 1.0 / 60.0;

			glfwPollEvents();
			glfwSwapBuffers(window);
		}
		Terminate();
	}

private:
	void Render();
	void Update();
	bool IsKeyPress(int key);

	bool CheckCollision(glm::vec4&, glm::vec4&);

	void Terminate() {
		glfwTerminate();
	}

private:
	// Objects for game
	Bar player1;
	Bar player2;
	Ball ball;
	Text score1;
	Text score2;
	Text TextStart;

	// Vars for game
	bool ShowLines = true;
	bool StartGame = false;
	int width, height = (0, 0);
	unsigned int points1, points2;
	glm::vec2 BallVelocity = glm::vec2(5.0f, 5.0f);
	int BallStartDirection;
	
	// Vars for GLFW
	GLFWwindow* window;
};

Game::Game() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(800, 600, "Ping Pong - OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Error in create Window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Error in start glad" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	glfwGetWindowSize(window, &this->width, &this->height);

	this->points1 = 0;
	this->points2 = 0;
	this->BallStartDirection = (rand() & 1) ? 1 : -1;
	this->BallVelocity = glm::vec2(3.5f * BallStartDirection, 3.5f * -BallStartDirection);

	this->player1 = Bar(glm::vec2(15.f), glm::vec2(20.f, 60.f), glm::vec3(1.0f));
	this->player2 = Bar(glm::vec2((float)(this->width - 15) - 20.f, 15.f), glm::vec2(20.f, 60.f), glm::vec3(1.0f));
	this->ball = Ball(glm::vec2(this->width / 2.0f + 10.0f, this->height / 2.0f + 10.0f), 20.f, glm::vec3(1.0f));
	this->score1 = Text(glm::vec2((float)this->width / 2.0f - 100.0f, (float)this->height / 2.0f - 20.0f), 1.0f, glm::vec3(1.0f));
	this->score2 = Text(glm::vec2((float)this->width / 2.0f + 100.0f, (float)this->height / 2.0f - 20.0f), 1.0f, glm::vec3(1.0f));
	this->TextStart = Text(glm::vec2((float)this->width / 2.0f - 150.0f, (float)this->height / 2.0f - 20.0f), 1.0f, glm::vec3(1.0f));
}

void Game::Update() {
	#pragma region Show Lines or fill

	if (this->IsKeyPress(GLFW_KEY_F1) && ShowLines) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShowLines = false;
	}
	else if (this->IsKeyPress(GLFW_KEY_F1) && !ShowLines) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		ShowLines = true;
	}
	#pragma endregion

	// Start game if space is pressed
	if (this->IsKeyPress(GLFW_KEY_SPACE))
		this->StartGame = true;

	#pragma region Movement player 1
	
	if (this->IsKeyPress(GLFW_KEY_W) && this->player1.position.y >= 20 ) 
	{
		this->player1.position += glm::vec2(0.0f, -10.f);
	}
	if (this->IsKeyPress(GLFW_KEY_S) && this->player1.position.y + 
		this->player1.size.y <= this->height - 20) 
	{
		this->player1.position += glm::vec2(0.0f, 10.f);
	}

	#pragma endregion

	#pragma region Movement player 2

	if (this->IsKeyPress(GLFW_KEY_UP) && this->player2.position.y >= 20)
	{
		this->player2.position += glm::vec2(0.0f, -10.f);
	}
	if (this->IsKeyPress(GLFW_KEY_DOWN) && this->player2.position.y +
		this->player2.size.y <= this->height - 20)
	{
		this->player2.position += glm::vec2(0.0f, 10.f);
	}

	#pragma endregion

	#pragma region Movement and Collision Ball
	if (this->StartGame) {
		this->ball.position += this->BallVelocity;

		// Collision for walls
		if (ball.position.y <= 0) {
			this->BallVelocity.y = -this->BallVelocity.y;
		}
		if (ball.position.y + this->ball.radius >= this->height) {
			this->BallVelocity.y = -this->BallVelocity.y;
		}
		
		// Collision for pads
		glm::vec4 BallRect = glm::vec4(this->ball.position, glm::vec2(10.0f));
		glm::vec4 Player1Rect = glm::vec4(this->player1.position, this->player1.size);
		glm::vec4 Player2Rect = glm::vec4(this->player2.position, this->player2.size);
		
		if (this->CheckCollision(BallRect, Player1Rect)) {
			this->BallVelocity.x = -this->BallVelocity.x;
		}
		if (this->CheckCollision(BallRect, Player2Rect)) {
			this->BallVelocity.x = -this->BallVelocity.x;
		}

		// Check players mark point
		if (ball.position.x <= 0) {
			this->ball.position = glm::vec2(this->width / 2.0f + 10.0f, this->height / 2.0f + 10.0f);
			this->points2++;
			this->StartGame = false;
		}
		if (ball.position.x + this->ball.radius >= this->width) {
			this->ball.position = glm::vec2(this->width / 2.0f + 10.0f, this->height / 2.0f + 10.0f);
			this->points1++;
			this->StartGame = false;
		}
	}

	#pragma endregion
}

void Game::Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	this->player1.Render(this->window);
	this->player2.Render(this->window);
	this->ball.Render(this->window);

	// Show points if game is start
	if (this->StartGame) {
		this->score1.Render(this->window, std::to_string(this->points1));
		this->score2.Render(this->window, std::to_string(this->points2));
	}

	// Unshow text if game is start
	if (!this->StartGame)
		this->TextStart.Render(this->window, "Press Space to start");
}

bool Game::IsKeyPress(int key) {
	bool ret = false;
	const int state = glfwGetKey(this->window, key);
	if (state == GLFW_PRESS) ret = true;
	return ret;
}

bool Game::CheckCollision(glm::vec4& one, glm::vec4& two){
	// collision x-axis?
	bool collisionX = one.x + one.z >= two.x &&
		two.x + two.z >= one.x;
	// collision y-axis?
	bool collisionY = one.y + one.w >= two.y &&
		two.y + two.w >= one.y;
	// collision only if on both axes
	return collisionX && collisionY;
}