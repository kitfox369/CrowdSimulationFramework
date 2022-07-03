#ifndef CYLINDER_H
#define CYLINDER_H

#include"../Component.h"

class cylinder {
public:
	Transform myTrns;
	float x, y, angle;
	std::string tag;
	cylinder();
	cylinder(float radius,float length,int rings,int slices);
	void setup();
	void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::mat4& offset,int colorNum);
	void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);
private:

	float m_radius;
	float m_length;
	int m_rings;
	int m_slices;
	int faces;
	int nVerts;
	int index;
	void generateVertexData();
	void generateEndCapVertexData(float y, int& index, int& elIndex);

	ShaderProgram* m_shaderProgram;
	std::vector<float> vertexPositions;
	std::vector<float> vertexNormals;
	std::vector<unsigned int> vertexIndices;
	GLuint vaoHandle;
	GLuint vbo_cube_vertices, vbo_cube_colors;
	GLuint ibo_cube_elements;

};

#endif