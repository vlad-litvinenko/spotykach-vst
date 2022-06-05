//
//  Source.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Source__
#define __Spotykach__Source__

#include <vector>

class Source {
    
public:
    Source();
    
    void adjustBufferSize(double framesPerMeasure);
    
    void write(float in0, float in1);
    void writeHeadNext();
    void writeHeadRewind();
    
    bool bufferFilled();
    
    float readChannelFrame(int channel, long frameIndex);
    
    int getChannelsCount();
    
private:
    int _channelsCount;
    long _framesPerMeasure;
    long _writeHead;
    std::vector<float> *_buffer;
};

#endif
