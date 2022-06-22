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

LFO::LFO(): _framesPerBeat(0), _framesPerMeasure(0) {}

void LFO::setFramesPerMeasure(long framesPerMeasure) {
    _framesPerMeasure = framesPerMeasure;
    _framesPerBeat = _framesPerMeasure / 4;
}

float LFO::valueAt(double beat, int frame) {
    float a = 2;
    double fraction = beat - int(beat);
    long p = _framesPerBeat / 2;
    long phase = static_cast<long>(fraction * _framesPerBeat + frame);
    return (a / p) * (p - std::abs(mod(phase - p, 2 * p) - p)) - a / 2;
}
