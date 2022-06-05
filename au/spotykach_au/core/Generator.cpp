//
//  Generator.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/01/15.
//  Copyright (c) 2015 Vladyslav Lytvynenko. All rights reserved.
//

#include "Generator.h"

static const int kSlicesCount = 10;

Generator::Generator(Source& inSource, Envelope& inEnvelope) :
    _source(inSource)
{
    _slices = std::vector<Slice *>(kSlicesCount);
    
    for (int i = 0; i < kSlicesCount; i++) {
        _slices[i] = new Slice(inSource, inEnvelope);
    }
}

void Generator::generate() {
    
    _out0 = 0;
    _out1 = 0;
    
    for (int i = 0; i < kSlicesCount; i++) {
        Slice *slice = _slices[i];
        if (slice->isActive()) {
            slice->synthesize();
            _out0 += slice->out0();
            _out1 += slice->out1();
        }
    }
}

void Generator::reset () {
    _fwd = true;
}

void Generator::activateSlice(int inFramesTillStart, int inFramesPerSlice, bool forceForward) {
    for (auto *slice : _slices) {
        if (slice->isInactive()) {
            if (forceForward) {
                _fwd = true;
            }
            else {
                _fwd = _bnf ? !_fwd : !_rev;
            }
            int direction = _fwd ? 1 : -1;
            slice->activate(inFramesTillStart, inFramesPerSlice, direction);
            break;
        }
    }
}

void Generator::setReverse(bool reverse) {
    _rev = reverse;
}

void Generator::setBackAndForth(bool backAndForth) {
    _bnf = backAndForth;
}

float Generator::out0() {
    return _out0;
}

float Generator::out1() {
    return _out1;
}