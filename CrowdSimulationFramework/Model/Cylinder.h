#ifndef CYLINDER_H
#define CYLINDER_H

#include"../Component.h"

class cylinder {
public:
	Transform myTrns;
	float x, y, angle;
	std::string tag;
	cylinder();
	void setup();
	void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::mat4& offset,int colorNum);
	void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);
private:
	ShaderProgram* m_shaderProgram;
	std::vector<glm::vec4> vertexPositions;
	GLuint vaoHandle;
	GLuint vbo_cube_vertices, vbo_cube_colors;
};

#endif