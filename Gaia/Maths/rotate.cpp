//
//  rotate.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "rotate.h"

rotate_x::rotate_x(object* p, float angle) : obj_ptr(p) {
    float radians = (M_PI / 180.) * angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = obj_ptr->bounding_box(0, 1, bbox);
    vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                float x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                float y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                float z = k * bbox.max().z() + (1 - k) * bbox.min().z();
                float newy = cos_theta * y - sin_theta * z;
                float newz = sin_theta * y + cos_theta * z;
                vec3 tester(x, newy, newz);
                for (int c = 0; c < 3; c++)
                {
                    if (tester[c] > max[c])
                        max[c] = tester[c];
                    if (tester[c] < min[c])
                        min[c] = tester[c];
                }
            }
        }
    }
    bbox = aabb(min, max);
}

bool rotate_x::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 origin = r.origin();
    vec3 direction = r.direction();
    origin[1] = cos_theta * r.origin()[1] + sin_theta * r.origin()[2];
    origin[2] = -sin_theta * r.origin()[1] + cos_theta * r.origin()[2];
    direction[1] = cos_theta * r.direction()[1] + sin_theta * r.direction()[2];
    direction[2] = -sin_theta * r.direction()[1] + cos_theta * r.direction()[2];
    ray rotated_ray(origin, direction);
    if (obj_ptr->hit(rotated_ray, t_min, t_max, rec)) {
        vec3 p = rec.p;
        vec3 normal = rec.normal;
        p[1] = cos_theta * rec.p[1] - sin_theta * rec.p[2];
        p[2] = sin_theta * rec.p[1] + cos_theta * rec.p[2];
        normal[1] = cos_theta * rec.normal[1] - sin_theta * rec.normal[2];
        normal[2] = sin_theta * rec.normal[1] + cos_theta * rec.normal[2];
        rec.p = p;
        rec.normal = normal;
        return true;
    }
    else
        return false;
}

rotate_y::rotate_y(object* p, float angle) : obj_ptr(p) {
    float radians = float((M_PI / 180.0) * angle);
    sintheta = sin(radians);
    costheta = cos(radians);

    hasbox = obj_ptr->bounding_box(0, 1, bbox);
    vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                float x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                float y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                float z = k * bbox.max().z() + (1 - k) * bbox.min().z();

                float newx = costheta * x + sintheta * z;
                float newz = -sintheta * x + costheta * z;

                vec3 tester(newx, y, newz);
                for (int c = 0; c < 3; c++) {
                    if (tester[c] > max[c]) {
                        max[c] = tester[c];
                    }
                    if (tester[c] < min[c]) {
                        min[c] = tester[c];
                    }
                }

            }
        }
    }
    bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 origin = r.origin();
    vec3 direction = r.direction();

    origin[0] = costheta * r.origin()[0] - sintheta * r.origin()[2];
    origin[2] = sintheta * r.origin()[0] + costheta * r.origin()[2];

    direction[0] = costheta * r.direction()[0] - sintheta * r.direction()[2];
    direction[2] = sintheta * r.direction()[0] + costheta * r.direction()[2];
    ray rotated_ray(origin, direction);
    if (obj_ptr->hit(rotated_ray, t_min, t_max, rec)) {
        vec3 p = rec.p;
        vec3 normal = rec.normal;
        p[0] = costheta * rec.p[0] + sintheta * rec.p[2];
        p[2] = -sintheta * rec.p[0] + costheta * rec.p[2];
        normal[0] = costheta * rec.normal[0] + sintheta * rec.normal[2];
        normal[2] = -sintheta * rec.normal[0] + costheta * rec.normal[2];

        rec.p = p;
        rec.normal = normal;
        return true;
    }
    else {
        return false;
    }

}

rotate_z::rotate_z(object* p, float angle) : obj_ptr(p) {
    float radians = (M_PI / 180.) * angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = obj_ptr->bounding_box(0, 1, bbox);
    vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                float x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                float y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                float z = k * bbox.max().z() + (1 - k) * bbox.min().z();
                float newx = cos_theta * x - sin_theta * y;
                float newy = sin_theta * x + cos_theta * y;
                vec3 tester(newx, newy, z);
                for (int c = 0; c < 3; c++)
                {
                    if (tester[c] > max[c])
                        max[c] = tester[c];
                    if (tester[c] < min[c])
                        min[c] = tester[c];
                }
            }
        }
    }
    bbox = aabb(min, max);
}

bool rotate_z::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 origin = r.origin();
    vec3 direction = r.direction();
    origin[0] = cos_theta * r.origin()[0] + sin_theta * r.origin()[1];
    origin[1] = -sin_theta * r.origin()[0] + cos_theta * r.origin()[1];
    direction[0] = cos_theta * r.direction()[0] + sin_theta * r.direction()[1];
    direction[1] = -sin_theta * r.direction()[0] + cos_theta * r.direction()[1];
    ray rotated_ray(origin, direction);
    if (obj_ptr->hit(rotated_ray, t_min, t_max, rec)) {
        vec3 p = rec.p;
        vec3 normal = rec.normal;
        p[0] = cos_theta * rec.p[0] - sin_theta * rec.p[1];
        p[1] = sin_theta * rec.p[0] + cos_theta * rec.p[1];
        normal[0] = cos_theta * rec.normal[0] - sin_theta * rec.normal[1];
        normal[1] = sin_theta * rec.normal[0] + cos_theta * rec.normal[1];
        rec.p = p;
        rec.normal = normal;
        return true;
    }
    else
        return false;
}

