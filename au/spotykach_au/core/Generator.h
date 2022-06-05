//
//  Generator.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/01/15.
//  Copyright (c) 2015 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Synth__
#define __Spotykach__Synth__

#include <stdio.h>
#include "Source.h"
#include "Envelope.h"
#include "Slice.h"

class Generator {
    
public:
    
    Generator(Source& source, Envelope &envelope);
    
    void activateSlice(int inFramesTillStart, int inFramesPerSlice, bool forceForward);
    
    void generate();
    
    void reset();
    
    void setReverse(bool reverse);
    void setBackAndForth(bool backAndForth);
    void setDeclick();
    
    float out0();
    float out1();
    
private:
    
    std::vector<Slice *> _slices;
    
    Source& _source;
    
    bool _fwd;
    bool _rev;
    bool _bnf;
    
    bool _declick;
    
    float _out0;
    float _out1;
};



#endif /* defined(__Spotykach__Synth__) */
