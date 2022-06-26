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
    
    void setPeriod(double) override;
    void setFramesPerMeasure(long frames) override;
    float triangleValueAt(double beat, int frame) override;
    
private:
    long _framesPerMeasure { 0 };
    long _framesPerBeat { 0 };
    double _period { 0 };
    double _amp { 2 };
 };

#endif /* LFO_h */
