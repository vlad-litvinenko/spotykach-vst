//
//  SpotykachView.m
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 22/07/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#import "SpotykachView.h"
#import "ParameterIdentifiers.h"

NSString * const SpotykachViewNib = @"SpotykachView";

@interface SpotykachView () {
    AudioUnitParameter		mParameter[kNumberOfParams];
    AUParameterListenerRef	mParameterListener;
}

@property (nonatomic, weak) IBOutlet NSView *displayView;

@property (nonatomic, weak) IBOutlet NSSlider *startSlider;
@property (nonatomic, weak) IBOutlet NSSlider *sliceSlider;
@property (nonatomic, weak) IBOutlet NSSlider *stepSlider;
@property (nonatomic, weak) IBOutlet NSButton *syncButton;
@property (nonatomic, weak) IBOutlet NSButton *declickButton;
@property (nonatomic, weak) IBOutlet NSButton *reverseButton;
@property (nonatomic, weak) IBOutlet NSButton *backAndForthButton;

@property (nonatomic, weak) IBOutlet NSSlider *occursSlider;
@property (nonatomic, weak) IBOutlet NSButton *retriggerButton;

@property (nonatomic, weak) IBOutlet NSSlider *mixSlider;
@property (nonatomic, weak) IBOutlet NSMatrix *mixModeMatrix;

- (void)parameterListener:(void *)inObject parameter:(const AudioUnitParameter *)inParameter value:(Float32)inValue;

@end

void ParameterListener(void *inUserData, void *inObject, const AudioUnitParameter *inParameter, AudioUnitParameterValue inValue) {
    SpotykachView *selfView = (__bridge SpotykachView *)inUserData;
    [selfView parameterListener:inObject parameter:inParameter value:inValue];
}

@implementation SpotykachView

#pragma mark - Lifecycle
- (void)dealloc {
    [self unsubscribeFromAUNotes];
}

#pragma mark - Audio Unit Setter
- (void)setSpotykachAU:(AudioUnit)value {
    
    if (_spotykachAU) {
        [self unsubscribeFromAUNotes];
    }
    
    _spotykachAU = value;
    
    [self subscribeToAUNotes];
    //[self synchronize];
}

#pragma mark - Core Notifications
- (void)subscribeToAUNotes {
    void *listen = (__bridge void *)(self);
    OSStatus listenerCreateResult = AUListenerCreate(ParameterListener, listen, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0.1, &mParameterListener);
    if (noErr == listenerCreateResult) {
        for (int i = 0; i < kNumberOfParams; i++) {
            mParameter[i].mAudioUnit = self.spotykachAU;
            mParameter[i].mScope = kAudioUnitScope_Global;
            mParameter[i].mElement = 0;
            mParameter[i].mParameterID = i;
            AUListenerAddParameter(mParameterListener, NULL, &mParameter[i]);
        }
    }
}
- (void)unsubscribeFromAUNotes {
    for (int i = 0; i < kNumberOfParams; i++) {
        AUListenerRemoveParameter(mParameterListener, NULL, &mParameter[i]);
    }
    AUListenerDispose(mParameterListener);
}
- (void)synchronize {
	Float32 value;
    int i;
    for (i = 0; i < kNumberOfParams; i++) {
        AudioUnitGetParameter(self.spotykachAU, mParameter[i].mParameterID, kAudioUnitScope_Global, 0, &value);
        AUParameterSet(mParameterListener, (__bridge void *)(self), &mParameter[i], value, 0);
        AUParameterListenerNotify(mParameterListener, (__bridge void *)(self), &mParameter[i]);
    }
}
- (void)parameterListener:(void *)inObject parameter:(const AudioUnitParameter *)inParameter value:(Float32)inValue {
    switch (inParameter->mParameterID) {
        case kParamID_Slice_Start:
            self.startSlider.floatValue = inValue;
            break;
        case kParamID_Slice_Length:
            self.sliceSlider.floatValue = inValue;
            break;
        case kParamID_Slice_Step:
            self.stepSlider.floatValue = inValue;
            break;
        case kParamID_Reverse:
            self.reverseButton.state = inValue;
            break;
        case kParamID_BackNForth:
            self.backAndForthButton.state = inValue;
            break;
        case kParamID_RetriggerAfter:
            self.occursSlider.floatValue = inValue;
            break;
        case kParamID_Retrigger:
            self.retriggerButton.state = inValue;
            break;
        case kParamID_Sync:
            self.syncButton.state = inValue;
            break;
        case kParamID_Mix:
            self.mixSlider.floatValue = inValue;
            break;
        case kParamID_Declick:
            self.declickButton.state = inValue;
            break;
        case kParamID_Mode :
            [self.mixModeMatrix selectCellAtRow:inValue column:0];
            break;
    }
}

#pragma mark - Start / Slice / Step
- (IBAction)startSliderChanged:(NSSlider *)slider {
    AUParameterSet(mParameterListener, (__bridge void *)(slider), &mParameter[kParamID_Slice_Start], slider.floatValue, 0);
}
- (IBAction)sliceSliderChanged:(NSSlider *)slider {
    AUParameterSet(mParameterListener, (__bridge void *)(slider), &mParameter[kParamID_Slice_Length], slider.floatValue, 0);
}
- (IBAction)stepSliderChanged:(NSSlider *)slider {
    AUParameterSet(mParameterListener, (__bridge void *)(slider), &mParameter[kParamID_Slice_Step], slider.floatValue, 0);
}
- (IBAction)syncButtonClicked:(NSButton *)button {
    AUParameterSet(mParameterListener, (__bridge void *)(button), &mParameter[kParamID_Sync], button.state, 0);
}
- (IBAction)declickButtonClicked:(NSButton *)button {
    AUParameterSet(mParameterListener, (__bridge void *)(button), &mParameter[kParamID_Declick], button.state, 0);
}
- (IBAction)reverseButtonClicked:(NSButton *)button {
    AUParameterSet(mParameterListener, (__bridge void *)(button), &mParameter[kParamID_Reverse], button.state, 0);
}
- (IBAction)backAndForthButtonClicked:(NSButton *)button {
    AUParameterSet(mParameterListener, (__bridge void *)(button), &mParameter[kParamID_BackNForth], button.state, 0);
}

#pragma mark - Occurs / Retrigger
- (IBAction)occurSliderChanged:(NSSlider *)slider {
    AUParameterSet(mParameterListener, (__bridge void *)(slider), &mParameter[kParamID_RetriggerAfter], slider.floatValue, 0);
}
- (IBAction)retriggerButtonClicked:(NSButton *)button {
    AUParameterSet(mParameterListener, (__bridge void *)(button), &mParameter[kParamID_Retrigger], button.state, 0);
}

#pragma mark - Mix
- (IBAction)mixSliderChanged:(NSSlider *)slider {
    AUParameterSet(mParameterListener, (__bridge void *)(slider), &mParameter[kParamID_Mix], slider.floatValue, 0);
}
- (IBAction)mixModeMatrixChanged:(NSMatrix *)matrix {
    AUParameterSet(mParameterListener, (__bridge void *)(matrix), &mParameter[kParamID_Mode], (CGFloat)matrix.selectedRow, 0);
}

@end
