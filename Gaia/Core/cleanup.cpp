//
//  cleanup.cpp
//  Gaia
//
//  Created by Alex Richardson on 05/06/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//

#include <stdio.h>

#include "cleanup.h"

#include "image_parameters.h"
#include "object.h"
#include "camera.h"

// Globals to be cleaned up
extern image_parameters* image;
extern bool* window_open;
extern object* world;
extern object* light_list;
extern material** matList;
extern camera* cam;

// Cleanup function to be called by AppDelegate on applicationWillTerminate
// or after OpenGL window has closed

void cleanup(){
    if(world){
        delete world;
    }
    if(light_list){
        delete light_list;
    }
    if(matList){
        delete[] matList;
    }
    if(cam){
        delete cam;
    }
    if(image){
        delete image;
    }
    if(window_open){
        delete window_open;
    }
    std::cout << "Cleanup complete" << std::endl;
}
