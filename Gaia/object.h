#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "hit_record.h"
#include "ray.h"

class object{
public:
	//Hit function. Takes ray, returns a hit_record of the object if it hits anything
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};
#endif // !OBJECT_H