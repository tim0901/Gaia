#pragma once
#ifndef OBJECT_LIST_H
#define OBJECT_LIST_H

#include "object.h"
#include "vec3.h"

//Object list - stores a list of objects!
class object_list :public object {
public:
	object_list() {}
	object_list(object **l, int n) { list = l; list_size = n; }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record &rec) const;
	object **list;
	int list_size;
};

//Directs ray through list of objects if hit
bool object_list::hit(const ray& r, float t_min, float t_max, hit_record &rec) const {
	hit_record temp_record;
	bool hit_anything = false;
	double closest_so_far = t_max;
	//Iterates through list, returns closest
	for (int i = 0; i < list_size; i++) {
		if (list[i]->hit(r, t_min, closest_so_far, temp_record)) {
			hit_anything = true;
			if (temp_record.t < closest_so_far) {
				closest_so_far = temp_record.t;
				rec = temp_record;
			}

		}
	}
	return hit_anything;

}

#endif // !OBJECT_LIST.H

