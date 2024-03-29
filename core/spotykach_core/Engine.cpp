//
//  Engine.cpp
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 02.04.22.
//

#include <stdio.h>
#include "Engine.h"

using namespace vlly;
using namespace spotykach;

inline int gridStepCount(Grid grid) {
    switch (grid) {
        case kGrid_Even: return EvenStepsCount;
        case kGrid_Odd: return OddStepsCount;
        case kGrid_CWord: return CWordsCount;
    }
}

Engine::Engine(ITrigger& t, ISource& s, IEnvelope& e, IGenerator& g, ILFO& l):
    _trigger { t },
    _source { s },
    _envelope { e },
    _generator { g },
    _jitterLFO { l },
    _step { 0.0625 },
    _tempo { 0 },
    _onsets { 0 }
{
    setIsOn(false);
    setSlicePosition(0.0);
    setShift(0.0);
    setGrid(1.0);
    setStepPosition(4.0 / (CWordsCount - 1));
    setSliceLength(0.5);
    setRepeats(9);
    setDirection(0);
    setRetrigger(0);
    setDeclick(false);
    setRetriggerChance(1);
    setJitterAmount(0);
    setJitterRate(0.75);
    setFrozen(false);
    _trigger.prepareMeterPattern(_step, 0, 4, 4);
}

void Engine::setIsOn(bool on) {
    _raw.on = on;
    _isOn = on;
};

void Engine::setShift(double normVal) {
    _raw.shift = normVal;
    double shiftValue = normVal * 15 / 16;
    if (shiftValue != _shift) {
        _shift = shiftValue; 
        _invalidatePattern = true;
    }
}

void Engine::setStepPosition(double normVal) {
    _raw.stepGridPosition = normVal;
    int maxIndex;
    int valueIndex;
    int onsets = _onsets;
    double step = _step;
    switch (_grid) {
        case kGrid_Even: {
            maxIndex = EvenStepsCount - 1;
            valueIndex = std::min(maxIndex, int(normVal * (maxIndex + 1)));
            step = EvenSteps[valueIndex].value;
            break;
        }
        case kGrid_Odd: {
            maxIndex = OddStepsCount - 1;
            valueIndex = std::min(maxIndex, int(normVal * (maxIndex + 1)));
            step = OddSteps[valueIndex].value;
            break;
        }
        case kGrid_CWord: {
            maxIndex = CWordsCount - 1;
            valueIndex = std::min(maxIndex, int(normVal * (maxIndex + 1)));
            onsets = CWords[valueIndex].onsets;
            break;
        }
    }
    
    if (step != _step) {
        _step = step;
        _invalidatePattern = true;
    }
    else if (onsets != _onsets) {
        _onsets = onsets;
        _invalidatePattern = true;
    }
}

void Engine::setGrid(double normVal) {
    _raw.grid = normVal;
    Grid grid = spotykach::Grid(normVal * (kGrids_Count - 1));
    if (grid != _grid) {
        _grid = grid;
        setStepPosition(_raw.stepGridPosition);
        _invalidatePattern = true;
    }
}

void Engine::setSlicePosition(double normVal) {
    _raw.slicePosition = normVal;
    double start = std::min(normVal, 127./128.);
    if (start != _start) {
        _start = start;
        _invalidateSlicePosition = true;
    }
}

void Engine::setSliceLength(double normVal) {
    _raw.sliceLength = normVal;
    double slice = fmax(normVal, 1./128.);
    if (slice != _slice) {
        _slice = slice;
        _invalidateSliceLength = true;
    }
}

int Engine::pointsCount() {
    return _trigger.pointsCount();
}

void Engine::setRepeats(double normVal) {
    _raw.repeats = normVal;
    _trigger.setRepeats(round(normVal * pointsCount()));
}

void Engine::setRetrigger(double normVal) {
    _raw.retrigger = normVal;
    _trigger.setRetrigger(round(normVal * 16));
}

void Engine::setRetriggerChance(bool value) {
    _raw.retriggerChance = value;
    _trigger.setRetriggerChance(value);
}

void Engine::setJitterAmount(double value) {
    _raw.jitterAmount = value;
    _jitterLFO.setAmplitude(value);
}

void Engine::setJitterRate(double value) {
    _jitterLFO.setPeriod(1. - value);
}

void Engine::setDeclick(bool declick) {
    _raw.declick = declick;
    _envelope.setDeclick(declick);
}

void Engine::setDirection(double normVal) {
    _raw.direction = normVal;
    Direction direction = static_cast<Direction>(round(normVal * (kDirections_Count - 1)));
    _generator.setDirection(direction);
}

void Engine::setFrozen(bool frozen) {
    _raw.frozen = frozen;
    _source.setFrozen(frozen);
}

void Engine::initialize(int sampleRate) {
    _source.initialize(sampleRate);
    _generator.initialize(sampleRate);
    _isInitialized = true;
}

void Engine::preprocess(PlaybackParameters p) {
    bool isLaunch = false;
    if (p.isPlaying != _isPlaying) {
        _isPlaying = p.isPlaying;
        isLaunch = _isPlaying;
        reset(true);
    }
    
    if (p.tempo != _tempo) {
        _tempo = p.tempo;
        _trigger.measure(p.tempo, p.sampleRate, p.bufferSize);
    }
    
    if (_invalidatePattern) {
        if (_grid == Grid::kGrid_CWord) {
            _trigger.prepareCWordPattern(_onsets, _shift, p.numerator, p.denominator);
        }
        else {
            _trigger.prepareMeterPattern(_step, _shift, p.numerator, p.denominator);
        }
        _invalidateSliceLength = true;
        _invalidatePattern = false;
    }
    
    if (_invalidateSlicePosition) {
        _trigger.setSlicePosition(_start);
        _invalidateSlicePosition = false;
    }
    
    if (_invalidateSliceLength) {
        _trigger.setSliceLength(_slice, _envelope);
        _invalidateSliceLength = false;
    }
    
    if (_isPlaying) {
        _trigger.schedule(p.currentBeat, isLaunch);
    }
}

void Engine::process(float in0, float in1, float* out0, float* out1, bool engaged) {
    _trigger.next(_isOn && engaged);
    _source.write(in0, in1);
    _generator.generate(out0, out1);
}

void Engine::reset(bool hard) {
    _source.reset();
    _generator.reset();
    if (hard) _trigger.reset();
    
}
