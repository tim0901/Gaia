//
//  portal.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/09/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "portal.h"

bool portal::scatter(const ray& incident, const hit_record& rec, scattering_record& scatter) const {

	/// Shift position from 'here' to 'there'
	// This assumes both portals are idential in shape and size
	
	// Fetch the UV coords of the ray collision on the surface of 'here'
	vec2 uvPosition = here->positionToUV(rec.p);

	// Convert the UV coords to the corresponding spatial coords on the surface of 'there'
	vec3 therePosition = there->UVToPosition(uvPosition);

	/// Transform ray direction to the new surface
	
	// Starting with the direction vector of the incident ray
	vec3 incDir = incident.direction();

	// Find this vector in the local coordinate system of 'here'
	incDir = hereSys.toLocal(incDir);

	// Now we can convert it back from the local coordinate system of 'there'
	incDir = thereSys.toGlobal(incDir);

	scatter.specular_ray = ray(therePosition, incDir);
	scatter.brdf = albedo->value(vec2(rec.u, rec.v), rec.p);
	scatter.is_specular = true;
	scatter.pdf = 0;

	return true;
}