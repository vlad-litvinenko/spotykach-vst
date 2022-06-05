//
//  Trigger.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Scheduler__
#define __Spotykach__Scheduler__

#include <vector>
#include "Generator.h"
#include "Source.h"

class Trigger {
    
public:
    Trigger(Generator& inGenerator, Source& inSource);
    
    void measure(double tempo, float numerator, int denominator, int sampleRate);
    
    void schedule(float step, float sliceLength, bool sync);

    void scheduleSync(double currentBeat);

    void run(double startBeat);
    void stop();
    bool isRunning();

    void next();
    
    void reset();

    long repeats();

    void setRetrigger(bool retrigger);
    void setRepeatsTillRetrigger(int repeatsTillRetrigger);
    
    long getFramesPerMeasure();
    long getFramesPerCrossFade();
    
private:
    
    Generator& _generator;
    Source& _source;
    
    float _numerator;
    int _denominator;

    double _stepKoef;

    bool _running;

    bool _sync;
    double _syncInterval;
    double _lastSync;
    long _framesTillSync;

    long _repeats;

    bool _retrigger;
    long _repeatsTillRetrigger;
    
    long _framesPerMeasure;
    long _framesPerBeat;

    long _framesPerStep;
    long _framesPerSlice;
    long _framesPerCrossFade;
    
    long _iterator;
};

#endif
