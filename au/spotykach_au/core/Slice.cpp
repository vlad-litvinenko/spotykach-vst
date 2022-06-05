//
//  Slice.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Slice.h"


Slice::Slice(Source& inSource, Envelope& inEnvelope) :
    _source(inSource),
    _envelope(inEnvelope)
{
    _offset = 0;
    _length = 0;
    _iterator = 0;
    _direction = 0;
    _channelsCount = inSource.getChannelsCount();
}

void Slice::activate(int offset, int length, int direction) {
    _offset = offset;
    _length = length;
    _iterator = direction < 0 ? _length - 1 : 0;
    _direction = direction;
    _active = true;
}

void Slice::synthesize() {
    
    float out0 = _source.readChannelFrame(0, _iterator + _offset);
    float out1 = _source.readChannelFrame(1, _iterator + _offset);
    
    float attenuation = 1;
    if (_iterator < _envelope.attackLength()) {
        attenuation = _envelope.attackAttenuation(_iterator);
    }
    else if (_iterator > _length - _envelope.decayLength()) {
        attenuation = _envelope.decayAttenuation(_iterator - _length + _envelope.decayLength());
    }
    
    _out0 = out0 * attenuation;
    _out1 = out1 * attenuation;
    
    this->next();
}

float Slice::out0() {
    return _out0;
}

float Slice::out1() {
    return _out1;
}

bool Slice::isActive() {
    return _active;
}

bool Slice::isInactive() {
    return !_active;
}

void Slice::next() {
    this->_iterator += this->_direction;
    if (this->_iterator == 0 || _iterator == _length) {
        _active = false;
    }
}
