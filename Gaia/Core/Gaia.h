#ifndef GAIA_H
#define GAIA_H

#include <memory>
#include "Integrator.h"
#include "Image.h"

#include <string>
#include <fstream>

class Gaia
{
public:
	Gaia(std::shared_ptr<Image> img, std::shared_ptr<Scene> s, std::shared_ptr<Flags> f) : image(img), flags(f), scene(s) {

		if (!flags->savePPM) {
			std::cout << "No save modes selected" << std::endl;
			flags->allSavesComplete = true;
		}

		// Build light sampler
		s->BuildLightSampler();

		// Encompass scene in BVH
		s->GenerateSceneBVH();

		std::unique_ptr<Tiler> t;

		switch (flags->renderMode) {
		case RenderMode::Iterative:
			t = std::make_unique<IterativeTiler>(image->xDim, image->yDim, 16, image->nSamples);
			break;
		case RenderMode::Batch:
		default:
			t = std::make_unique<BatchTiler>(image->xDim, image->yDim, 16, image->nSamples);
		}

		integrator = std::make_unique<PathIntegrator>(scene, std::move(t));
	
	}
	~Gaia() {}

	void Render() {

		flags->threadsActive++;

		// Start timer
		auto start = std::chrono::steady_clock::now();
		
		// Start render
		integrator->Render(image, flags);

		saveMutex.lock();

		// If this is the final active thread
		if (flags->threadsActive == 1) {
			// Stop the timer
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> diff = end - start;
			std::cout << "Render time: " << std::chrono::duration <double, std::milli>(diff).count() << "ms" << std::endl;

			// If the render has not been cancelled, start the save process
			if (!flags->allSavesComplete && !flags->cancelRender)
				Save();

		}

		saveMutex.unlock();
		flags->threadsActive--;

		if (flags->threadsActive == 0) {
			flags->renderComplete = true;
		}
	}

	void Save() {

		if (flags->savePPM) {
			std::cout << "Saving " << scene->fileName << ".ppm... ";
			std::ofstream ofs;

			ofs.open("Renders/" + scene->fileName + ".ppm");

			//PPM file header
			ofs << "P3\n" << image->xDim << " " << image->yDim << "\n255\n";

			//Outputs colours as required by file format
			for (int j = image->yDim - 1; j >= 0; j--) {
				for (int i = 0; i < image->xDim; i++) {

					int pixelLocation = 3 * ((image->xDim * j) + i);

					double ir = std::clamp(255.0 * SRGBGammaCorrect(image->data[pixelLocation + 0]) + 0.5, 0.0, 255.0);
					double ig = std::clamp(255.0 * SRGBGammaCorrect(image->data[pixelLocation + 1]) + 0.5, 0.0, 255.0);
					double ib = std::clamp(255.0 * SRGBGammaCorrect(image->data[pixelLocation + 2]) + 0.5, 0.0, 255.0);

					ofs << ir << " " << ig << " " << ib << "\n";
				}
			}

			//Close file
			ofs.close();
			std::cout << "Complete" << std::endl;
		}

		flags->allSavesComplete = true;
	}

	std::shared_ptr<Flags> flags;
	std::shared_ptr<Image> image;
	std::unique_ptr<Integrator> integrator;
	std::shared_ptr<Scene> scene;

	std::mutex saveMutex;
};

#endif // !GAIA_H
