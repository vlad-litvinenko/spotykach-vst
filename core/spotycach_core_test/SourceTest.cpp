//
//  SourceTest.cpp
//  spotycach_core_test
//
//  Created by Vladyslav Lytvynenko on 16.05.22.
//

#include <stdio.h>
#include <gtest/gtest.h>
#include "Source.h"

using namespace ::testing;

class SourceTest: public Test {
protected:
    Source *_src;
    
    void SetUp() override {
        _src = new Source();
        _src->reset();
        _src->size(10);
    }
    
    void TearDown() override {
        delete _src;
    }
    
    void doTestBufferFreeze() {
        for (int i = 0; i < 20; i++) {
            if (i == 15) _src->setFrozen(true);
            _src->write(i, i);
        }
        
        float l10 = 0;
        float l11 = 0;
        _src->read(l10, l11, 4);
        EXPECT_EQ(l10, 14.);
        EXPECT_EQ(l11, 14.);
        
        float l20 = 0;
        float l21 = 0;
        _src->read(l20, l21, 9);
        EXPECT_EQ(l20, 9.);
        EXPECT_EQ(l21, 9.);
    }
    
    void doTestFlow() {
        float out0 = 0;
        float out1 = 0;
        _src->setFrozen(false);
        for (int i = 0; i < 100; i++) {
            _src->write(i, i);
            _src->read(out0, out1, _src->readHead());
            EXPECT_EQ(out0, float(i));
            EXPECT_EQ(out1, float(i));
        }
    }
    
    void doTestFlowOffset() {
        float out0 = 0;
        float out1 = 0;
        _src->setFrozen(false);
        for (int i = 0; i < 100; i++) {
            _src->write(i, i);
            if (i >= 20) {
                _src->read(out0, out1, i + 8); //read at 8-th position of 10-slots buffer.
                EXPECT_EQ(out0, float(i - 2)); //8-th position value is 18 for i==20 (i.e. write head
                EXPECT_EQ(out1, float(i - 2)); //is at 0 after 2nd loop). This relation should remain.
            }
        }
    }
};

TEST_F(SourceTest, bufferFreeze) { doTestBufferFreeze(); }
TEST_F(SourceTest, bufferFlow) { doTestFlow(); }
TEST_F(SourceTest, bufferFlowOffset) { doTestFlowOffset(); }
