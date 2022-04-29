#ifndef METALVIEWPORT_H
#define METALVIEWPORT_H

#include "../Core/Viewport.h"

// C++ - Objective-C interface file

class MetalViewport :public Viewport {
public:
	MetalViewport() {}
	MetalViewport(std::shared_ptr<Flags> f, std::shared_ptr<Image> i) {};

	void InitialiseViewport() {
		// Initialisation of the Metal viewport is performed when UpdateViewport is first called
	}

	void UpdateViewport(int argc, const char* argv[]);
};

bool MetalSupportCheck();

#endif // !METALVIEWPORT_H
