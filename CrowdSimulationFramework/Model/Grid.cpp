#include"Grid.h"

grid::grid() {
	myTrns = { {0,0,0},{0,0,0},{0,0,0} };
	gridSize = 100.0f;
	gridNum = 25;
	tag = "grid";
	setup();
}

void grid::setup() {
	vertexPositions.clear();
	for (int i = -gridSize; i <= gridSize; i += gridSize / gridNum)
	{
		vertexPositions.push_back(glm::vec4(i, -gridSize, 0.0f, 1.0f));
		vertexPositions.push_back(glm::vec4(i, gridSize, 0.0f, 1.0f));
		vertexPositions.push_back(glm::vec4(-gridSize, i, 0.0f, 1.0f));
		vertexPositions.push_back(glm::vec4(gridSize, i, 0.0f, 1.0f));
	}

	m_shaderProgram = new ShaderProgram();

	//load shaders
	m_shaderProgram->initFromFiles("Shader/black.vert", "Shader/black.frag");

	//create vao
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	//add attributes and uniform vars
	m_shaderProgram->addAttribute("coord3d");
	m_shaderProgram->addUniform("mvp");

	//create vbo for vertices
	glGenBuffers(1, &vbo_vertexPos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertexPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * vertexPositions.size(), vertexPositions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		m_shaderProgram->attribute("coord3d"), // attribute
		4,                 // number of elements per vertex, here (x,y,z,1)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);
	glEnableVertexAttribArray(m_shaderProgram->attribute("coord3d"));

	glBindVertexArray(0);

}

void grid::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) {
	glm::mat4 mview = view * model;
	glm::mat4 mvp = projection * view * model;

	m_shaderProgram->use();

	glUniformMatrix4fv(m_shaderProgram->uniform("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

	glBindVertexArray(vaoHandle);

	glDrawArrays(GL_LINES, 0, 4 * vertexPositions.size());
}