//
//  Source.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Source.h"

Source::Source() {
    _buffer = new std::vector<float>[2];
    this->writeHeadRewind();
}

void Source::adjustBufferSize(double framesPerMeasure) {
    _framesPerMeasure = framesPerMeasure;
    if (framesPerMeasure > this->_buffer[0].size()) {
        _buffer[0].resize(framesPerMeasure);
        _buffer[1].resize(framesPerMeasure);
    }
}

void Source::write(float in0, float in1) {
    _buffer[0][this->_writeHead] = in0;
    _buffer[1][this->_writeHead] = in1;
    _writeHead ++;
}

void Source::writeHeadRewind() {
    _writeHead = 0;
}

bool Source::bufferFilled() {
    return _writeHead == _framesPerMeasure;
}

float Source::readChannelFrame(int channel, long frameIndex) {
    return _buffer[channel][frameIndex];
}

int Source::getChannelsCount() {
    return _channelsCount;
}