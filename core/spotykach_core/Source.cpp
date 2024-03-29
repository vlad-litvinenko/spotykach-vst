//
//  Source.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Source.h"
#include <vector>
#include <algorithm>

static int kBufferSeconds = 10;

Source::Source() :
    _frozen(false),
    _filled(false),
    _writeHead(0),
    _readHead(0) {
}

void Source::setFrozen(bool frozen) {
    _frozen = frozen;
}

unsigned long Source::readHead() {
    return _readHead;
}

void Source::initialize(int sampleRate) {
    _bufferLength = kBufferSeconds * sampleRate;
    _buffer[0] = (float* )malloc(_bufferLength * sizeof(float));
    _buffer[1] = (float* )malloc(_bufferLength * sizeof(float));
    reset();
}

void Source::read(float& out0, float& out1, unsigned long frame) {
    frame %= _bufferLength;
    out0 = _buffer[0][frame];
    out1 = _buffer[1][frame];
}

void Source::write(float in0, float in1) {
    if (!_frozen || !_filled) {
        _buffer[0][_writeHead] = in0;
        _buffer[1][_writeHead] = in1;
    }
    _readHead = _writeHead;
    _writeHead++;
    _writeHead %= _bufferLength;
    if (_writeHead == 0) _filled = true;
}

void Source::reset() {
    memset(_buffer[0], 0, _bufferLength * sizeof(float));
    memset(_buffer[1], 0, _bufferLength * sizeof(float));
    _writeHead = 0;
    _readHead = 0;
    _filled = false;
}
