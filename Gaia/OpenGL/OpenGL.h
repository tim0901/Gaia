#pragma once

#ifndef OPENGL_H
#define OPENGL_H

#include "image_parameters.h"

int initialise_window(bool* windowOpen, image_parameters* image);
void terminate_window();

#endif //OPENGL_H