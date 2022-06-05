//
//  Envelope.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/01/15.
//  Copyright (c) 2015 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Envelope__
#define __Spotykach__Envelope__

#include <stdio.h>

class Envelope {
    
public:
    
    Envelope();
    
    void setDeclick(bool inDeclick);
    
    int attackLength();
    int decayLength();
    
    void setFramesPerCrossfade(int inFrames);
    
    float attackAttenuation(int currentFrame);
    float decayAttenuation(int currentFrame);

private:
    
    bool _declick;
    
    int _attackLength;
    int _decayLength;
    int _framesPerCrossfade;
    
    void measure();
    
};

#endif /* defined(__Spotykach__Envelope__) */
