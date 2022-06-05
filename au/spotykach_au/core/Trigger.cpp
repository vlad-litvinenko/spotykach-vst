//
//  Trigger.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Trigger.h"
#include <vector>

static const double kMaxSlicesPerMeasure    = 128.0;
static const int kDefaultQuadrat            = 4;
static const int kSecondsInMinute           = 60;

inline long roundIterator(long iterator, long iteratorBase) {
    return lround(iterator / static_cast<double>(iteratorBase)) * iteratorBase;
}
inline double synchroniseStep(double step) {
    return kMaxSlicesPerMeasure / round(kMaxSlicesPerMeasure / step);
}

Trigger::Trigger(Generator &inGenerator, Source& inSource) :
    _generator(inGenerator),
    _source(inSource)
{
    _repeatsTillRetrigger = INT32_MAX;
}

void Trigger::measure(double tempo, float numerator, int denominator, int sampleRate) {
    
    _numerator = numerator;
    _denominator = denominator;

    _syncInterval = _numerator;

    float beatsPerMeasure = kDefaultQuadrat * numerator / denominator;
    _framesPerMeasure = static_cast<long>(kSecondsInMinute * sampleRate * beatsPerMeasure / tempo);
    _framesPerBeat = _framesPerMeasure / numerator;

    _stepKoef = (_framesPerMeasure * _denominator) / (_numerator * kMaxSlicesPerMeasure);
}

void Trigger::schedule(float step, float sliceLength, bool sync) {
    
    _sync = sync;

    if (sync) {
        sliceLength = synchroniseStep(sliceLength);
        step = synchroniseStep(step);
    }

    long framesPerSlice = static_cast<long>(_stepKoef * sliceLength);
    long framesPerStep = static_cast<long>(_stepKoef * step);
    
    if (framesPerSlice > framesPerStep) {
        framesPerStep = framesPerSlice - (framesPerSlice - framesPerStep) / 2;
    }
    
    _framesPerCrossFade = (framesPerStep < framesPerSlice) ? (framesPerSlice - framesPerStep) / 2 : 0;
    
    _framesPerStep = framesPerStep;
    _framesPerSlice = framesPerSlice;

}

void Trigger::run(double startBeat) {
    _lastSync = startBeat;
    _running = true;
}

void Trigger::stop() {
    _running = false;
}

bool Trigger::isRunning() {
    return _running;
}

void Trigger::scheduleSync(double currentBeat) {
    _framesTillSync =  static_cast<int>((_lastSync + _syncInterval - currentBeat) * _framesPerBeat);
}

long Trigger::repeats() {
    return _repeats;
}

void Trigger::setRetrigger(bool retrigger) {
    _retrigger = retrigger;
}

void Trigger::setRepeatsTillRetrigger(int repeatsTillRetrigger) {
    _repeatsTillRetrigger = repeatsTillRetrigger;
}

long Trigger::getFramesPerMeasure() {
    return _framesPerMeasure;
}

long Trigger::getFramesPerCrossFade() {
    return _framesPerCrossFade;
}

void Trigger::next() {

    if (_sync && (--_framesTillSync) <= 0) {
        _iterator = roundIterator(_iterator, _framesPerStep) + 1;
        _lastSync += _syncInterval;
    }

    if ((--_iterator) <= 0) {

        bool forceForward = false;
        if (_retrigger && _repeats % _repeatsTillRetrigger == 0) {
            _source.writeHeadRewind();
            forceForward = true;
        }

        _generator.activateSlice(0, _framesPerSlice, forceForward);

        _repeats ++;

        _iterator = _framesPerStep;

    }
}

void Trigger::reset() {
    _running = false;
    _iterator = 0;
    _repeats = 0;
}
