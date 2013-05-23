//
//  MacTestAppDelegate.h
//  MacTest
//
//  Created by Robert Konrad on 25.07.11.
//  Copyright 2011 KonTechs Limited. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MacTestAppDelegate : NSObject <NSApplicationDelegate> {
	NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end
