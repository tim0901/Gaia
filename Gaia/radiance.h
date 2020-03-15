#pragma once

#ifndef RADIANCE_H
#define RADIANCE_H

#include "ray.h"

#include "objects.h"
#include "material.h"
#include "pdf.h"

#include "vec4.h"
#include "image_parameters.h"

vec4 cast(image_parameters *image, const ray& r, object *world, object* light_list, int depth);

hit_record edge_cast(image_parameters *image, const ray& r, object *world, int depth);
#endif // !RADIANCE_H
