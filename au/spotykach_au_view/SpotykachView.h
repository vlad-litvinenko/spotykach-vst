//
//  SpotykachView.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 22/07/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>

extern NSString * const SpotykachViewNib;

@interface SpotykachView : NSView

@property (nonatomic, assign) AudioUnit spotykachAU;

@end