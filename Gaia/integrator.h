#pragma once

#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "image_parameters.h"
#include "object.h"
#include "camera.h"

void render(bool *window_open, image_parameters *image, int k, object **world, camera **cam_Ptr);


#endif // !INTEGRATOR_H
