#ifndef VIEWPORT_H
#define VIEWPORT_H


#include <memory>
#include <iostream>

#include "Flags.h"
#include "Image.h"

class Viewport {
public:
	Viewport() {}
	Viewport(std::shared_ptr<Flags> f, std::shared_ptr<Image> i){}

	virtual void InitialiseViewport() = 0;

	virtual void UpdateView(int argc, const char* argv[]) = 0;
};


#include "../Metal/MetalViewport.h"


#include <glad/glad.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../Vulkan/VulkanViewport.h"
#include "../OpenGL/OpenGLViewport.h"

std::unique_ptr<Viewport> CreateViewport(std::shared_ptr<Flags> f, std::shared_ptr<Image> i);

#endif // !VIEWPORT_H
