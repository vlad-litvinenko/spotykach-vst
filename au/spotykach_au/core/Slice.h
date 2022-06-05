//
//  Slice.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Slice__
#define __Spotykach__Slice__

#include "Source.h"
#include "Envelope.h"

class Slice {
    
public:
    Slice(Source& inSource, Envelope& inEnvelope);
    
    ~Slice();
    
    bool isActive();
    bool isInactive();
    
    void activate(int offset, int length, int direction);
    
    void synthesize();
    
    float out0();
    float out1();
    
private :
    
    Source& _source;
    Envelope& _envelope;
    
    float *_declickIn;
    float *_declickOut;
    
    int _channelsCount;
    
    int _offset;
    int _length;
    bool _active;
    
    int _iterator;
    int _direction;
    
    float _out0;
    float _out1;
    
    void next();
};

#endif
