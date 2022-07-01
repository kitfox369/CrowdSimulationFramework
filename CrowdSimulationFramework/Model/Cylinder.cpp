#include"Cylinder.h"

cylinder::cylinder()
{
	setup();
}

void cylinder::setup()
{

	float Length = 1.0f;

	float cosD = glm::cos(glm::radians(30.0f));
	float sinD = glm::sin(glm::radians(30.0f));

	float tmp = 1 / glm::sqrt(3);

	const float vertexPositions[] = {
		-1 * cosD * Length * tmp, -1 * sinD * Length * tmp, 0.0f, 1.0f,
		0.0f,cosD * Length * tmp,  0.0f, 1.0f,
		cosD * Length * tmp, -1 * sinD * Length * tmp,0.f, 1.0f,
	};


	const float vertexColors[] = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
		1, 1, 0,
	};


	m_shaderProgram = new ShaderProgram();

		//load shaders
	m_shaderProgram->initFromFiles("Shader/diffuse.vert", "Shader/diffuse.frag");

		//create vao
		glGenVertexArrays(1, &vaoHandle);
		glBindVertexArray(vaoHandle);

		//add attributes and uniform vars
		m_shaderProgram->addAttribute("coord3d");
		m_shaderProgram->addAttribute("v_color");

		m_shaderProgram->addUniform("LightLocation");
		m_shaderProgram->addUniform("Kd");
		m_shaderProgram->addUniform("Ld");

		m_shaderProgram->addUniform("color");

		m_shaderProgram->addUniform("mvp");
		m_shaderProgram->addUniform("model");
		m_shaderProgram->addUniform("view");
		m_shaderProgram->addUniform("projection");
		m_shaderProgram->addUniform("location");

		//create vbo for vertices
		glGenBuffers(1, &vbo_cube_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 3, &vertexPositions, GL_STATIC_DRAW);
		glVertexAttribPointer(
			m_shaderProgram->attribute("coord3d"), // attribute
			4,                 // number of elements per vertex, here (x,y,z,1)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
		);
		glEnableVertexAttribArray(m_shaderProgram->attribute("coord3d"));


		//create vbo for colors
		glGenBuffers(1, &vbo_cube_colors);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 4, &vertexColors, GL_STATIC_DRAW);
		glVertexAttribPointer(
			m_shaderProgram->attribute("v_color"), // attribute
			3,                 // number of elements per vertex, here (R,G,B)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
		);
		glEnableVertexAttribArray(m_shaderProgram->attribute("v_color"));


		glBindVertexArray(0);

}

void cylinder::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::mat4& offset, int colorNum)
{

	glm::mat4 mview = view * model;
	glm::mat4 mvp = projection * view * model;

	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));


	glm::vec4 lightPos(50, 50, 50, 1);
	glm::vec3 kd(1, 1, 1);
	glm::vec3 ld(1, 1, 1);

	glm::vec3 color(1, 1, 1);

	switch (colorNum) {
	case 0:
		color = glm::vec3(1, 1, 0);
		break;
	case 1:
		color = glm::vec3(0, 1, 0);
		break;
	case 2:
		color = glm::vec3(0, 0, 1);
		break;
	default:
		color = glm::vec3(1, 1, 1);
		break;
	}

	m_shaderProgram->use();


	glUniformMatrix4fv(m_shaderProgram->uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(m_shaderProgram->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(m_shaderProgram->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(m_shaderProgram->uniform("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(m_shaderProgram->uniform("location"), 1, GL_FALSE, glm::value_ptr(offset));

	glUniform4fv(m_shaderProgram->uniform("LightLocation"), 1, glm::value_ptr(lightPos));
	glUniform3fv(m_shaderProgram->uniform("Kd"), 1, glm::value_ptr(kd));
	glUniform3fv(m_shaderProgram->uniform("Ld"), 1, glm::value_ptr(ld));

	glUniform3fv(m_shaderProgram->uniform("color"), 1, glm::value_ptr(color));

	glBindVertexArray(vaoHandle);

	glDrawArrays(GL_TRIANGLES, 0, 4 * 3);

	/*int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	float tt = size / sizeof(GLushort);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);*/

	m_shaderProgram->disable();

}
