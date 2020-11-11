//
//  bvh.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "bvh.h"
#include "medianbvh.h"
#include "sahbvh.h"

bvh* bvh::constructBVH(object** objectList, int numPrimitives, float time0, float time1, int type) {
    switch (type) {
    case MEDIAN:
        return new medianBVH(objectList, numPrimitives, time0, time1);
    case SAH:
        return new sahBVH(objectList, numPrimitives, time0, time1);
    case SBVH:
        std::cout << "SBVH" << std::endl;
    }
}