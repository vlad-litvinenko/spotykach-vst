//
//  Spotykach.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 02/03/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "AUMIDIEffectBase.h"
#include "SpotykachVersion.h"

#include "LockedTrigger.h"
#include "Source.h"
#include "Envelope.h"
#include "Generator.h"

using namespace ausdk;

#ifndef Spotykach_Spotykach_h
#define Spotykach_Spotykach_h

class Spotykach : public AUMIDIEffectBase {
    
public:
    Spotykach(AudioUnit component);
    
    virtual OSStatus Initialize();

    virtual OSStatus Reset(AudioUnitScope   inScope,
                           AudioUnitElement inElement);

    virtual void Cleanup();
    
    virtual	ComponentResult GetParameterInfo (AudioUnitScope            inScope,
                                              AudioUnitParameterID      inParameterID,
                                              AudioUnitParameterInfo	&outParameterInfo);
    
    virtual OSStatus GetParameterValueStrings(AudioUnitScope        inScope,
                                              AudioUnitParameterID	inParameterID,
                                                         CFArrayRef *outStrings);
    
    virtual OSStatus ProcessBufferLists(AudioUnitRenderActionFlags  &ioActionFlags,
                                        const AudioBufferList       &inBuffer,
                                        AudioBufferList             &outBuffer,
                                        UInt32                      inFramesToProcess );
    
    virtual OSStatus HandleNoteOn(UInt8 	inChannel,
                                  UInt8 	inNoteNumber,
                                  UInt8 	inVelocity,
                                  UInt32    inStartFrame);
    
	virtual OSStatus HandleNoteOff(UInt8 	inChannel,
                                   UInt8 	inNoteNumber,
                                   UInt8 	inVelocity,
                                   UInt32 	inStartFrame);
    
    virtual OSStatus GetPropertyInfo (AudioUnitPropertyID   inID,
                                      AudioUnitScope        inScope,
                                      AudioUnitElement      inElement,
                                      UInt32                &outDataSize,
                                      bool               &outWritable);
    
    virtual OSStatus GetProperty(AudioUnitPropertyID    inID,
                                 AudioUnitScope 		inScope,
                                 AudioUnitElement       inElement,
                                 void *                 outData);
    
    virtual OSStatus Version() { return kSpotykachVersion; }
    
private:
    
    void PassThrough(const  AudioBufferList &inBuffer,
                            AudioBufferList &outBuffer);
    
    void InitValues();
    
    Source *_source;
    LockedTrigger *_trigger;
    Envelope *_envelope;
    Generator *_generator;
    
    double  _tempo;
    int     _start;
    float   _step;
    float   _slice;
    bool    _sync;

};

#endif
