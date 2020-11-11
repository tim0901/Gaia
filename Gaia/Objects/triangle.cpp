//
//  triangle.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "triangle.h"

bool triangle::hit(const ray& r, float tmin, float tmax, hit_record& rec) const {

    // Translate vertices based on ray origin
    vec3 p0 = point0 - vec3(r.origin());
    vec3 p1 = point1 - vec3(r.origin());
    vec3 p2 = point2 - vec3(r.origin());

    // Permute components of triangle vertices and ray direction
    int kz = max_dimension(abs(r.direction()));
    int kx = kz + 1;
    if (kx == 3)
        kx = 0;
    int ky = kx + 1;
    if (ky == 3)
        ky = 0;
    vec3 d = permute(r.direction(), kx, ky, kz);
    p0 = permute(p0, kx, ky, kz);
    p1 = permute(p1, kx, ky, kz);
    p2 = permute(p2, kx, ky, kz);

    // Apply shear transformation to translated vertex positions
    float Sx = -d.x() / d.z();
    float Sy = -d.y() / d.z();
    float Sz = 1.f / d.z();
    p0[0] += Sx * p0.z();
    p0[1] += Sy * p0.z();
    p1[0] += Sx * p1.z();
    p1[1] += Sy * p1.z();
    p2[0] += Sx * p2.z();
    p2[1] += Sy * p2.z();

    // Compute edge function coefficients _e0_, _e1_, and _e2_
    float e0 = p1.x() * p2.y() - p1.y() * p2.x();
    float e1 = p2.x() * p0.y() - p2.y() * p0.x();
    float e2 = p0.x() * p1.y() - p0.y() * p1.x();

    // Perform triangle edge and determinant tests
    if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
        return false;
    float det = e0 + e1 + e2;
    if (det == 0) return false;

    // Compute scaled hit distance to triangle and test against ray $t$ range
    p0[2] *= Sz;
    p1[2] *= Sz;
    p2[2] *= Sz;
    float tScaled = e0 * p0.z() + e1 * p1.z() + e2 * p2.z();
    
    if (det < 0 && (tScaled >= 0 || tScaled < FLT_MAX * det))
        return false;
    else if (det > 0 && (tScaled <= 0 || tScaled > FLT_MAX * det))
        return false;

    // Compute barycentric coordinates and $t$ value for triangle intersection
    float invDet = 1 / det;
    float b0 = e0 * invDet;
    float b1 = e1 * invDet;
    float b2 = e2 * invDet;
    float t = tScaled * invDet;

    // Compute triangle partial derivatives
    vec3 dpdu, dpdv;
    vec2 uv[3];
    //TODO Fetch proper UVs from mesh
    uv[0] = vec2(0, 0);
    uv[1] = vec2(1, 0);
    uv[2] = vec2(1, 1);

    // Compute deltas for triangle partial derivatives
    //vec2 duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
    vec3 dp02 = point0 - point2, dp12 = point1 - point2;
    
    // Interpolate $(u,v)$ parametric coordinates and hit point
    vec3 pHit = b0 * point0 + b1 * point1 + b2 * point2;
    //vec2 uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];
    
    vec3 norm;
    if (normal == vec3(0, 0, 0)) {
        norm = vec3(unit_vector(cross(dp02, dp12)));
    }

    rec.object_id = object_id;
    rec.primitive_id = primitive_id;
    rec.mat_ptr = mat_ptr;
    rec.t = t;
    rec.p = pHit;
    //rec.u = uvHit.u();
    //rec.v = uvHit.v();
    rec.normal = vec3(unit_vector(cross(dp02, dp12)));
    rec.type = type;
    rec.primitive = true;

    //broken?
    /*if (normal0 != (0,0,0)) {
        rec.normal = unit_vector(b0* *normal0 + b1 * *normal1 + b2 * *normal2);
    }
    else {
        rec.normal = vec3(unit_vector(cross(dp02, dp12)));
    }*/
    return true;
}

bool triangle::bounding_box(float t0, float t1, aabb& box) const {
    vec3 p0 = point0;
    vec3 p1 = point1;
    vec3 p2 = point2;

    vec3 pmin(std::min(p0.x(), std::min(p1.x(), p2.x())), std::min(p0.y(), std::min(p1.y(), p2.y())), std::min(p0.z(), std::min(p1.z(), p2.z())));
    vec3 pmax(std::max(p0.x(), std::max(p1.x(), p2.x())), std::max(p0.y(), std::max(p1.y(), p2.y())), std::max(p0.z(), std::max(p1.z(), p2.z())));
    
    box = aabb(pmin, pmax);
    return true;
}
