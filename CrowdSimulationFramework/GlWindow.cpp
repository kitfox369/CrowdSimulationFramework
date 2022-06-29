#include "GlWindow.h"

	GlWindow::GlWindow(int w, int h)
	{
		m_width = w;
		m_height = h;
		m_ratio = m_width / static_cast<float>(m_height);

		setupBuffer();
	}

	/// <summary>
	/// ¼Ò¸êÀÚ
	/// </summary>
	GlWindow::~GlWindow() {

		delete m_grid;
	}

	void GlWindow::setupBuffer()
	{
		m_grid = new grid();
	}

	void GlWindow::setSize(int width, int height) {
		m_width = width;
		m_height = height;
	}

	void GlWindow::setAspect(float ratio) {
		m_ratio = ratio;
	}

	void GlWindow::draw(camera cam, float zoom)
	{
		glClearColor(0.5f, 0.5f, 0.5f, 0);		//background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//clear up color and depth buffer

		glEnable(GL_CLIP_DISTANCE0);

		//glEnable(GL_DEPTH_TEST);			//test whether an object is in front of other object?
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(200, 0, m_width, m_height);

		glm::mat4 viewMat = cam.GetViewMatrix();

		//glm::mat4 projection = glm::ortho(-(m_width / rate), m_width / rate , -(m_height / rate), m_height / rate , -1.0f, 1000.0f);
		glm::mat4 projection = glm::perspective(glm::radians(zoom), m_ratio, 0.1f, 1000.0f);


		m_model.glPushMatrix();
		glm::mat4 model = m_model.getMatrix();

		//rect->draw(model, viewMat, projection);

		m_grid->draw(model, viewMat, projection);

	}

