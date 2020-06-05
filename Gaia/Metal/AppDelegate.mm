//
//  AppDelegate.m
//  Gaia mac test
//
//  Created by Alex Richardson on 04/06/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//

#import "AppDelegate.h"

#include "cleanup.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
    return YES;
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    cleanup();
}


@end
