//
//  LFO.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 21.06.22.
//

#include "LFO.h"
#include <cmath>

template<typename T>
inline float mod(T a, T b) {
    return ((a % b) + b) % b;
}

LFO::LFO() {}

void LFO::setAmplitude(double amplitude) {
    _amp = 2 * amplitude;
}

void LFO::setPeriod(double value) {
    _period = value;
}

void LFO::setCurrentBeat(double value) {
    _currentBeat = value;
}

void LFO::setFramesPerMeasure(long framesPerMeasure) {
    _framesPerMeasure = framesPerMeasure;
    _framesPerBeat = _framesPerMeasure / 4;
}

float LFO::triangleValueAt(int frame) {
    long fp = _framesPerMeasure * _period / 2;
    double fraction = _currentBeat - int(_currentBeat / (4 * _period)) * (4 * _period);
    long fphase = static_cast<long>(fraction * _framesPerBeat + frame);
    return (_amp / fp) * (fp - std::abs(mod(fphase - fp, 2 * fp) - fp)) - _amp / 2;
}
