#ifndef _GLWINDOW_
#define _GLWINDOW_

#include <iostream>

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_inverse.hpp"

#include"Model/Grid.h"
#include"Camera.h"
#include"ModelView.h"

class GlWindow {
public:
	GlWindow(int w, int h);
	~GlWindow();
	
	ShaderProgram* m_shaderProgram;

	void setSize(int width, int height);
	void setAspect(float ratio);
	void draw(camera cam, float zoom);

	GLuint vaoHandle;			//
	GLuint vbo_vetexPositon, vbo_colors;


private:
	ModelView m_model;

	int m_width;
	int m_height;
	float m_ratio;

	grid* m_grid;

	void setupBuffer();
};


#endif // !_GLWINDOW_