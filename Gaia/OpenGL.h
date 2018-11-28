#pragma once

#ifndef OPENGL_H
#define OPENGL_H

#include "image_parameters.h"

int initialise_window(image_parameters* image, bool *windowOpen);
void terminate_window();

#endif //OPENGL_H