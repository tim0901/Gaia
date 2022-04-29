#ifndef CAMERA_H
#define CAMERA_H

#include "../Maths/Random.h"
#include "../Maths/Pi.h"
#include "../Core/Ray.h"
#include "../Maths/Basis.h"

// A simple camera class - to be replaced in the future

class Camera
{
public:
	Camera() {}
	Camera(Vec3d o, Vec3d d, Vec3d u, double fov, double aspectRatio, double aperture, double focalLength) {

		double theta = fov * M_PI / 180.0;
		double halfHeight = tan(theta / 2.0);
		double halfWidth = aspectRatio * halfHeight;

		// Radius of the lens for depth of field
		lensRadius = aperture / 2.0;

		// Camera local basis
		basis.w = unit_vector(origin - d);
		basis.u = unit_vector(cross_product(u, basis.w));
		basis.v = cross_product(basis.w, basis.u);

		// Position of camera
		origin = o;
		lowerLeft = origin - (halfWidth * focalLength * basis.u) - (halfHeight * focalLength * basis.v) - (focalLength * basis.w);
		horizontal = 2.0 * halfWidth * focalLength * basis.u;
		vertical = 2.0 * halfHeight * focalLength * basis.v;
	}

	// Returns a ray at a random angle taken from a uniformly sampled disk of size equal to the lens
	Ray FetchRay(double u, double v) {
		Vec3d direction = lensRadius * rng.RandomInUnitDiskXY();
		Vec3d offset = basis.u * direction.x() + basis.v * direction.y();
		Ray ret(origin + offset, lowerLeft + u * horizontal + v * vertical - origin - offset);
		return ret;
	}

	Vec3d origin, vertical, lowerLeft, horizontal;
	Basis basis;
	double lensRadius = 0.0;
};

#endif // !CAMERA_H