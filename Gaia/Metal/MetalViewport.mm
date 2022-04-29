
#import "MetalViewport.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <AppKit/NSApplication.h>
#import <Cocoa/Cocoa.h>

// Check that the device supports Metal.
bool MetalSupportCheck(){
	id<MTLDevice> _device = MTLCreateSystemDefaultDevice();
	if(_device){
		return true;
	}
	else{
		return false;
	}
}

// Start and maintain the Metal viewport
void UpdateViewport(int argc, const char* argv[]){
	NSLog(@"Metal window start\n");
	@autoreleasepool{
		return NSAApplicationMain(argc, argv); // Start the metal application
	}
}