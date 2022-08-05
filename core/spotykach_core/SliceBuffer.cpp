//
//  SliceBuffer.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 17.05.22.
//

#include <stdio.h>
#include <algorithm>
#include "SliceBuffer.h"

static int kBufferSeconds = 3;

void SliceBuffer::initialize(int sampleRate) {
    _size = kBufferSeconds * sampleRate;
    _buffer[0] = (float *)malloc(_size * sizeof(float));
    _buffer[1] = (float *)malloc(_size * sizeof(float));
    reset();
}

float SliceBuffer::read(int channel, long frame) {
    if (frame >= _size) return 0;
    return _buffer[channel][frame];
}

void SliceBuffer::write(float in0, float in1) {
    _buffer[0][_writeHead] = in0;
    _buffer[1][_writeHead] = in1;
    _writeHead ++;
}

void SliceBuffer::rewind() {
    _writeHead = 0;
}

bool SliceBuffer::isFull() {
    return _writeHead == _size;
}

void SliceBuffer::reset() {
    rewind();
    memset(_buffer[0], 0, _size * sizeof(float));
    memset(_buffer[1], 0, _size * sizeof(float));
    
}
