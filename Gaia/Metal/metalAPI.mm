//
//  MetalAPI.mm
//  Gaia
//
//  Created by Alex Richardson on 31/05/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MetalAPI.h"

#import <Metal/Metal.h>
#import <AppKit/AppKit.h>
#import <AppKit/NSApplication.h>
#import <Cocoa/Cocoa.h>

int metalWindow(int argc, const char * argv[]){
    NSLog(@"Metal window start\n");
    @autoreleasepool {
       return NSApplicationMain(argc, argv); // Start the Metal application
    }
}
