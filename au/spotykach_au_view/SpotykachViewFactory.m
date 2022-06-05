//
//  SpotykachViewFactory.m
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 22/07/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#import "SpotykachViewFactory.h"
#import "SpotykachView.h"

@interface SpotykachViewFactory()

@property (nonatomic, strong) IBOutlet SpotykachView *spotykachView;

@end

@implementation SpotykachViewFactory

- (unsigned)interfaceVersion {
    return 0x01;
}

- (NSView *)uiViewForAudioUnit:(AudioUnit)inAudioUnit withSize:(NSSize)inPreferredSize {
    NSArray *nibTopLevelObjects = nil;
    NSNib *viewNib = [[NSNib alloc] initWithNibNamed:SpotykachViewNib bundle:[NSBundle bundleForClass:[SpotykachView class]]];
    [viewNib instantiateWithOwner:self topLevelObjects:&nibTopLevelObjects];
    
    SpotykachView *result = self.spotykachView;
    self.spotykachView = nil;
    result.spotykachAU = inAudioUnit;
    
    return result;
}

@end
