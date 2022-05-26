// rt.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>

#include "Core/Flags.h"
#include "Core/Gaia.h"
#include "Core/Scenes.h"
#include "Core/Viewport.h"


int main(int argc, const char* argv[])
{
	// Start setup timer
	auto setupStart = std::chrono::steady_clock::now();

	std::shared_ptr<Flags> flags = std::make_shared<Flags>();
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	ShaderBall(scene, flags);
	std::shared_ptr<Image> image = std::make_shared<Image>(scene->xDim, scene->yDim, scene->samples, scene->fileName, flags->renderMode == RenderMode::Iterative ? true : false );

	Gaia g(image, scene, flags);

	// Open viewport window
	std::unique_ptr<Viewport> viewport = nullptr;

	flags->showViewport = true;

	if (flags->showViewport) {
		viewport = CreateViewport(flags, image);
	}

	// End setup timer
	auto setupEnd = std::chrono::steady_clock::now();
	std::chrono::duration<double> diff = setupEnd- setupStart;
	std::cout << "Setup time: " << std::chrono::duration <double, std::milli>(diff).count() << "ms" << std::endl;

	// Spawn render threads
	const unsigned int nThreads = std::thread::hardware_concurrency();
	for (unsigned int i = 0; i < nThreads; i++)
		std::thread(&Gaia::Render, &g).detach();

	if (viewport) {
		viewport->UpdateView(argc, argv);
	}

	// Ensure that the program does not terminate while render/save threads are running
	while (!flags->renderComplete) {
		// Wait
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
