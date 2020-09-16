//
//  ellipse.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "ellipse.h"

//Ray-ellipse intersection test
bool ellipse::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin() - centre;

    vec3 out = vec3(dot(cross(radius1, radius2), oc), dot(cross(oc, radius1), r.direction()), dot(cross(radius2, oc), r.direction())) / dot(cross(radius2, radius1), r.direction());
       
    vec2 axisTest = vec2(out.y(), out.z());

    float t = (dot(axisTest, axisTest) < 1.0) ? out.x() : -1.0;


    if (t > 0.0) {
        rec.object_id = object_id;
        rec.primitive_id = primitive_id;
        rec.type = type;
        rec.t = t;
        rec.p = r.p_at_t(rec.t);

        vec3 norm = cross(radius1, radius2);
        norm.make_unit_vector();
        rec.normal = norm;

        // Convert the global hit location to the local coordinate system
        vec3 local = localCoordinateSystem.toLocal(rec.p - centre);
        
        // Now transform to cylindrical coords
        rec.u = sqrt((local.x() * local.x()) + (local.y() * local.y()));

        rec.v = atan2(local.y() , local.x());

        rec.mat_ptr = mat_ptr;
        rec.primitive = true;
        return true;
    }
    else {
        return false;
    }

}
