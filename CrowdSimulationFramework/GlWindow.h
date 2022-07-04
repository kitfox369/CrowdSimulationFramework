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
#include"Camera.h"
#include"ModelView.h"
#include"SiAgent.h"

class GlWindow {
public:
	GlWindow(int w, int h);
	~GlWindow();
	
	ShaderProgram* m_shaderProgram;

	void setSize(int width, int height);
	void setAspect(float ratio);
	void draw(camera cam, float zoom, float _deltaTime);

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


	std::vector<SiAgent*> agent;
	int agentNum;


	void setupBuffer();

};


#endif // !_GLWINDOW_