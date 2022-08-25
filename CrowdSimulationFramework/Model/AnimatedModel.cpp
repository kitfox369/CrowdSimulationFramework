#include"AnimatedModel.h"
#include <glm/gtx/string_cast.hpp>

AnimatedModel::AnimatedModel(int agentNum) {
	agentAmount = agentNum;
	setup();
}

AnimatedModel::~AnimatedModel() {
	delete m_meshModel;
}

void AnimatedModel::setup() {

	//m_meshModel = new Model("Model_Obj/DirCylinder/untitled.fbx", true, 0);
	m_meshModel = new Model("Model_Obj/Animated/fast.fbx", true, 0);
	m_meshModel->setConstraints(5, 9);

	m_shaderProgram = new ShaderProgram();
	m_shaderProgram->initFromFiles("Shader/model.vert", "Shader/model.frag");

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

	//m_shaderProgram->addUniform("numChar");

	//m_shaderProgram->addUniform("hasTextureDiffuse");
	//m_shaderProgram->addUniform("hasTextureSpecular");

	/*for (unsigned int i = 0; i < m_meshModel->m_NumBones; ++i)
	{
		std::string name = "gBones[" + std::to_string(i) + "]";
		m_shaderProgram->addUniform(name.c_str());
	}
	for (unsigned int i = 0; i < m_meshModel->m_NumBones; ++i)
	{
		std::string name = "dqs[" + std::to_string(i) + "]";
		m_shaderProgram->addUniform(name.c_str());
	}*/


	modelMatrices = new glm::mat4[agentAmount];
	degreeArray = new float[agentAmount];
	int gap = 360 / agentAmount;
	for (int i = 0; i < 360; i+= gap) {
		degreeArray[i] = i;
	}


	srand(clock()); // initialize random seed	




	rr = new float[agentAmount];


	float min_d = 9999.9;
	float max_d = -9999.9;


	//for (unsigned int i = 0; i < agentAmount; i++)
	//{
	//	glm::mat4 model = glm::mat4(1.0f);
	//	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//	// 1. translation: displace along circle with 'radius' in range [-offset, offset]
	//	float angle = (float)i / (float)agentAmount * 360.0f;
	//	//float displacement = (rand() % (int)(2 * offsetM * offset_2)) / offset_2 - offsetM;
	//	float displacement = (rand() % (int)(2 * offsetM * 100)) / 100.0f - offsetM;
	//	float x = sin(angle) * radius + displacement;
	//	//displacement = (rand() % (int)(2 * offsetM * offset_2)) / offset_2 - offsetM;
	//	displacement = (rand() % (int)(2 * offsetM * 100)) / 100.0f - offsetM;
	//	float y = 0; // keep height of field smaller compared to width of x and z
	//	//displacement = (rand() % (int)(2 * offsetM * offset_2)) / offset_2 - offsetM;
	//	displacement = (rand() % (int)(2 * offsetM * 100)) / 100.0f - offsetM;
	//	float z = cos(angle) * radius + displacement;
	//	model = glm::translate(model, glm::vec3(x, y, z));
	//	float scale = .1f;
	//	model = glm::scale(model, glm::vec3(scale));


	//	glm::vec3 c1(x, 0, z);
	//	c1 = glm::normalize(c1);
	//	glm::vec3 dir = glm::cross(glm::vec3(0, 1, 0), c1);
	//	float degree = glm::degrees(atan2(dir.x, dir.z));

	//	rr[i] = -degree;

	//	model = glm::rotate(model, glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));

	//	modelMatrices[i] = model;

	//}
	for (unsigned int i = 0; i < agentAmount; i++) {
		cout << "aa";
		int x, y;
		//while (1) {
		//	x = rand() % 200 - 100;
		//	y = rand() % 200 - 100;

		//	float ddd = sqrt(x * x + y * y);
		//	if (ddd > 30 && ddd < 50) break;
		//}
		x = sin(degreeArray[i]) * radius;
		y = cos(degreeArray[i]) * radius;


		glm::vec3 c1(x, 0, y);
		c1 = glm::normalize(c1);
		glm::vec3 dir = glm::cross(c1, glm::vec3(0, 1, 0));
		float degree = glm::degrees(atan2(dir.z, dir.x));

		rr[i] = degree;


		glm::mat4 model = glm::mat4(1.0f);

		model = glm::scale(model, glm::vec3(0.1f));

		model = glm::rotate(model, glm::radians(rr[i]), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));


		model = glm::translate(model, glm::vec3(x, 0, y));

		//glm::mat4 rot = glm::rotate(model, glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));

		modelMatrices[i] = model;

		cout << glm::to_string(modelMatrices[i]) << endl;

		degreeArray[i] += 1;
		if (degreeArray[i] > 360)
			degreeArray[i] -= 360;
	}
	
	computeshader = loadComputeShader("Shader/update.comp");

	lightPos = glm::vec3(10, 10, 10);

}

#define WORK_GROUP_SIZE 128

void AnimatedModel::runComputeShader(int m_width,int m_height, glm::mat4& view,glm::vec3 camPos) {
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
}

void AnimatedModel::drawMesh(glm::mat4& modelM, glm::mat4& view, glm::mat4& projection,glm::mat4& offset,glm::vec3 camPos,float animationTime, unsigned int st, unsigned int num, unsigned int shadow) {
	

	glm::vec3 LightIntensity(1, 1, 1);

	glm::vec3 Ka(0.1, 0.1, 0.1);
	glm::vec3 Kd(1, 1,1);
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

	glUniform4fv(m_shaderProgram->uniform("LightPosition"), 1, glm::value_ptr(view * glm::vec4(lightPos,0)));
	glUniform3fv(m_shaderProgram->uniform("LightIntensity"), 1, glm::value_ptr(LightIntensity));
	

	glUniform3fv(m_shaderProgram->uniform("Material.Ka"), 1, glm::value_ptr(Ka));
	glUniform3fv(m_shaderProgram->uniform("Material.Kd"), 1, glm::value_ptr(Kd));

	glUniform3fv(m_shaderProgram->uniform("Material.Ks"), 1, glm::value_ptr(Ks));
	glUniform1fv(m_shaderProgram->uniform("Material.Shiness"), 1, &shiness);

	glUniformMatrix4fv(m_shaderProgram->uniform("location"), 1, GL_FALSE, glm::value_ptr(offset));

	//glUniform1i(m_shaderProgram->uniform("hasTextureDiffuse"), 1);
	//glUniform1i(m_shaderProgram->uniform("hasTextureSpecular"), 1);

	unsigned int numChar = test.x + test.y;

	//glUniform1uiv(m_shaderProgram->uniform("numChar"), 1, &numChar);

	//ssbo
	glGenBuffers(1, &ssboHandle_t);  //transformation
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboHandle_t);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * agentAmount, modelMatrices, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboHandle_t);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	m_meshModel->Draw(1);

	m_shaderProgram->disable();
}


std::string readFile(const char* fileName)
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

GLuint AnimatedModel:: loadComputeShader(const char* computeShaderFile)
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

void AnimatedModel::printShaderLog(GLuint shader)
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

void AnimatedModel::printProgramLog(GLuint program)
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
