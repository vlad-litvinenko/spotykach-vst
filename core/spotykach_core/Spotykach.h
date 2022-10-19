//
//  Spotykach.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 22.05.22.
//

#ifndef Spotykach_h
#define Spotykach_h

#include "Engine.h"
#include "Parameters.h"
#include <array>
#include <bitset>
#include <vector>

namespace vlly {
namespace spotykach {

static const int kEnginesCount = UnitsCount;

struct SpotykachRawParameters {
    double vol[kEnginesCount]   = { 1.0, 1.0, 1.0, 1.0 };
    double mix                  = 1.0;
    double mainVol              = 1.0;
    int mutex                   = 0;
    bool cascade[kEnginesCount] = { false, false, false, false };
    bool ownBus[kEnginesCount]  = { false, false, false, false };
};

class Spotykach {
public:
    Spotykach();
    ~Spotykach();
    
    Engine& engineAt(int index);
    long enginesCount();
    
    void setMutex(int mutex);
    
    void setMix(double normVal);
    
    void setMainVolume(double normVal);
    
    void setVolume(double value, int index);
    
    void setCascade(bool value, int index);
    
    void sendToOwnBus(bool value, int index) { _raw.ownBus[index] = value; };
    
    void initialize(int sampleRate);
    void preprocess(PlaybackParameters p);
    void process(float** inBuf, bool inMono, float** outBuf[kEnginesCount], bool outMono, int numFrames);
    
    SpotykachRawParameters rawParameters() { return _raw; }
    
    void resetCascadeOf(int index) {
        int nextIndex = index + 1;
        if (_cascade[nextIndex]) engineAt(nextIndex).reset(false);
    }
    
private:
    std::array<std::shared_ptr<Engine>, kEnginesCount> _engines;
    
    SpotykachRawParameters _raw;
    
    double _vol[kEnginesCount];
    std::bitset<kEnginesCount> _cascade;
    
    double _mix;
    double _mainVol;
    Mutex _mutex;
    
    std::vector<std::shared_ptr<void>> _releasePool;
};
}
}

#endif /* Spotykach_h */
