#ifndef INTERSECTIONRECORD_H
#define INTERSECTIONRECORD_H

#include "../Maths/Vector.h"
class Material;
class Light;

#include <memory>

class IntersectionRecord {
public:
	IntersectionRecord() {}

	Vec3d p = { 0.0,0.0,0.0 };
	Vec2d uv = { 0.0,0.0 };
	double t = 0.0;
	Vec3d normal = { 0.0,0.0,0.0 }; // ALL NORMALS MUST BE UNIT VECTORS
	Material* mat = nullptr;
	Light* light = nullptr;
	int autodeskNodeType = 0;
};

inline std::ostream& operator << (std::ostream& os, const IntersectionRecord& rec) {
	os << "p: " << rec.p << "\n";
	os << "uv: " << rec.uv << "\n";
	os << "t: " << rec.t << "\n";
	os << "normal: " << rec.normal;
	return os;
}

#endif // !INTERSECTIONRECORD_H
