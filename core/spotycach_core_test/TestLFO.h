//
//  TestLFO.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 28.06.22.
//

#ifndef TestLFO_h
#define TestLFO_h

#include <gmock/gmock.h>
#include "ILFO.h"

class TestLFO: public ILFO {
public:
    MOCK_METHOD(bool, isOn, (), (override));
    MOCK_METHOD(void, setIsOn, (bool), (override));
    MOCK_METHOD(void, setAmplitude, (double), (override));
    MOCK_METHOD(void, setPeriod, (double), (override));
    MOCK_METHOD(void, setCurrentBeat, (double), (override));
    MOCK_METHOD(void, setFramesPerMeasure, (long), (override));
    MOCK_METHOD(float, triangleValueAt, (int), (override));
};

#endif /* TestLFO_h */
