//
//  MetalRenderer.h
//  Gaia
//
//  Created by Alex Richardson on 04/06/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//

@import MetalKit;

// This class performs Metal setup and per-frame rendering

@interface MetalRenderer : NSObject<MTKViewDelegate>

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView;

@end
