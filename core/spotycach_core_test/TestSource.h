//
//  TestSource.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 16.03.22.
//

#ifndef TestSource_h
#define TestSource_h

#include <gmock/gmock.h>

class TestSource: public ISource {
public:
    MOCK_METHOD(void, setFrozen, (bool), (override));
    MOCK_METHOD(bool, isFrozen, (), (override));
    MOCK_METHOD(unsigned long, readHead, (), (override));
    MOCK_METHOD(void, initialize, (int), (override));
    MOCK_METHOD(void, write, (float, float), (override));
    MOCK_METHOD(void, read, (float&, float&, unsigned long), (override));
    MOCK_METHOD(bool, isFilled, (), (override));
    MOCK_METHOD(void, reset, (), (override));
};

#endif /* TestSource_h */
