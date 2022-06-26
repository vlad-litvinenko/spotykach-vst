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
        
        EXPECT_EQ(_lfo->triangleValueAt(0, 0), 1.);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0.125, 0)), 0.5);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0.25, 0)), 0);
        EXPECT_EQ(_lfo->triangleValueAt(0.5, 0), -1.);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0.75, 0)), 0);
        EXPECT_EQ(_lfo->triangleValueAt(1.0, 0), 1.);
    }
    
    void doTestTriangleDot8th() {
        _lfo->setFramesPerMeasure(88200);
        _lfo->setPeriod(0.1875);
        
        EXPECT_EQ(_lfo->triangleValueAt(0, 0), 1.);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0.1875, 0)), 0);
        EXPECT_EQ(_lfo->triangleValueAt(0.375, 0), -1);
        EXPECT_EQ(_round(_lfo->triangleValueAt(0.5625, 0)), 0);
        EXPECT_EQ(_lfo->triangleValueAt(0.75, 0), 1.);
    }
        
};

TEST_F(LFOTest, triangle4th) { doTestTriangle4th(); }
TEST_F(LFOTest, triangleDot8th) { doTestTriangleDot8th(); }

