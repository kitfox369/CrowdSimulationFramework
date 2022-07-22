
#ifndef _ANIMATED_MODEL_H_
#define _ANIMATED_MODEL_H_

#include"../Component.h"
#include"../Model.h"

class AnimatedModel {
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

public:

	AnimatedModel();
	~AnimatedModel();

	void drawMesh(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::vec3 camPos, float animationTime, unsigned int st, unsigned int num, unsigned int shadow);
};

#endif