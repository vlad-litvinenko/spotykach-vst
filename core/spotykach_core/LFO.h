//
//  LFO.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 21.06.22.
//

#ifndef LFO_h
#define LFO_h

#include "ILFO.h"

class LFO: public ILFO {
public:
    LFO();
    
    void setFramesPerMeasure(long frames) override;
    float valueAt(double beat, int frame) override;
    
private:
    long _framesPerMeasure;
    long _framesPerBeat;
};

#endif /* LFO_h */
