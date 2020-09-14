//
//  radiance.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "radiance.h"

vec4 cast(image_parameters *image, const ray& r, object *world, object* light_list, int depth, float& numberOfIntersectionTests) {
    hit_record rec;
    scattering_record scatter;
    
    //To generate a heat map, we add the number of primitive intersection tests to the number of traversal steps for each primary ray. 
    numberOfIntersectionTests++; //Therefore we iterate this value by one each time cast() is called, to account for the number of traversal steps. 

    if (world->hit(r, 0.001, FLT_MAX, rec)) {

        vec3 emitted(0,0,0);
        
        //Will reflect up to a depth of 50 times before it gives up
        //std::cout << scatter.is_specular << std::endl;
        
        //These shouldn't be here. Object id = -1, type hit_record, material type "M"??? or ""
        if(rec.object_id == -1){
            //std::cout << "Phantom object: ignored." << std::endl;
            //std::cout << rec.object_id << " " << rec.type << " " << rec.mat_ptr->type << std::endl;
            return vec4(0,0,0,1); //Therefore we remove them to prevent segfaults.
        }
        
        bool hit = rec.mat_ptr->scatter(r, rec, scatter);
        emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

        numberOfIntersectionTests += rec.numberOfIntersectionTests; // Add the number of intersection tests performed to the overall counter for this primary ray. 

        vec4 emitteda = vec4(emitted.r(), emitted.g(), emitted.b(), 1);
        vec4 brdf = vec4(scatter.brdf.r(), scatter.brdf.g(), scatter.brdf.b(), 1);

        if (depth < image->maxDepth && hit) {
            if (scatter.is_specular) {
                vec4 temp = brdf * cast(image, scatter.specular_ray, world, light_list, depth + 1, numberOfIntersectionTests);
                delete scatter.pdf;
                return vec4(temp.r(), temp.g(), temp.b(), 1);
            }
            else {
                //Path tracing equation

                object_pdf lights_pdf(light_list , rec.p);
                mixture_pdf mix_pdf(&lights_pdf, scatter.pdf);
                ray scattered = ray(rec.p, mix_pdf.generate());
                float pdf = mix_pdf.pdf_value(scattered.direction());
                
                delete scatter.pdf;

                vec4 temp = emitteda + brdf * rec.mat_ptr->scattering_pdf(r, rec, scattered) * cast(image, scattered, world, light_list, depth + 1, numberOfIntersectionTests) / pdf;
                return vec4(temp.r(), temp.g(), temp.b(), 1);
            }
            
        }
        else {
            //Terminate ray
            return emitteda;
        }


    }
    else {
        //Background colour
        return vec4(image->background_colour->r(), image->background_colour->g(), image->background_colour->b(), 0);
        /*vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
        */
    }
}

hit_record edge_cast(image_parameters *image, const ray& r, object *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
        
        return rec;

    }
    else {
        //Background colour
        return rec;
    }
}
