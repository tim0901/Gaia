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
    
    //Destructor
    ~object_list(){
        if(list){
            //Delete each object in the list in turn
            //int numberDeleted = 0;
            for(int i = 0; i < list_size; i++){
                delete list[i];
              //  numberDeleted++;
            }
            //std::cout << numberDeleted << " objects deleted." << std::endl;
        }
        else{
            std::cout << "No list to delete" << std::endl;
        }
    }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb &box) const;
	
	virtual float pdf_value(const vec3& o, const vec3& v) const;
	virtual vec3 random(const vec3& o) const;

    float object_id = -2;
    float primitive_id = -2;
    std::string type = "object_list";
	object **list;
	int list_size;
};

#endif // !OBJECT_LIST.H

