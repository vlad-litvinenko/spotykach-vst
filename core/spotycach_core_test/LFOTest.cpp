//
//  LFOTest.cpp
//  spotycach_core_test
//
//  Created by Vladyslav Lytvynenko on 21.06.22.
//

//
//  SourceTest.cpp
//  spotycach_core_test
//
//  Created by Vladyslav Lytvynenko on 16.05.22.
//

#include <stdio.h>
#include <cmath>
#include <gtest/gtest.h>
#include "LFO.h"

using namespace ::testing;

float _round(float val) {
    return std::round(val * 1000) / 1000;
}

class LFOTest: public Test {
protected:
    LFO *_lfo;
    
    void SetUp() override {
        _lfo = new LFO();
    }
    
    void TearDown() override {
        delete _lfo;
    }
    
    void doTestTriangle4th() {
        _lfo->setFramesPerMeasure(88200);
        _lfo->setPeriod(0.25);
        
        _lfo->setCurrentBeat(0);
        EXPECT_EQ(_lfo->triangleValueAt(0), 1.);
        
        _lfo->setCurrentBeat(0.125);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0)), 0.5);
        
        _lfo->setCurrentBeat(0.25);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0)), 0);
        
        _lfo->setCurrentBeat(0.5);
        EXPECT_EQ(_lfo->triangleValueAt(0), -1.);
        
        _lfo->setCurrentBeat(0.75);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0)), 0);
        
        _lfo->setCurrentBeat(1.0);
        EXPECT_EQ(_lfo->triangleValueAt(0), 1.);
    }
    
    void doTestTriangleDot8th() {
        _lfo->setFramesPerMeasure(88200);
        _lfo->setPeriod(0.1875);
        
        _lfo->setCurrentBeat(0);
        EXPECT_EQ(_lfo->triangleValueAt(0), 1.);
        
        _lfo->setCurrentBeat(0.1875);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0)), 0);
        
        _lfo->setCurrentBeat(0.375);
        EXPECT_EQ(_lfo->triangleValueAt(0), -1);
        
        _lfo->setCurrentBeat(0.5625);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0)), 0);
        
        _lfo->setCurrentBeat(0.75);
        EXPECT_EQ(_lfo->triangleValueAt(0), 1.);
    }
    
    void doRunThrough() {
        _lfo->setFramesPerMeasure(88200);
        _lfo->setPeriod(0.5);
        double beatIncrement = 512. / 88200;
        double beat = 0;
        while (beat < 2) {
            _lfo->setCurrentBeat(beat);
            printf("############ %f ", beat);
            for (auto f = 0; f < 512; f++) {
                printf("%f\n", _lfo->triangleValueAt(f));
            }
            beat += beatIncrement;
        }
    }
        
};

TEST_F(LFOTest, triangle4th) { doTestTriangle4th(); }
TEST_F(LFOTest, triangleDot8th) { doTestTriangleDot8th(); }
//TEST_F(LFOTest, runThrough) { doRunThrough(); }

