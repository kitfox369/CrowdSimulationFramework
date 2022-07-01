#ifndef CYLINDER_H
#define CYLINDER_H

#include"../Component.h"

class cylinder {
	Transform myTrns;
	float gridSize;
	int gridNum;
	std::string tag;
	cylinder();
	void setup();
	void update();
	void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);
private:
	ShaderProgram* m_shaderProgram;
	std::vector<glm::vec4> vertexPositions;
	GLuint vaoHandle;
	GLuint vbo_vertexPos, vbo_colors;
};

#endif