#include"AnimatedModel.h"


AnimatedModel::AnimatedModel() {
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

	m_shaderProgram->addUniform("mvp");
	m_shaderProgram->addUniform("modelView");
	m_shaderProgram->addUniform("NormalMatrix");

	m_shaderProgram->addUniform("LightPosition");
	m_shaderProgram->addUniform("LightIntensity");

	m_shaderProgram->addUniform("Material.Ka");
	m_shaderProgram->addUniform("Material.Kd");
	m_shaderProgram->addUniform("Material.Ks");
	m_shaderProgram->addUniform("Material.Shiness");

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

}

void AnimatedModel::drawMesh(glm::mat4& modelM, glm::mat4& view, glm::mat4& projection,glm::vec3 camPos,float animationTime, unsigned int st, unsigned int num, unsigned int shadow) {
	
	glm::vec4 lightPos(10, 10, 10, 0);
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

	glUniformMatrix4fv(m_shaderProgram->uniform("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(m_shaderProgram->uniform("modelView"), 1, GL_FALSE, glm::value_ptr(mview));
	glUniformMatrix3fv(m_shaderProgram->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));

	glUniform4fv(m_shaderProgram->uniform("LightPosition"), 1, glm::value_ptr(view * lightPos));
	glUniform3fv(m_shaderProgram->uniform("LightIntensity"), 1, glm::value_ptr(LightIntensity));
	

	glUniform3fv(m_shaderProgram->uniform("Material.Ka"), 1, glm::value_ptr(Ka));
	glUniform3fv(m_shaderProgram->uniform("Material.Kd"), 1, glm::value_ptr(Kd));

	glUniform3fv(m_shaderProgram->uniform("Material.Ks"), 1, glm::value_ptr(Ks));
	glUniform1fv(m_shaderProgram->uniform("Material.Shiness"), 1, &shiness);

	//glUniform1i(m_shaderProgram->uniform("hasTextureDiffuse"), 1);
	//glUniform1i(m_shaderProgram->uniform("hasTextureSpecular"), 1);


	m_meshModel->Draw(1);

	m_shaderProgram->disable();
}