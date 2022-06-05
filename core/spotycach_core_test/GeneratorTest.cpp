//
//  GeneratorTest.cpp
//  spotycach_core_test
//
//  Created by Vladyslav Lytvynenko on 17.05.22.
//

#include <stdio.h>
#include <gtest/gtest.h>
#include "Generator.h"
#include "Source.h"
#include "SliceBuffer.h"
#include "TestEnvelope.h"

using namespace ::testing;

class GeneratorTest: public Test {
protected:
    Generator *_gen;
    Source* _src;
    SliceBuffer* _buf;
    TestEnvelope* _env;
    
    void SetUp() override {
        _src = new Source();
        _src->reset();
        
        _buf = new SliceBuffer();
        _buf->reset();
        
        _env = new TestEnvelope();
        
        _gen = new Generator(*_src, *_buf, *_env);
    }
    
    void TearDown() override {
        delete _gen;
        delete _src;
        delete _buf;
        delete _env;
    }
    
    void doTestGenerateSlice() {
        auto sliceLength = 20;
        _gen->adjustBuffers(50);
        _gen->activateSlice(0, sliceLength, true, true);
        
        for (auto i = 0; i < 100; i++) {
            float out0 = 0;
            float out1 = 0;
            _src->write(i, i);
            
            if (i < sliceLength) {
                EXPECT_CALL(*_env, attackLength);
                EXPECT_CALL(*_env, decayLength);
            }
            _gen->generate(&out0, &out1);
            if (i < sliceLength) {
                EXPECT_EQ(out0, i);
                EXPECT_EQ(out1, i);
            }
        }
    }
    
    void doTestGenerateSliceOffset() {
        auto sliceLength = 8;
        auto offset = 5;
        _src->setMode(SourceMode::flow);
        _gen->adjustBuffers(20);
        float out0 = 0;
        float out1 = 0;
        for (auto i = 0; i < 20; i++) {
            _src->write(i, i);
            if (i == offset) {
                _gen->activateSlice(offset, sliceLength, true, true);
            }
            
            
            if (i >= offset && i < sliceLength + offset) {
                EXPECT_CALL(*_env, attackLength);
                EXPECT_CALL(*_env, decayLength);
            }
            _gen->generate(&out0, &out1);
            if (i >= offset && i < sliceLength + offset) {
                EXPECT_EQ(out0, i);
                EXPECT_EQ(out1, i);
            }
        }
    }
    
    void doTestGenerateSliceOffsetFlow() {
        auto sliceLength = 8;
        auto offset = 5;
        _src->setMode(SourceMode::flow);
        _gen->adjustBuffers(10);
        float out0 = 0;
        float out1 = 0;
        for (auto i = 0; i < 30; i++) {
            _src->write(i, i);
            if (i == offset) {
                _gen->activateSlice(offset, sliceLength, true, true);
            }
            
            
            if (i >= offset && i < sliceLength + offset) {
                EXPECT_CALL(*_env, attackLength);
                EXPECT_CALL(*_env, decayLength);
            }
            _gen->generate(&out0, &out1);
            if (i >= offset && i < sliceLength + offset) {
                EXPECT_EQ(out0, i);
                EXPECT_EQ(out1, i);
            }
        }
    }
};

TEST_F(GeneratorTest, doTestGenerateSlice) { doTestGenerateSlice(); }
TEST_F(GeneratorTest, doTestGenerateSliceOffset) { doTestGenerateSliceOffset(); }
TEST_F(GeneratorTest, doTestGenerateSliceOffsetFlow) { doTestGenerateSliceOffsetFlow(); }