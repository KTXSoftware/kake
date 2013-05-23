//
//  TestAppDelegate.h
//  Test
//
//  Created by Robert Konrad on 23.07.11.
//  Copyright 2011 KonTechs Limited. All rights reserved.
//

#import <UIKit/UIKit.h>

@class TestViewController;

@interface TestAppDelegate : NSObject <UIApplicationDelegate>

@property (nonatomic, retain) IBOutlet UIWindow *window;

@property (nonatomic, retain) IBOutlet TestViewController *viewController;

@end
