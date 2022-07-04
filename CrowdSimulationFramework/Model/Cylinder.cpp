#include"Cylinder.h"

const float pi = 3.14159265358979323846;
const float twoPi = 2.0 * pi;

cylinder::cylinder():m_radius(1.0f), m_length(2.0f), m_rings(10), m_slices(30){
	setup();
}

cylinder::cylinder(float radius, float length, int rings, int slices):m_radius(radius),m_length(length),m_rings(rings),m_slices(slices)
{
	setup();
}

void cylinder::generateVertexData()
{
	faces = m_slices * m_rings;           // Number of "rectangular" faces
	nVerts = (m_slices + 1) * m_rings  // Sides
		+ 2 * (m_slices + 1);       // 2 x endcaps

// Resize vector to hold our data
	vertexPositions.resize(3 * nVerts);
	vertexNormals.resize(3 * nVerts);
	vertexIndices.resize(6 * faces + 2 * 3 * m_slices);

	// The side of the cylinder
	const float dTheta = twoPi / static_cast<float>(m_slices);
	const float dy = m_length / static_cast<float>(m_rings - 1);

	// Iterate over heights (rings)
	index = 0;

	for (int ring = 0; ring < m_rings; ++ring)
	{
		const float y = -m_length / 2.0f + static_cast<float>(ring) * dy;

		// Iterate over slices (segments in a ring)
		for (int slice = 0; slice < m_slices + 1; ++slice)
		{
			float theta = static_cast<float>(slice) * dTheta;
			float cosTheta = cosf(theta);
			float sinTheta = sinf(theta);

			vertexPositions[index] = m_radius * cosTheta;
			vertexPositions[index + 1] = y;
			vertexPositions[index + 2] = m_radius * sinTheta;

			vertexNormals[index] = cosTheta;
			vertexNormals[index + 1] = 0.0f;
			vertexNormals[index + 2] = sinTheta;

			index += 3;
		}
	}

	int elIndex = 0;
	for (int i = 0; i < m_rings - 1; ++i)
	{
		const int ringStartIndex = i * (m_slices + 1);
		const int nextRingStartIndex = (i + 1) * (m_slices + 1);

		for (int j = 0; j < m_slices; ++j)
		{
			// Split the quad into two triangles
			vertexIndices[elIndex] = ringStartIndex + j;
			vertexIndices[elIndex + 1] = ringStartIndex + j + 1;
			vertexIndices[elIndex + 2] = nextRingStartIndex + j;
			vertexIndices[elIndex + 3] = nextRingStartIndex + j;
			vertexIndices[elIndex + 4] = ringStartIndex + j + 1;
			vertexIndices[elIndex + 5] = nextRingStartIndex + j + 1;

			elIndex += 6;
		}
	}

	// Generate the vertex data and indices for the two end caps
	generateEndCapVertexData(m_length / 2.0f, index, elIndex);
	generateEndCapVertexData(-m_length / 2.0f, index, elIndex);
}

void cylinder::generateEndCapVertexData(float y,int &index, int& elIndex)
{
	// Make a note of the vertex index for the center of the end cap
	int endCapStartIndex = index / 3;

	vertexPositions[index] = 0.0f;
	vertexPositions[index + 1] = y;
	vertexPositions[index + 2] = 0.0f;

	vertexNormals[index] = 0.0f;
	vertexNormals[index + 1] = (y >= 0.0f) ? 1.0f : -1.0;
	vertexNormals[index + 2] = 0.0f;

	index += 3;

	const float dTheta = twoPi / static_cast<float>(m_slices);
	for (int slice = 0; slice < m_slices; ++slice)
	{
		const float theta = static_cast<float>(slice) * dTheta;
		const float cosTheta = cosf(theta);
		const float sinTheta = sinf(theta);

		vertexPositions[index] = m_radius * cosTheta;
		vertexPositions[index + 1] = y;
		vertexPositions[index + 2] = m_radius * sinTheta;

		vertexNormals[index] = 0.0f;
		vertexNormals[index + 1] = (y >= 0.0f) ? 1.0f : -1.0;
		vertexNormals[index + 2] = 0.0f;

		index += 3;
	}

	for (int i = 0; i < m_slices; ++i)
	{
		// Use center point and i+1, and i+2 vertices to create triangles
		if (i != m_slices - 1)
		{
			vertexIndices[elIndex] = endCapStartIndex;
			vertexIndices[elIndex + 1] = endCapStartIndex + i + 1;
			vertexIndices[elIndex + 2] = endCapStartIndex + i + 2;
		}
		else
		{
			vertexIndices[elIndex] = endCapStartIndex;
			vertexIndices[elIndex + 1] = endCapStartIndex + i + 1;
			vertexIndices[elIndex + 2] = endCapStartIndex + 1;
		}

		elIndex += 3;
	}
}

void cylinder::setup()
{
	generateVertexData();

	m_shaderProgram = new ShaderProgram();

		//load shaders
	m_shaderProgram->initFromFiles("Shader/Phong.vert", "Shader/Phong.frag");

		//create vao
		glGenVertexArrays(1, &vaoHandle);
		glBindVertexArray(vaoHandle);

		//add attributes and uniform vars
		m_shaderProgram->addAttribute("coord3d");
		m_shaderProgram->addAttribute("v_normal");

		m_shaderProgram->addUniform("ModelViewMatrix");
		m_shaderProgram->addUniform("NormalMatrix");
		m_shaderProgram->addUniform("model");
		m_shaderProgram->addUniform("view");
		m_shaderProgram->addUniform("projection");

		m_shaderProgram->addUniform("Light.Position");
		m_shaderProgram->addUniform("Light.La");
		m_shaderProgram->addUniform("Light.Ld");
		m_shaderProgram->addUniform("Light.Ls");
		m_shaderProgram->addUniform("Material.Ka");
		m_shaderProgram->addUniform("Material.Kd");
		m_shaderProgram->addUniform("Material.Ks");
		m_shaderProgram->addUniform("Material.Shiness");

		m_shaderProgram->addUniform("location");

		//create vbo for vertices
		glGenBuffers(1, &vbo_cube_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexPositions.size(), vertexPositions.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(
			m_shaderProgram->attribute("coord3d"), // attribute
			3,                 // number of elements per vertex, here (x,y,z,1)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
		);
		glEnableVertexAttribArray(m_shaderProgram->attribute("coord3d"));


		//create vbo for colors
		glGenBuffers(1, &vbo_cube_colors);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexNormals.size(), vertexNormals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(
			m_shaderProgram->attribute("v_normal"), // attribute
			3,                 // number of elements per vertex, here (R,G,B)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
		);
		glEnableVertexAttribArray(m_shaderProgram->attribute("v_normal"));

		glGenBuffers(1, &ibo_cube_elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vertexIndices.size(), vertexIndices.data(), GL_STATIC_DRAW);


		glBindVertexArray(0);

}

void cylinder::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::mat4& offset, int colorNum)
{

	glm::mat4 mview = view * model;
	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));


	glm::vec3 Ka(0.1, 0.1, 0.1);
	glm::vec3 Kd(1, 0, 0);
	glm::vec3 Ks(1, 1, 0);
	GLfloat shiness = 10;

	glm::vec3 La(0.9, 0.9, 0.9);
	glm::vec3 Ld(0.6, 0.6, 0.6);
	glm::vec3 Ls(0.6, 0.6, 0.6);

	glm::vec4 lightPos(0, 0, 300, 1);


	m_shaderProgram->use();


	glUniformMatrix4fv(m_shaderProgram->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mview));
	glUniformMatrix3fv(m_shaderProgram->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));
	glUniformMatrix4fv(m_shaderProgram->uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(m_shaderProgram->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(m_shaderProgram->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform4fv(m_shaderProgram->uniform("Light.Position"), 1, glm::value_ptr(lightPos));
	glUniform3fv(m_shaderProgram->uniform("Light.La"), 1, glm::value_ptr(La));
	glUniform3fv(m_shaderProgram->uniform("Light.Ld"), 1, glm::value_ptr(Ld));
	glUniform3fv(m_shaderProgram->uniform("Light.Ls"), 1, glm::value_ptr(Ls));

	glUniform3fv(m_shaderProgram->uniform("Material.Ka"), 1, glm::value_ptr(Ka));
	glUniform3fv(m_shaderProgram->uniform("Material.Kd"), 1, glm::value_ptr(Kd));
	glUniform3fv(m_shaderProgram->uniform("Material.Ks"), 1, glm::value_ptr(Ks));
	glUniform1fv(m_shaderProgram->uniform("Material.Shiness"), 1, &shiness);

	glUniformMatrix4fv(m_shaderProgram->uniform("location"), 1, GL_FALSE, glm::value_ptr(offset));

	glBindVertexArray(vaoHandle);

	glDrawElements(GL_TRIANGLES,vertexIndices.size(), GL_UNSIGNED_INT, 0);

	m_shaderProgram->disable();

}
