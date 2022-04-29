#ifndef FLAGS_H
#define FLAGS_H

#include <atomic>

enum class ViewportType { Auto, Vulkan, OpenGL, Metal, None };

enum class RenderMode { Batch, Iterative };

struct Flags {
	bool cancelRender = false;
	std::atomic<unsigned int> threadsActive = 0;
	bool savePPM = true;
	bool allSavesComplete = false;
	bool showViewport = true;
	int maxBounces = 50;
	bool renderComplete = false;
	ViewportType viewportType = ViewportType::OpenGL;
	RenderMode renderMode = RenderMode::Batch;
};

#endif // !FLAGS_H
