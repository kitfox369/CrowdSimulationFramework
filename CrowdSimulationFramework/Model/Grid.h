#ifndef GRID_H
#define GRID_H

#include"../Component.h"

class grid {
public:
	Transform myTrns;
	float gridSize;
	int gridNum;
	std::string tag;
	grid();
	void setup();
	void update();
	void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);
private:
	ShaderProgram* m_shaderProgram;
	std::vector<glm::vec4> vertexPositions;
	GLuint vaoHandle;
	GLuint vbo_vertexPos, vbo_colors;
	
};

#endif // !GRID_H