//
//  MetalShaderTypes.h
//  Gaia mac test
//
//  Created by Alex Richardson on 04/06/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//

#ifndef MetalShaderTypes_h
#define MetalShaderTypes_h

#include <simd/simd.h>

// Buffer index values that are shared between shader and C API code to ensure the Metal shader buffer inputs match the Metal API buffer set calls

typedef enum VertexInputIndex{
    VertexInputIndexVertices = 0,
    VertexInputIndexViewportSize = 1,
} VertexInputIndex;

// Texture index values shared between shader and C to ensure Metal shader buffer
// inputs match API texture set calls
typedef enum TextureIndex{
    TextureIndexBaseColor = 0,
} TextureIndex;

// Struct defining the layout of vertices sent to the vert shader. Shared to ensure compatability between C API and .metal shaders.
typedef struct{
    // Position in pixel space relative to origin
    vector_float2 position;
    
    // 2D texture coords
    vector_float2 textureCoordinates;
} MetalVertex;

#endif /* MetalShaderTypes_h */
