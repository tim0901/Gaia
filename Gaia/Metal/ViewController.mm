//
//  ViewController.m
//  Gaia mac test
//
//  Created by Alex Richardson on 04/06/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//

#import "ViewController.h"
#import "MetalRenderer.h"
#import "image_parameters.h"

#import <thread>
#import <Carbon/Carbon.h>


extern std::shared_ptr<Image> image;
extern std::shared_ptr<Flags> flags;

@implementation ViewController{
    MTKView *_view; // The view (viewport)
    MetalRenderer *_renderer; // The renderer
}

- (void)viewDidAppear{
    [super viewDidAppear];
    
    // Set the title of the window
    self.view.window.title = [NSString stringWithUTF8String:("Gaia - " + image->fileName).c_str()];
}

// Do any additional setup after loading the view.
- (void)viewDidLoad {
    
    [super viewDidLoad]; // super allows the use of implementations of the current class' superclass
    
    _view = (MTKView *)self.view;
    
    // Look for keyDown events
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown handler:^(NSEvent * keyEvent){
        
        switch (int(keyEvent.keyCode)){
        case kVK_Escape: // If ESC is pressed
            NSLog(@"Escape");
            flags->cancelRender = true; // Tell Gaia to stop rendering
                
            // Keep process running until window is closed and rendering has stopped
            while (flags->threadsActive > 0 && !flags->allSavesComplete) {
		        // Wait
		        std::this_thread::sleep_for(std::chrono::milliseconds(100));
	        }
                
            [NSApp terminate:self]; // And to terminate the window
        }
        return keyEvent;
    }];
    
    // Calling setNeedsDisplay() notifies the system that the view needs to be redrawn. Setting to NO causes it to update automatically
    _view.enableSetNeedsDisplay = NO;
    
    _view.device = MTLCreateSystemDefaultDevice();
    
    _view.clearColor = MTLClearColorMake(0.0, 0.5, 1.0, 1.0); // Set background colour of the view
    
    // Set frame limit
    _view.preferredFramesPerSecond = 30;
    
    _renderer = [[MetalRenderer alloc] initWithMetalKitView:_view];
    
    if(!_renderer){ // Check the renderer initialized correctly
        NSLog(@"MetalRenderer init failed");
        return;
    }
    
    // Init the renderer with the view size.
    [_renderer mtkView:_view drawableSizeWillChange:_view.drawableSize];
    
    _view.delegate = _renderer;
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

@end
