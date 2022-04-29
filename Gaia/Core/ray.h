#ifndef RAY_H
#define RAY_H

#include "../Maths/Vector.h"

class Ray
{
public:
	Ray() {}
	Ray(Vec3d o, Vec3d d) : origin(o), direction(d) {};

	Vec3d PositionAtt(double t) const {
		return (origin + t * direction);
	}

	Vec3d origin = Vec3d(0.0,0.0,0.0), direction = Vec3d(0.0, 0.0, 0.0);
};

inline std::ostream& operator << (std::ostream& os, const Ray& r) {
	std::cout << "Origin: " << r.origin << " Direction: " << r.direction;
	return os;
}

Ray FromAToB(const Vec3d& v1, const Vec3d& v2);

#endif // !RAY_H