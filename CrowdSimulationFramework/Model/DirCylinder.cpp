#include"DirCylinder.h"

#include <glm/gtx/string_cast.hpp>

DirCylinder::DirCylinder(int agentNum) {
	agentAmount = agentNum;
	setup();
}

DirCylinder::~DirCylinder() {
	delete m_meshModel;
}

void DirCylinder::setup() {

	//m_meshModel = new Model("Model_Obj/DirCylinder/untitled.fbx", true, 0);
	m_meshModel = new Model("Model_Obj/DirCylinder/Untitled.fbx", true, 0);
	m_meshModel->setConstraints(5, 9);

	m_shaderProgram = new ShaderProgram();
	//m_shaderProgram->initFromFiles("Shader/model.vert", "Shader/model.frag");
	m_shaderProgram->initFromFiles("Shader/modelTexture.vert", "Shader/modelTexture.frag");

	m_shaderProgram->addUniform("model");
	m_shaderProgram->addUniform("view");
	m_shaderProgram->addUniform("projection");
	m_shaderProgram->addUniform("NormalMatrix");

	m_shaderProgram->addUniform("LightPosition");
	m_shaderProgram->addUniform("LightIntensity");

	m_shaderProgram->addUniform("Material.Ka");
	m_shaderProgram->addUniform("Material.Kd");
	m_shaderProgram->addUniform("Material.Ks");
	m_shaderProgram->addUniform("Material.Shiness");

	m_shaderProgram->addUniform("location");

	m_shaderProgram->addUniform("numChar");

	m_shaderProgram->addUniform("texture_diffuse1");

	//computeshader = loadComputeShader("Shader/update.comp");

	lightPos = glm::vec3(10, 10, 10);

}

/*#define WORK_GROUP_SIZE 128

void DirCylinder::runComputeShader(int m_width, int m_height, glm::mat4& view, glm::vec3 camPos) {
	glUseProgram(computeshader);
	glUniform1iv(glGetUniformLocation(computeshader, "numChar"), 1, &agentAmount);
	glUniformMatrix4fv(glGetUniformLocation(computeshader, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));

	glm::vec2 res = glm::vec2(m_width, m_height);
	glUniform2fv(glGetUniformLocation(computeshader, "resolution"), 1, glm::value_ptr(res));
	glUniform3fv(glGetUniformLocation(computeshader, "viewPos"), 1, glm::value_ptr(camPos));
	glm::vec3 l = glm::normalize(lightPos - glm::vec3(0, 0, 0));
	glUniform3fv(glGetUniformLocation(computeshader, "lightDir"), 1, glm::value_ptr(l));
	glUniform1i(glGetUniformLocation(computeshader, "startIndex"), 0);



	if (agentAmount < 128) {
		glDispatchCompute(agentAmount, 1, 1);
	}
	else {
		glDispatchCompute((agentAmount / WORK_GROUP_SIZE + 1), 1, 1);
	}

	glUseProgram(0);

	//	glMemoryBarrier(GL_ELEMENT_ARRAY_BARRIER_BIT);
	//defines a barrier ordering memory transactions
	//명령 전에 실행된 메모리 트랜잭션을 자엽ㄱ 이후에 실행된 트랜잭션과 비교하여 정렬하는 장벽을 정의
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, ssboHandle_sum);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboHandle_sum);
	p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	//GLvoid* p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::ivec3), bufMask);
	//memcpy(&test, (glm::ivec3*)p, sizeof(glm::ivec3));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	//	std::cout << glm::to_string(test) << test.x + test.y + test.z << std::endl;
}*/

void DirCylinder::drawMesh(glm::mat4& modelM, glm::mat4& view, glm::mat4& projection, glm::mat4* offset, glm::vec3 camPos, float animationTime, unsigned int idx, unsigned int num, unsigned int shadow) {


	glm::vec3 LightIntensity(1, 1, 1);

	glm::vec3 Ka(0.1, 0.1, 0.1);
	glm::vec3 Kd(1, 1, 1);
	glm::vec3 Ks(1, 1, 0);
	GLfloat	shiness = 10;

	glm::mat4 mview = view * modelM;
	glm::mat4 mvp = projection * view * modelM;
	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));

	m_shaderProgram->use();

	glUniformMatrix4fv(m_shaderProgram->uniform("model"), 1, GL_FALSE, glm::value_ptr(modelM));
	glUniformMatrix4fv(m_shaderProgram->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(m_shaderProgram->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix3fv(m_shaderProgram->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));

	glUniform4fv(m_shaderProgram->uniform("LightPosition"), 1, glm::value_ptr(view * glm::vec4(lightPos, 0)));
	glUniform3fv(m_shaderProgram->uniform("LightIntensity"), 1, glm::value_ptr(LightIntensity));


	glUniform3fv(m_shaderProgram->uniform("Material.Ka"), 1, glm::value_ptr(Ka));
	glUniform3fv(m_shaderProgram->uniform("Material.Kd"), 1, glm::value_ptr(Kd));

	glUniform3fv(m_shaderProgram->uniform("Material.Ks"), 1, glm::value_ptr(Ks));
	glUniform1fv(m_shaderProgram->uniform("Material.Shiness"), 1, &shiness);

	//glUniformMatrix4fv(m_shaderProgram->uniform("location"), 1, GL_FALSE, glm::value_ptr(offset));

	glUniform1ui(m_shaderProgram->uniform("numChar"), idx);

	glUniform1i(m_shaderProgram->uniform("texture_diffuse1"), 0);

	glGenBuffers(1, &ssboHandle_t);  //transformation
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboHandle_t);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * num, offset, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboHandle_t);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	m_meshModel->Draw(1);

	m_shaderProgram->disable();
}


std::string DirCylinder::readFile(const char* fileName)
{
	std::string fileContent;
	std::ifstream fileStream(fileName, std::ios::in);
	if (!fileStream.is_open()) {
		printf("File %s not found\n", fileName);
		return "";
	}
	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		fileContent.append(line + "\n");
	}
	fileStream.close();
	return fileContent;
}

GLuint DirCylinder::loadComputeShader(const char* computeShaderFile)
{
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

	// Read shaders
	std::string computeShaderStr = readFile(computeShaderFile);
	const char* computeShaderSrc = computeShaderStr.c_str();

	GLint result = GL_FALSE;

	std::cout << "Compiling compute shader." << std::endl;
	glShaderSource(computeShader, 1, &computeShaderSrc, NULL);
	glCompileShader(computeShader);

	printShaderLog(computeShader);

	std::cout << "Linking program" << std::endl;
	GLuint program = glCreateProgram();
	glAttachShader(program, computeShader);
	glLinkProgram(program);
	printProgramLog(program);

	glDeleteShader(computeShader);

	return program;

}

void DirCylinder::printShaderLog(GLuint shader)
{
	GLint result = GL_FALSE;
	int logLength;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar* strInfoLog = new GLchar[logLength + 1];
		glGetShaderInfoLog(shader, logLength, NULL, strInfoLog);
		printf("shaderlog: %s\n", strInfoLog);
	};
}

void DirCylinder::printProgramLog(GLuint program)
{
	GLint result = GL_FALSE;
	int logLength;

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar* strInfoLog = new GLchar[logLength + 1];
		glGetProgramInfoLog(program, logLength, NULL, strInfoLog);
		printf("programlog: %s\n", strInfoLog);
	};
}
