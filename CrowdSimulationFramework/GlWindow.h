#ifndef _GLWINDOW_
#define _GLWINDOW_

#include <iostream>

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_inverse.hpp"

#include"Model/Grid.h"
#include"Model/Cylinder.h"
#include"Model/AnimatedModel.h"
#include"Model/DirCylinder.h"
#include"Camera.h"
#include"ModelView.h"
#include"SiAgent.h"
#include"SystemInfo.h"

class GlWindow {
public:
	GlWindow(int w, int h);
	~GlWindow();
	
	ShaderProgram* m_shaderProgram;

	void setSIAgentSetting();

	void setSize(int width, int height);
	void setAspect(float ratio);
	void draw(camera cam, float zoom, float _deltaTime,float animationTime);

	GLuint vaoHandle;			//
	GLuint vbo_vetexPositon, vbo_colors;


	grid* getSelectedObj();
	void update();
private:
	ModelView m_model;
	int m_width;
	int m_height;
	float m_ratio;

	grid* m_grid;
	cylinder* m_cylinder;
	DirCylinder* m_dircylinder;

	AnimatedModel* m_animatedModel;

	std::vector<SiAgent*> agent;
	int agentNum;
	int agentModelKindNum;

	void setupBuffer();
	void drawModel(glm::mat4& modelM, glm::mat4& view, glm::mat4& projection, glm::mat4& offset, glm::vec3 camPos, float animationTime, unsigned int st, unsigned int num, unsigned int shadow);
};


#endif // !_GLWINDOW_