#include "Integrator.h"

void PathIntegrator::Render(std::shared_ptr<Image> image, std::shared_ptr<Flags> flags) {

	Tile tile;

	// Fetch new tile
	while (tiler->FetchTile(&tile)) {

		// For each pixel
		for (int y = tile.yEnd - 1; (int)tile.yStart <= y; y--) {
			for (int x = tile.xStart; x < (int)tile.xEnd; x++) {

				// Find the position of this pixel in the image array
				size_t pixelLocation = (y * image->xDim + x);

				// For storing colour information
				Vec3d pixel(0.0, 0.0, 0.0);

				if (image->samplesArray.size() > 0) {
					// We're using iterative sampling - Load any existing pixel information from image array
					pixel.r() = image->data[pixelLocation * 3 + 0] * image->samplesArray[pixelLocation];
					pixel.g() = image->data[pixelLocation * 3 + 1] * image->samplesArray[pixelLocation];
					pixel.b() = image->data[pixelLocation * 3 + 2] * image->samplesArray[pixelLocation];
				}

				for (size_t s = 0; s < tile.samples; s++) {

					// 0. Generate a ray from the camera
					double u = double(x + rng.RandomDouble()) / double(image->xDim);
					double v = double(y + rng.RandomDouble()) / double(image->yDim);
					Ray ray = scene->camera.FetchRay(u, v);

					bool specularBounce = false;
					Vec3d beta(1.0, 1.0, 1.0);


					// Max 50 bounces
					for (int i = 0; i < flags->maxBounces; i++) {

						// 1. Sample participating media. Use to determine tmax for ray
						// TODO: participating media

						// 2. Intersect ray with scene - see if objects are closer than media
						IntersectionRecord iRec;
						bool hit = scene->Intersect(ray, iRec);

						// Possibly add emitted light
						if (i == 0 || specularBounce) {
							if (hit) {
								if (iRec.light) {
									pixel += beta * iRec.light->Le(ray, iRec);
								}
							}
							else {
								// Infinite area lights
							}
						}

						// Ray went to Narnia
						if (!hit || i == (flags->maxBounces - 1)) {
							beta *= Vec3d(0.0, 0.0, 0.0);
							break;
						}

						// Sample illumination from lights - but not for pure specular materials!
						if (!iRec.mat->pureSpecular) {
							Vec3d dirlight = DirectLightingAt(iRec.p, ray, iRec);

							pixel += beta * dirlight;
						}

						// Reverse ray direction in ray for scattering equations
						ray.direction = -ray.direction;

						// 3. Evaluate interaction with scene/media
						ScatteringRecord sRec;
						Vec3d f = iRec.mat->SampleAndEvaluate(ray, iRec, sRec);
						specularBounce = sRec.specular;

						// Fetch new ray direction
						ray = sRec.scatteredRay;

						// Find the angle between the new ray and the surface normal at the current intersection
						double cosTheta = dot(ray.direction, iRec.normal);
						double pdf = sRec.scatteringPDF;

						// End the ray if this bounce would result in a black pixel
						if (f == Vec3d(0.0, 0.0, 0.0) || pdf == 0.0)
							break;

						// Rendering equation
						beta *= (f * std::abs(cosTheta) / pdf); // L = f * cos(theta) / p

						// 4. Russian Roulette elimination
					}
					if (flags->cancelRender) {
						return;
					}

				}

				float invDivisor = 0.0f;
				if (image->samplesArray.size() > 0) {
					// Store the number of samples that have been perfprmed
					image->samplesArray[pixelLocation] += tile.samples;
					invDivisor = 1.0f / image->samplesArray[pixelLocation];
				}
				else {
					invDivisor = 1.0f / tile.samples;
				}

				// Store pixel data in image
				image->data[pixelLocation * 3 + 0] = pixel.r() * invDivisor;
				image->data[pixelLocation * 3 + 1] = pixel.g() * invDivisor;
				image->data[pixelLocation * 3 + 2] = pixel.b() * invDivisor;

				if (image->data[pixelLocation * 3 + 0] > 1.0)
				//	std::cout << image->data[pixelLocation * 3] << " " << image->data[pixelLocation * 3 + 1] << " " << image->data[pixelLocation * 3 + 2] << "\n";

				if (flags->cancelRender) {
					return;
				}
					
			}
		}
	}
	return;
}

Vec3d PathIntegrator::DirectLightingAt(const Vec3d& p, const Ray& r, IntersectionRecord& iRec) const {

	double lightPdf = 0, scatteringPdf = 0;
	Vec3d sampledDirection(0.0, 0.0, 0.0);
	bool visibility = true;
	Vec3d directLuminosity(0.0, 0.0, 0.0);

	// Sample light source

	Light* sampledLight = scene->ChooseLight();

	Vec3d incidentLuminosity = sampledLight->Sample(p, sampledDirection, lightPdf, visibility);

	bool hitscene = false;

	Vec3d sampledPosition = sampledDirection + p;

	// We reverse the sampled direction here as we are calculating the contribution arriving from the sampled light source
	// Prior to this, sampledDirection is a vector from p to the light.
	sampledDirection = -unit_vector(sampledDirection);

	if (lightPdf > 0 && (incidentLuminosity != Vec3d(0.0, 0.0, 0.0))) {
		// Calculate BSDF for point
		ScatteringRecord sRec;
		Vec3d f = iRec.mat->ScatteredRadiance(sampledDirection, r.direction, iRec, sRec) * std::abs(dot(sampledDirection, iRec.normal));;
		scatteringPdf = iRec.mat->EvaluatePDFFromDirections(sampledDirection, r.direction, iRec.normal);

		Vec3d transmittance = Vec3d(0.0, 0.0, 0.0);
		transmittance += scene->TransmittanceBetween(p, sampledPosition);

		if (!visibility || (transmittance == Vec3d(0.0, 0.0, 0.0))) {
			hitscene = true;
			incidentLuminosity = Vec3d(0.0, 0.0, 0.0);
		}

		if (incidentLuminosity != Vec3d(0.0, 0.0, 0.0)) {
			// Add contribution
			double weighting = PowerHeuristic(1, lightPdf, 1, scatteringPdf);
			directLuminosity += f * incidentLuminosity * weighting / lightPdf;
		}
	}

	// Sample BSDF
	ScatteringRecord sRec;

	Ray reverseRay(r.origin, -r.direction);
	
	//iRec.mat->SampleDirection(reverseRay, iRec, sRec);
	//Vec3d f = iRec.mat->ScatteredRadiance(reverseRay.direction, sRec.scatteredRay.direction, iRec, sRec);
	
	Vec3d f = iRec.mat->SampleAndEvaluate(reverseRay, iRec, sRec);
	
	f *= abs(dot(sRec.scatteredRay.direction, iRec.normal));

	if (f != Vec3d(0.0, 0.0, 0.0) && sRec.scatteringPDF > 0.0) {

		double weight = 1.0;
		if (!sRec.specular) {
			lightPdf = sampledLight->EvaluatePdfInDirection(p, sRec.scatteredRay.direction);
			if (lightPdf == 0)
				return directLuminosity;
			weight = PowerHeuristic(1, sRec.scatteringPDF, 1, lightPdf);
		}

		IntersectionRecord lightIntersection;
		bool hitLight = scene->Intersect(sRec.scatteredRay, lightIntersection);
		incidentLuminosity = Vec3d(0.0);
		if (hitLight) {
			if (lightIntersection.light == sampledLight) {
				sRec.scatteredRay.direction = -sRec.scatteredRay.direction;
				incidentLuminosity = sampledLight->Le(sRec.scatteredRay, lightIntersection);
			}
		}
		else {
			// Deal with infinite area lights
		}
		if (incidentLuminosity != Vec3d(0.0, 0.0, 0.0)) {
			directLuminosity += f * incidentLuminosity * weight / sRec.scatteringPDF;
		}
	}

	//if (directLuminosity.x() > 0.9) {
	//	std::cout << "2: " << directLuminosity << std::endl;
	//	std::cout << f << " " << incidentLuminosity << std::endl;
	//}
	return directLuminosity;
}