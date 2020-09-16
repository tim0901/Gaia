#pragma once

#ifndef PORTAL_H
#define PORTAL_H

#include "material.h"
#include "random.h"
#include "object.h"
#include "onb.h"

class portal : public material {
public:
	portal(object* h, object* t, const vec3& a = vec3(1, 1, 1)) : here(h), there(t), albedo(a) {};
	~portal() { delete here; delete there; }

	virtual bool scatter(const ray& incident, const hit_record& rec, scattering_record& scatter) const;

	std::string type = "portal";
	object* here;
	object* there;

	vec3 albedo;
};

#endif // PORTAL_H