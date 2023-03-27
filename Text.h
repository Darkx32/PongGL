#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <map>
#include <iostream>

#include "Shader.h"

struct Character {
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	FT_Pos Advance;
};

class Text {
public:
	Text() : position(glm::vec2()), size(0), color(glm::vec3()), VAO(0), texture(0), VBO(0) {}
	Text(glm::vec2, float, glm::vec3);

	float size;
	glm::vec3 color;

	void Render(GLFWwindow*, std::string);

private:
	Shader shader;
    glm::vec2 position;

	GLuint VAO;
    GLuint VBO;
	GLuint texture;

	std::map<char, Character> Characters;
};

Text::Text(glm::vec2 pos, float s, glm::vec3 c) : position(pos), size(s), color(c), texture(0) {
    this->shader = Shader("Shaders/textVertex.glsl", "Shaders/textFragment.glsl");

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cerr << "Error in init ft" << std::endl;
		exit(-1);
	}

	FT_Face face;
	if (FT_New_Face(ft, "Fonts/gratelos.ttf", 0, &face)) {
		std::cerr << "Error on import font" << std::endl;
		exit(-1);
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		std::cerr << "Error on load char" << std::endl;
		exit(-1);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        glGenTextures(1, &this->texture);
        glBindTexture(GL_TEXTURE_2D, this->texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            this->texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenBuffers(1, &this->VBO);
    glGenVertexArrays(1, &this->VAO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Text::Render(GLFWwindow* window, std::string text) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->shader.Use();

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glm::mat4 ortho = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

    glUniformMatrix4fv(glGetUniformLocation(this->shader.ID, "vOrtho"), 1, GL_FALSE, &ortho[0][0]);
    glUniform3f(glGetUniformLocation(this->shader.ID, "vTextColor"), color.r, color.g, color.b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    std::string::const_iterator c;
    glm::vec2 pos = this->position;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = pos.x + ch.Bearing.x * this->size;
        float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * this->size;

        float w = ch.Size.x * this->size;
        float h = ch.Size.y * this->size;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos,     ypos,       0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f }
        };
        
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        pos.x += (ch.Advance >> 6) * this->size;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}