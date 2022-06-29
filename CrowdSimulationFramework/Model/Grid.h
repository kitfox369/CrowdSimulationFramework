#ifndef GRID_H
#define GRID_H

#include <glm/mat4x4.hpp>

#include<iostream>
#include"../Loader.h"

#include<vector>

#include<glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<math.h>

class grid {
public:
	grid();
	void setup();
	void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);
private:
	ShaderProgram* m_shaderProgram;
	std::vector<glm::vec4> vertexPositions;
	GLuint vaoHandle;
	GLuint vbo_vertexPos, vbo_colors;
	float gridSize = 0;
};

#endif // !GRID_H
