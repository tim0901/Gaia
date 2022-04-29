#pragma once

#ifndef OPENGLVIEWPORT_H
#define OPENGLVIEWPORT_H

#include "../Core/Viewport.h"

#ifndef __APPLE__

#include <Windows.h>
#include <gl\wglext.h>

#else

#include <glad.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#ifndef GLFWLOADED
#define GLFWLOADED

#include <GLFW/glfw3.h>

#endif // GLFWLOADED

#pragma clang diagnostic pop

//#include <wglext.h>
//#include "glad.c"

#endif

// Hander class for the OpenGL viewport window

class OpenGLViewport: public Viewport {
public:
	OpenGLViewport(std::shared_ptr<Flags> f, std::shared_ptr<Image> i) : flags(f), image(i) { InitialiseViewport(); }
	~OpenGLViewport() {

		// Delete resources now they've outlived their purpose
		glDeleteTextures(1, &texture);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		glfwDestroyWindow(window);

		// glfw: terminate, clears all previously allocated GLFW resources.
		glfwTerminate();
	}

	void InitialiseViewport();

	// Call to update viewport contents
	void UpdateView(int argc, const char* argv[]);

private:

	// Process IO events
	void ProcessInput() {
		//Kills window on ESC
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
			std::cout << "ESCAPE" << std::endl;
			flags->cancelRender = true;
		}
	}

	std::shared_ptr<Flags> flags;
	std::shared_ptr<Image> image;
	//Stores vertices in GPU memory
	//
	unsigned int VBO = 0, VAO = 0, EBO = 0;
	unsigned int texture = 0;
	GLFWwindow* window = nullptr;

	bool initialised = false;
};

#endif //OPENGLVIEWPORT_H