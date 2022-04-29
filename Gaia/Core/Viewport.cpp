#include "Viewport.h"

std::unique_ptr<Viewport> CreateViewport(std::shared_ptr<Flags> f, std::shared_ptr<Image> i) {
	
	VulkanViewport testViewport;
	
	switch (f->viewportType)
	{
	case ViewportType::None:
		return nullptr;
		break;
	case ViewportType::Auto:
	case ViewportType::Metal:
		// Force the use of the Metal viewport - assuming the compatibbility check succeeds.
#if __APPLE__
// We're using macOS - check for metal compatibility
		if (MetalSupportCheck()) {
			// Metal is supported

			// TODO: FIX METAL VIEWPORT TO DEAL WITH RGB OUTPUT ARRAY - STILL EXPECTS RGBA
			std::cout << "Metal viewport created\n";
			return std::make_unique<MetalViewport>(f, i);
		}
#endif
		if(f->viewportType == ViewportType::Metal) 
			std::cout << "Metal viewport not supported\n";

	case ViewportType::Vulkan:
		// Check for Vulkan compatibility

		if (testViewport.VulkanSupportCheck()) {
			std::cout << "Vulkan viewport created\n";
			return std::make_unique<VulkanViewport>(f, i);
		}
		else {
			if (f->viewportType == ViewportType::Vulkan)
				std::cout << "Vulkan viewport not supported\n";
		}

	case ViewportType::OpenGL:
	default:
		// OpenGL is the default
		std::cout << "OpenGL viewport created\n";
		return std::make_unique<OpenGLViewport>(f, i);
		break;
	}

	return nullptr;
}