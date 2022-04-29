#include "Ray.h"


Ray FromAToB(const Vec3d& v1, const Vec3d& v2) {
 return Ray(v1, v2 - v1);
}