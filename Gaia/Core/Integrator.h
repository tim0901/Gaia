#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "Flags.h"
#include "Tiler.h"
#include "Scene.h"
#include "../Maths/Colour.h"
#include "../Objects/Object.h"
#include "Image.h"
#include "../Maths/Pdf.h"
#include "Light.h"

class Integrator
{
public:
	Integrator(){}

	virtual void Render(std::shared_ptr<Image> image, std::shared_ptr<Flags> flags)  = 0;

	virtual Vec3d DirectLightingAt(const Vec3d& p, const Ray& r, IntersectionRecord& iRec) const = 0;

protected:
	std::unique_ptr<Tiler> tiler;
	std::shared_ptr<Scene> scene;
};

class PathIntegrator : public Integrator {
public:
	PathIntegrator(std::shared_ptr<Scene> s, std::unique_ptr<Tiler> t) {

		this->tiler = std::move(t);
		this->scene= s;
	}

	void Render(std::shared_ptr<Image> image, std::shared_ptr<Flags> flags) ; 
	Vec3d DirectLightingAt(const Vec3d& p, const Ray& r, IntersectionRecord& iRec) const;

};
#endif // !INTEGRATOR_H
