#pragma once

#ifndef PORTAL_H
#define PORTAL_H

#include "material.h"
#include "random.h"
#include "object.h"
#include "onb.h"

class portal : public material {
public:
	portal(object* h, object* t, const vec3& a = vec3(1, 1, 1)) : here(h), there(t), albedo(new solid_colour(a)) {
		vec3 vec = vec3(0, 1, 0); // Unlike with make_from_w(), we require all rays to use the same a to avoid issues
		hereSys.build_from_w_with_a(here->normal(), vec);
		thereSys.build_from_w_with_a(there->normal(), vec);
	};

	~portal() { delete here; delete there; delete albedo; }

	virtual bool scatter(const ray& incident, const hit_record& rec, scattering_record& scatter) const;


	virtual std::string type(int i = 0) const {
		return "portal";
	}
	object* here;
	object* there;

	onb hereSys;
	onb thereSys;

	texture* albedo;
};

#endif // PORTAL_H