//
//  Envelope.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/01/15.
//  Copyright (c) 2015 Vladyslav Lytvynenko. All rights reserved.
//

#include "Envelope.h"

static const float kP_2    = 1.5707963267949;
static const float kCoef3  = 0.16666666666667;
static const float kCoef5  = 0.00833333333333;
static const float kCoef7  = 0.00019841269841;

inline float sine(float x) {

    //x-3+5-7

    float x3 = x * x * x;

    float x5 = x3 * x * x;

    float x7 = x5 * x * x;

    return x - kCoef3 * x3 + kCoef5 * x5 - kCoef7 * x7;
}

Envelope::Envelope() {
    
}

void Envelope::setDeclick(bool inDeclick) {
    
    _declick = inDeclick;
    
    measure();
}

void Envelope::setFramesPerCrossfade(int inFrames) {
    
    _framesPerCrossfade = inFrames;
    
    measure();
    
}

void Envelope::measure() {

    if (_framesPerCrossfade > 0) {
        _attackLength = _decayLength = _framesPerCrossfade;
    }
    else {
        _attackLength = _decayLength = _declick ? 512 : 0;
    }
}

int Envelope::attackLength() {
    return _attackLength;
}

int Envelope::decayLength() {
    return _decayLength;
}

float Envelope::attackAttenuation(int currentFrame) {
        
    return _attackLength ? sine(kP_2 * currentFrame / _attackLength) : 1;
}

float Envelope::decayAttenuation(int currentFrame) {

    return _decayLength ? sine(kP_2 + kP_2 * currentFrame / _decayLength) : 1;
}
