//
//  MetalShaders.metal
//  Gaia mac test
//
//  Created by Alex Richardson on 04/06/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//

#include <metal_stdlib>
#include <simd/simd.h> // Vector and matrix functions

using namespace metal;


// This header is shared between the C Metal API and the Metal shader code
#import "MetalShaderTypes.h"


typedef struct {
    // The [[position]] attribute indicates that this values is the clip space
    // position of the vertex when this struct is returned from the vertex function
    float4 position[[position]];
    
    // This member doesn't have a special attribute, so the rasterizer interprets
    // its value with the values of the other triangle verts and then passes the
    // interpolated value to the frag shader for each fragment in that triangle
    float2 textureCoordinates;
    
} RasterizerData;

// Vertex function
vertex RasterizerData vertexShader(uint vertexID [[vertex_id]],
                                   constant MetalVertex *vertices [[buffer(VertexInputIndexVertices)]],
                                   constant vector_uint2 *viewportSizePointer [[buffer(VertexInputIndexViewportSize)]]){
    
    RasterizerData out;
    
    // Get the current vertex.
    // Positions are specified in pixel dimensions from the origin
    // ie. value of 100 = 100 pixels from origin.
    float2 pixelSpacePosition = vertices[vertexID].position.xy;
    
    // Get viewport size, cast to float
    vector_float2 viewportSize = vector_float2(*viewportSizePointer);
    
    // To convert from pixel space positions to clip-space positions, divide the pixel coordinates by half of the viewport size
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.xy = pixelSpacePosition / (viewportSize * 0.5);
    
    out.textureCoordinates = vertices[vertexID].textureCoordinates;
    
    return out;
}

// Fragment function
fragment float4 fragmentShader(RasterizerData in [[stage_in]],
                               texture2d<half> colorTexture [[ texture(TextureIndexBaseColor) ]]){
    constexpr sampler textureSampler (mag_filter::linear,
                                      min_filter::linear);
    
    // Sample the texture to obtain a colour
    const half4 colorSample = colorTexture.sample(textureSampler, in.textureCoordinates);
    
    
    // Return interpolated colour.
    return float4(colorSample);
}
