//
//  Spotykach.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 02/03/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Spotykach.h"
#include "Parameters.h"
#include <math.h>

using Base = AUMIDIEffectBase;

CFStringRef makeCFString(const char* source) {
    return CFStringCreateWithCString(NULL, source, kCFStringEncodingUnicode);
};

Spotykach::Spotykach(AudioUnit component) : Base(component) {
    
    CreateElements();
    
    Globals()->UseIndexedParameters(kNumberOfParams);
	Globals()->SetParameter(kParamID_Slice_Length,   kDefValue_Slice_Length);
    Globals()->SetParameter(kParamID_Slice_Step,     kDefValue_Slice_Step);
    Globals()->SetParameter(kParamID_Reverse,        kDefValue_Reverse);
    Globals()->SetParameter(kParamID_BackNForth,     kDefValue_BackNForth);
    Globals()->SetParameter(kParamID_Retrigger,      kDefValue_Retrigger);
    Globals()->SetParameter(kParamID_RetriggerAfter, kDefValue_RetriggerAfter);
    Globals()->SetParameter(kParamID_Sync,           kDefValue_Sync);
    Globals()->SetParameter(kParamID_Declick,        kDefValue_Declick);
    Globals()->SetParameter(kParamID_Mix,            kDefValue_Mix);
    
    _source     = new Source();
    _envelope   = new Envelope();
    _generator  = new Generator(*_source, *_envelope);
    _trigger    = new LockedTrigger(*_generator, *_source);
};

AUSDK_COMPONENT_ENTRY(ausdk::AUMIDIEffectFactory, Spotykach);

void Spotykach::InitValues() {
    
    _tempo               = 0;
    _sync                = -1;
    _start               = -1;
    _step                = -1;
    _slice               = -1;
}

OSStatus Spotykach::Initialize() {
    OSStatus result = AUMIDIEffectBase::Initialize();
    this->InitValues();
    _trigger->reset();
    return result;
};
void Spotykach::Cleanup() {
    AUEffectBase::Cleanup();
}

OSStatus Spotykach::Reset(AudioUnitScope   inScope,
                          AudioUnitElement inElement) {
    OSStatus result = Base::Reset(inScope, inElement);
    
    this->InitValues();
    _trigger->reset();
    
    return result;
}

ComponentResult Spotykach::GetParameterInfo (AudioUnitScope         inScope,
                                             AudioUnitParameterID   inParameterID,
                                             AudioUnitParameterInfo &outParameterInfo) {
    
    ComponentResult result = noErr;
    
    outParameterInfo.flags = kAudioUnitParameterFlag_IsReadable | kAudioUnitParameterFlag_IsWritable;
    
    if (kAudioUnitScope_Global == inScope) {
        switch (inParameterID) {

            case kParamID_Slice_Length :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_Slice_Length),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Generic;
                outParameterInfo.minValue = kMinValue_Slice_Length;
                outParameterInfo.maxValue = kMaxValue_Slice_Length;
                outParameterInfo.defaultValue = kDefValue_Slice_Length;
                
                break;
                
            case kParamID_Slice_Step :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_Slice_Step),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Generic;
                outParameterInfo.minValue = kMinValue_Slice_Step;
                outParameterInfo.maxValue = kMaxValue_Slice_Step;
                outParameterInfo.defaultValue = kDefValue_Slice_Step;
                break;
            case kParamID_Reverse :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_Reverse),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Boolean;
                outParameterInfo.minValue = kMinValue_Reverse;
                outParameterInfo.maxValue = kMaxValue_Reverse;
                outParameterInfo.defaultValue = kDefValue_Reverse;
                break;
            case kParamID_BackNForth :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_BackNForth),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Boolean;
                outParameterInfo.minValue = kMinValue_BackNForth;
                outParameterInfo.maxValue = kMaxValue_BackNForth;
                outParameterInfo.defaultValue = kDefValue_BackNForth;
                break;
            case kParamID_RetriggerAfter :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_RetriggerAfter),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Generic;
                outParameterInfo.minValue = kMinValue_RetriggerAfter;
                outParameterInfo.maxValue = kMaxValue_RetriggerAfter;
                outParameterInfo.defaultValue = kDefValue_RetriggerAfter;
                break;
            case kParamID_Retrigger :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_Retrigger),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Boolean;
                outParameterInfo.minValue = kMinValue_Retrigger;
                outParameterInfo.maxValue = kMaxValue_Retrigger;
                outParameterInfo.defaultValue = kDefValue_Retrigger;
                break;
            case kParamID_Sync :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_Sync),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Boolean;
                outParameterInfo.minValue = kMinValue_Sync;
                outParameterInfo.maxValue = kMaxValue_Sync;
                outParameterInfo.defaultValue = kDefValue_Sync;
                break;
            case kParamID_Declick :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_Declick),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Boolean;
                outParameterInfo.minValue = kMinValue_Declick;
                outParameterInfo.maxValue = kMaxValue_Declick;
                outParameterInfo.defaultValue = kDefValue_Declick;
                break;
            case kParamID_Mix :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_Mix),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Generic;
                outParameterInfo.minValue = kMinValue_Mix;
                outParameterInfo.maxValue = kMaxValue_Mix;
                outParameterInfo.defaultValue = kDefValue_Mix;
                break;
            case kParamID_Mode :
                AUBase::FillInParameterName (outParameterInfo,
                                             makeCFString(kParamName_Mode),
                                             false
                                             );
                outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
                outParameterInfo.minValue = kMode_Replace;
                outParameterInfo.maxValue = kMode_Mix;
                outParameterInfo.defaultValue = kDefValue_Mode;
                break;
            default :
                result = kAudioUnitErr_InvalidParameter;
        }
    } else {
        result = kAudioUnitErr_InvalidParameter;
    }
    
    return result;
};

OSStatus Spotykach::GetParameterValueStrings(AudioUnitScope         inScope,
                                             AudioUnitParameterID	inParameterID,
                                             CFArrayRef             *outStrings) {
    if ((kAudioUnitScope_Global == inScope) && (kParamID_Mode == inParameterID)) {
        if (NULL == outStrings) {
            return noErr;
        }
        
        int numberOfStrings = 2;
        CFStringRef strings[numberOfStrings];
        strings[0] = makeCFString(kModeName_Replace);
        strings[1] = makeCFString(kModeName_Mix);
        
        *outStrings = CFArrayCreate(kCFAllocatorDefault, (const void **)strings, numberOfStrings, NULL);
        return noErr;
    }
    return AUBase::GetParameterValueStrings(inScope, inParameterID, outStrings);
};

OSStatus Spotykach::HandleNoteOn(UInt8      inChannel,
                                 UInt8      inNoteNumber,
                                 UInt8      inVelocity,
                                 UInt32     inStartFrame) {
    OSStatus result = AUMIDIBase::HandleNoteOn(inChannel, inNoteNumber, inVelocity, inStartFrame);
    
    
    
    return result;
}

OSStatus Spotykach::HandleNoteOff(UInt8     inChannel,
                                  UInt8     inNoteNumber,
                                  UInt8     inVelocity,
                                  UInt32    inStartFrame) {
    OSStatus result = AUMIDIBase::HandleNoteOff(inChannel, inNoteNumber, inVelocity, inStartFrame);
    
    
    
    return result;
}

OSStatus Spotykach::GetPropertyInfo (AudioUnitPropertyID	inID,
                                     AudioUnitScope         inScope,
                                     AudioUnitElement       inElement,
                                     UInt32                 &outDataSize,
                                     bool                &outWritable) {
	if (inScope == kAudioUnitScope_Global) {
		switch (inID) {
			case kAudioUnitProperty_CocoaUI : {
				outWritable = false;
				outDataSize = sizeof (AudioUnitCocoaViewInfo);
				return noErr;
            }
		}
	}
	return Base::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

OSStatus Spotykach::GetProperty(AudioUnitPropertyID inID,
								AudioUnitScope 		inScope,
								AudioUnitElement 	inElement,
								void *				outData) {
    
	if (inScope == kAudioUnitScope_Global) {
		switch (inID) {
			case kAudioUnitProperty_CocoaUI : {
                CFBundleRef bundle = CFBundleGetBundleWithIdentifier( CFSTR("com.vlly.spotykach.au") );
				
				if (bundle == NULL) return fnfErr;
                
				CFURLRef bundleURL = CFBundleCopyResourceURL(bundle, CFSTR("spotykach_au_view"), CFSTR("bundle"), NULL);
                
                if (bundleURL == NULL) return fnfErr;
                
				AudioUnitCocoaViewInfo cocoaInfo;
				cocoaInfo.mCocoaAUViewBundleLocation = bundleURL;
				cocoaInfo.mCocoaAUViewClass[0] = CFStringCreateWithCString(NULL, "SpotykachViewFactory", kCFStringEncodingUTF8);
				
				*((AudioUnitCocoaViewInfo *)outData) = cocoaInfo;
				
				return noErr;
			}
		}
	}
    
    return Base::GetProperty (inID, inScope, inElement, outData);
}

void Spotykach::PassThrough(const AudioBufferList &inBuffer, AudioBufferList &outBuffer) {
    memcpy(outBuffer.mBuffers[0].mData, inBuffer.mBuffers[0].mData, inBuffer.mBuffers[0].mDataByteSize);
    memcpy(outBuffer.mBuffers[1].mData, inBuffer.mBuffers[1].mData, inBuffer.mBuffers[1].mDataByteSize);
};

OSStatus Spotykach::ProcessBufferLists(AudioUnitRenderActionFlags  &ioActionFlags,
                                       const AudioBufferList       &inBuffer,
                                       AudioBufferList             &outBuffer,
                                       UInt32                      inFramesToProcess ) {
    
    Boolean nowPlaying;
    float numerator;
    unsigned denominator;
    double currentBeat;
    double tempo;
    
    OSStatus err;
    
    err = AUBase::CallHostTransportState(&nowPlaying, NULL, NULL, NULL, NULL, NULL);
    if (err || false == nowPlaying) {
        this->PassThrough(inBuffer, outBuffer);
        return noErr;
    }
    
    err = AUBase::CallHostMusicalTimeLocation(NULL, &numerator, &denominator, NULL);
    if (err) {
        this->PassThrough(inBuffer, outBuffer);
        return noErr;
    }
    
    err = AUBase::CallHostBeatAndTempo(&currentBeat, &tempo);
    if (err) {
        this->PassThrough(inBuffer, outBuffer);
        return noErr;
    }
    
    if (tempo != _tempo) {
        _tempo = tempo;
        _trigger->measure(tempo, numerator, denominator, GetSampleRate());
        _source->adjustBufferSize(_trigger->getFramesPerMeasure());
    }
    
    float step = this->GetParameter(kParamID_Slice_Step);
    float slice = this->GetParameter(kParamID_Slice_Length);
    bool sync = static_cast<bool>(this->GetParameter(kParamID_Sync));
    if (step != _step || slice != _slice || sync != _sync) {
        _step = step;
        _slice = slice;
        _sync = sync;
        _trigger->schedule(step, slice, sync);
    }
    _trigger->setRetrigger(this->GetParameter(kParamID_Retrigger));
    _trigger->setRepeatsTillRetrigger(this->GetParameter(kParamID_RetriggerAfter));

    if (!_trigger->isRunning()) {
        _trigger->run(currentBeat);
    }
    _trigger->scheduleSync(currentBeat);

    _envelope->setDeclick(GetParameter(kParamID_Declick));
    _envelope->setFramesPerCrossfade(_trigger->getFramesPerCrossFade());
    
    _generator->setReverse(this->GetParameter(kParamID_Reverse));
    _generator->setBackAndForth(this->GetParameter(kParamID_BackNForth));
    
    double mix = this->GetParameter(kParamID_Mix);
    //double mixmode = this->GetParameter(kParamID_Mode);
    
    float *in0  = static_cast<float *>(inBuffer.mBuffers[0].mData);
    float *in1;
    if (inBuffer.mNumberBuffers > 1) {
        in1 = static_cast<float *>(inBuffer.mBuffers[1].mData);
    }
    
    float *out0 = static_cast<float *>(outBuffer.mBuffers[0].mData);
    float *out1;
    if (outBuffer.mNumberBuffers > 1) {
        out1 = static_cast<float *>(outBuffer.mBuffers[1].mData);
    }
    
    for (int currentFrame = 0; currentFrame < inFramesToProcess; currentFrame++) {

        _trigger->next();

        if (!_source->bufferFilled()) {
            _source->write(in0[currentFrame], in1[currentFrame]);
        }
        
        _generator->generate();
    
        out0[currentFrame] = _generator->out0() * mix + in0[currentFrame] * (1 - mix);
        if (outBuffer.mNumberBuffers > 1) {
            out1[currentFrame] = _generator->out1() * mix + in1[currentFrame] * (1 - mix);
        }
        
    }
    
    return noErr;
};
