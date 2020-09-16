//
//  portal.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/09/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "portal.h"

bool portal::scatter(const ray& incident, const hit_record& rec, scattering_record& scatter) const {

	// Shift position from 'here' to 'there'
	// This assumes both portals are idential in shape and size
	
	// Fetch the UV coords of the ray collision on the surface of 'here'
	vec2 uvPosition = here->positionToUV(rec.p);

	// Convert the UV coords to the corresponding spatial coords on the surface of 'there'
	vec3 therePosition = there->UVToPosition(uvPosition);

	/// Transform ray direction 
	
	// Starting with the direction vector of the incident ray
	vec3 incDir = incident.direction();

	// Find this vector in the local coordinate system of 'here'
	onb hereSys;
	hereSys.axis[2] = unit_vector(here->normal());
	vec3 a = vec3(0, 1, 0); // Unlike make_from_w(), we require all rays to use the same a to avoid issues

	hereSys.axis[1] = unit_vector(cross(hereSys.w(), a));
	hereSys.axis[0] = cross(hereSys.w(), hereSys.v());

	hereSys.inverseAxis[0] = vec3(hereSys.axis[0][0], hereSys.axis[1][0], hereSys.axis[2][0]);
	hereSys.inverseAxis[1] = vec3(hereSys.axis[0][1], hereSys.axis[1][1], hereSys.axis[2][1]);
	hereSys.inverseAxis[2] = vec3(hereSys.axis[0][2], hereSys.axis[1][2], hereSys.axis[2][2]);

	incDir = hereSys.toLocal(incDir);

	// Now we can convert it back from the local coordinate system of 'there'

	onb thereSys;
	thereSys.axis[2] = unit_vector(there->normal());
	vec3 b = vec3(0, 1, 0); // Unlike make_from_w(), we require all rays to use the same a to avoid issues

	thereSys.axis[1] = unit_vector(cross(thereSys.w(), b));
	thereSys.axis[0] = cross(thereSys.w(), thereSys.v());

	thereSys.inverseAxis[0] = vec3(thereSys.axis[0][0], thereSys.axis[1][0], thereSys.axis[2][0]);
	thereSys.inverseAxis[1] = vec3(thereSys.axis[0][1], thereSys.axis[1][1], thereSys.axis[2][1]);
	thereSys.inverseAxis[2] = vec3(thereSys.axis[0][2], thereSys.axis[1][2], thereSys.axis[2][2]);

	incDir = thereSys.toGlobal(incDir);

	// Get incoming ray direction and transform it to new surface
	vec3 newDirection = incDir;// (-incident.direction())* there->normal();

	scatter.specular_ray = ray(therePosition, newDirection);
	scatter.brdf = albedo;
	scatter.is_specular = true;
	scatter.pdf = 0;

	return true;
}