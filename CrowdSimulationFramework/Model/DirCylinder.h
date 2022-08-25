
#ifndef _DIRCYLINDER_H_
#define _DIRCYLINDER_H_

#include"../Component.h"
#include"../Model.h"

class DirCylinder {
	Model* m_meshModel;
	void setup();

	vector<glm::mat4> Transforms;
	vector<glm::fdualquat> dualQuaternions;
	vector<glm::mat2x4> DQs;

	vector<glm::mat4> Transforms2;
	vector<glm::mat4> Transforms3;

	vector<glm::fdualquat> dualQuaternions2;
	vector<glm::mat2x4> DQs2;

	GLuint vaoHandle;
	GLuint ssboHandle_t = 0;

	glm::mat4* modelMatrices;
	float* degreeArray;
	float* rr;
	float radius = 1000.0;
	int agentAmount = 0;

	GLuint computeshader;
	GLuint ssboHandle_sum = 0;
	glm::vec3 lightPos;
	glm::ivec3 test;
	GLvoid* p;

	glm::mat4 interpolate(glm::mat4& _mat1, glm::mat4& _mat2, float r)
	{
		glm::quat rot0 = glm::quat_cast(_mat1);
		glm::quat rot1 = glm::quat_cast(_mat2);

		glm::quat finalRot = glm::slerp(rot0, rot1, r);


		glm::mat4 finalMat = glm::mat4_cast(finalRot);

		finalMat[3] = _mat1[3] * (1 - r) + _mat2[3] * r;

		return finalMat;
	}

	ShaderProgram* m_shaderProgram;

	void printShaderLog(GLuint shader);
	void printProgramLog(GLuint program);

public:

	DirCylinder(int agentNum);
	~DirCylinder();

	std::string readFile(const char* fileName);
	GLuint loadComputeShader(const char* computeShaderFile);
	void runComputeShader(int m_width, int m_height, glm::mat4& view, glm::vec3 camPos);
	void drawMesh(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::mat4* offset, glm::vec3 camPos, float animationTime, unsigned int idx, unsigned int num, unsigned int shadow);
};

#endif