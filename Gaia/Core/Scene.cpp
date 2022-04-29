#include "Scene.h"

// Return the transmittance between two points - for now this is a binary value based 
// solely on the geometry of the scene
Vec3d Scene::TransmittanceBetween(const Vec3d& p1, const Vec3d& p2) const {
	Ray ray = FromAToB(p1, p2);
	IntersectionRecord iRec;

	if (objectList.Intersect(ray, iRec, 0.0001, 1.0 - 0.0001) && length(iRec.p - p2) > 0.001) {



		

		// Some rays intersect within a tiny distance of the light - essentially a rounding error
		// TODO: implement proper fix - the length(iRec.p - p2) > 0.001 condition is a hack
		// that checks how far the sampled point is from the light source
		/*
		if (iRec.mat->ReturnName() == "green") {
			std::cout << "origin: " << ray.origin << " direction: " << ray.direction << std::endl;
			std::cout << "origin + direction = p2: " << ray.origin + ray.direction << std::endl;
			std::cout << "p1: " << p1 << " p2: " << p2 << std::endl;

			std::cout << iRec.t << std::endl;
			std::cout << "Hit location: " << iRec.p << std::endl;
			std::cout << (iRec.p - p2) << " length: " << length(iRec.p - p2) << std::endl;
			std::cout << iRec.mat->ReturnName() << std::endl;
		}*/
		return Vec3d(0.0, 0.0, 0.0);
	}
	else {
		//std::cout << "Missed scene: " << iRec.p << " " << iRec.t << std::endl;
		return Vec3d(1.0, 1.0, 1.0);
	}
}