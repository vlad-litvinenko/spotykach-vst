//------------------------------------------------------------------------
// Copyright(c) 2022 vlly.
//------------------------------------------------------------------------

#include "processor.h"
#include "cids.h"
#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include "public.sdk/source/vst/utility/audiobuffers.h"
#include "Parameters.h"
#include "Engine.h"
#include "vstparams.h"
#include <cmath>
#include <algorithm>
#include <bitset>

using namespace Steinberg;
using namespace Vst;

namespace vlly {

//------------------------------------------------------------------------
// SpotykachProcessor
//------------------------------------------------------------------------
SpotykachProcessor::SpotykachProcessor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kSpotykachControllerUID);
}

//------------------------------------------------------------------------
SpotykachProcessor::~SpotykachProcessor ()
{}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachProcessor::initialize (FUnknown* context)
{
	// Here the Plug-in will be instanciated
	
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	// if everything Ok, continue
	if (result != kResultOk) {
		return result;
	}

	//--- create Audio IO ------
    
    createAudioIO();
    
    _core = new spotykach::Spotykach();
    _bypass = 0;
    
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachProcessor::terminate ()
{
	// Here the Plug-in will be de-instanciated, last possibility to remove some memory!
	
	//---do not forget to call parent ------
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachProcessor::setActive (TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive (state);
}

void SpotykachProcessor::createAudioIO() {
    auto stereo = Steinberg::Vst::SpeakerArr::kStereo;
    addAudioInput (STR16 ("Stereo In"), stereo);
    addAudioOutput (STR16 ("Main/One"), stereo);
    addAudioOutput (STR16 ("Two"), stereo);
    addAudioOutput (STR16 ("Three"), stereo);
    addAudioOutput (STR16 ("Four"), stereo);
}

tresult PLUGIN_API SpotykachProcessor::setBusArrangements (SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
    if (numIns == 1 && numOuts == 5 && inputs[0] == outputs[0]) {
        removeAudioBusses();
        createAudioIO();
        return AudioEffect::setBusArrangements (inputs, numIns, outputs, numOuts);
    }
    return kResultFalse;
}

tresult PLUGIN_API SpotykachProcessor::process (ProcessData& data)
{
    if (data.inputParameterChanges) {
        readParameters(*(data.inputParameterChanges));
	}
    
    if (data.numInputs == 0 || data.numOutputs == 0) {
        return kResultOk;
    }
    
    auto inputs = data.inputs;
    auto outputs = data.outputs;
    int32 numInChannels = inputs[0].numChannels;
    int32 numOutChannels = outputs[0].numChannels;
    
    uint32 sampleFramesSize = getSampleFramesSizeInBytes(processSetup, data.numSamples);
    void** in = getChannelBuffersPointer(processSetup, inputs[0]);

    ProcessContext *context = data.processContext;
    
    bool isPlaying = false;
    
    if (context) {
        isPlaying = context->state & ProcessContext::kPlaying;
        vlly::spotykach::PlaybackParameters p;
        p.isPlaying = isPlaying;
        p.tempo = context->tempo;
        p.numerator = context->timeSigNumerator;
        p.denominator = context->timeSigDenominator;
        p.currentBeat = context->projectTimeMusic;
        p.tempo = context->tempo;
        p.sampleRate = context->sampleRate;
        p.bufferSize = data.numSamples;
        
        _core->preprocess(p);
        
        IParameterChanges* outParamChanges = data.outputParameterChanges;
        if (outParamChanges) writeParamteers(*outParamChanges);
    }
    
    if (_bypass || !context || !isPlaying) {
        void** out;
        for (int i = 0; i < data.numOutputs; i++) {
            out = getChannelBuffersPointer(processSetup, outputs[i]);
            bool monoInput = numInChannels == 1;
            for (int32 i = 0; i < numOutChannels; i++) {
                if (in[monoInput ? 0 : i] != out[i]) {
                    memset(out[i], 0, sampleFramesSize);
                }
            }
        }
        return kResultOk;
    }
    
    for (int i = 0; i < data.numOutputs; i++) {
        data.outputs[i].silenceFlags = 0;
    }
    
    return doProcess(data);
}

tresult SpotykachProcessor::doProcess(ProcessData& data) {
    auto inBuf = getChannelBuffers<SymbolicSampleSizes::kSample32>(data.inputs[0]);
    bool inMono = data.inputs[0].numChannels == 1;
    bool outMono = data.outputs[0].numChannels == 1;
    
    Sample32** bufs[4] = {
        getChannelBuffers<SymbolicSampleSizes::kSample32>(data.outputs[0]),
        getChannelBuffers<SymbolicSampleSizes::kSample32>(data.outputs[1]),
        getChannelBuffers<SymbolicSampleSizes::kSample32>(data.outputs[2]),
        getChannelBuffers<SymbolicSampleSizes::kSample32>(data.outputs[3])
    };
    
    _core->process(inBuf, inMono, bufs, outMono, data.numSamples);
    
    return kResultOk;
}

void SpotykachProcessor::readParameters(IParameterChanges& ipc) {
    auto get = [](ParamValue& value, IParamValueQueue* queue, int32 index) {
        int32 numPoints = queue->getPointCount();
        ParamValue aValue;
        int32 sampleOffset;
        if (queue->getPoint(numPoints - 1, sampleOffset, aValue) == kResultTrue) {
            value = aValue;
        }
    };
    
    std::bitset<spotykach::kEnginesCount> changes { 0b0000 };
    int32 numParamsChanged = ipc.getParameterCount();
    for (int32 index = 0; index < numParamsChanged; index++) {
        if (IParamValueQueue* queue = ipc.getParameterData(index)) {
            std::pair<int, int> p = spotykach::decodePID(queue->getParameterId());
            int engineIndex = p.first / 100 - 1;
            int param = p.second;
            spotykach::Engine& e = _core->engineAt(engineIndex);
            switch (param) {
                case spotykach::kP_Shift: {
                    ParamValue aShift;
                    get(aShift, queue, index);
                    e.setShift(aShift);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_Slice_Step: {
                    ParamValue aStepPosition;
                    get(aStepPosition, queue, index);
                    e.setStepPosition(aStepPosition);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_Grid: {
                    ParamValue aGrid;
                    get(aGrid, queue, index);
                    e.setGrid(aGrid);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_Slice_Start: {
                    ParamValue aStart;
                    get(aStart, queue, index);
                    e.setSlicePosition(aStart);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_Slice_Length: {
                    ParamValue aSlice;
                    get(aSlice, queue, index);
                    e.setSliceLength(aSlice);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_Repeats: {
                    ParamValue aRepeats;
                    get(aRepeats, queue, index);
                    e.setRepeats(aRepeats);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_Retrigger: {
                    ParamValue aRetrigger;
                    get(aRetrigger, queue, index);
                    e.setRetrigger(aRetrigger);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_RetriggerChance: {
                    ParamValue aChance;
                    get(aChance, queue, index);
                    e.setRetriggerChance(aChance > 0.5 ? 0. : 1.);
                    break;
                }
                case spotykach::kP_Declick: {
                    ParamValue aDeclick;
                    get(aDeclick, queue, index);
                    e.setDeclick(aDeclick > 0.5);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_Direction: {
                    ParamValue aDirection;
                    get(aDirection, queue, index);
                    e.setDirection(aDirection * 0.5); //Currently only forward / reverse are supported
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_Level: {
                    ParamValue aLevel;
                    get(aLevel, queue, index);
                    _core->setVolume(aLevel, engineIndex);
                    break;
                }
                case spotykach::kP_On: {
                    ParamValue anOn;
                    get(anOn, queue, index);
                    e.setIsOn(anOn > 0.5);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_JitterAmount: {
                    ParamValue aJitterAmount;
                    get(aJitterAmount, queue, index);
                    e.setJitterAmount(aJitterAmount);
                    break;
                }
                case spotykach::kP_JitterRate: {
                    ParamValue aJitterRate;
                    get(aJitterRate, queue, index);
                    e.setJitterRate(aJitterRate);
                    break;
                }
                case spotykach::kP_Freeze: {
                    ParamValue aFreeze;
                    get(aFreeze, queue, index);
                    e.setFrozen(aFreeze > 0.5);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_OwnBus: {
                    ParamValue anOwnBus;
                    get(anOwnBus, queue, index);
                    _core->sendToOwnBus(anOwnBus, engineIndex);
                    break;
                }
                case spotykach::kP_Cascade: {
                    ParamValue aCascade;
                    get(aCascade, queue, index);
                    _core->setCascade(aCascade > 0.5, engineIndex);
                    changes.set(engineIndex);
                    break;
                }
                case spotykach::kP_Mix: {
                    ParamValue aMix;
                    get(aMix, queue, index);
                    _core->setMix(aMix);
                    break;
                }
                case spotykach::kP_Volume: {
                    ParamValue aVolume;
                    get(aVolume, queue, index);
                    _core->setMainVolume(aVolume);
                    break;
                }
                case pid(spotykach::SpotykachVSTParam::kBypass): {
                    ParamValue aBypass;
                    get(aBypass, queue, index);
                    _bypass = (aBypass > 0.5);
                    break;
                }
                case spotykach::kP_Mutex: {
                    ParamValue aMutex;
                    get(aMutex, queue, index);
                    _core->setMutex(int(round(aMutex * spotykach::kMutexCount)));
                    break;
                }
            }
            
            for (int i = 0; i < spotykach::kEnginesCount - 1; i++) {
                if (!changes[i]) continue;
                _core->resetCascadeOf(i);
            }
        }
    }
}

void SpotykachProcessor::writeParamteers(IParameterChanges& opc) {
    int32 dataIndex = 0;
    int32 pointIndex = 0;
    int unitID;
    int patternLengthPID = pid(spotykach::SpotykachVSTParam::kPatternLength);
    for (int i = 0; i < _core->enginesCount(); i++) {
        spotykach::Engine& e = _core->engineAt(i);
        int pointCount = e.pointsCount();
        if (pointCount != _pointsCount[i]) {
            _pointsCount[i] = pointCount;
            unitID = (i + 1) * 100;
            IParamValueQueue* paramQueue = opc.addParameterData(unitID + patternLengthPID, dataIndex);
            if (paramQueue) {
                paramQueue->addPoint(0, pointCount / 1000.0, pointIndex);
            }
            paramQueue = opc.addParameterData(unitID + spotykach::kP_Repeats, dataIndex);
            if (paramQueue) {
                auto val = static_cast<double>(e.repeats()) / pointCount;
                paramQueue->addPoint(0, val, pointIndex);
            }
        }
    }
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	//--- called before any processing ----
    _core->initialize(newSetup.sampleRate);
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachProcessor::setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);
    
    int32 version = 0;
    if (!streamer.readInt32(version)) return kResultFalse;
    
    bool bypass = false;
    if (!streamer.readBool(bypass)) return kResultFalse;
    _bypass = bypass;
    
    int32 mutex = 0;
    if (!streamer.readInt32(mutex)) return kResultFalse;
    _core->setMutex(mutex);
	
    double mix = 0;
    if (!streamer.readDouble(mix)) return kResultFalse;
    _core->setMix(mix);
    
    double volume = 0;
    if (!streamer.readDouble(volume)) return kResultFalse;
    _core->setMainVolume(volume);
    
    for (int i = 0; i < _core->enginesCount(); i++) {
        spotykach::Engine& e = _core->engineAt(i);
        double grid = 0;
        if (!streamer.readDouble(grid)) return kResultFalse;
        e.setGrid(grid);
        
        double shift = 0;
        if (!streamer.readDouble(shift)) return kResultFalse;
        e.setShift(shift);
        
        double step = 0;
        if (!streamer.readDouble(step)) return kResultFalse;
        e.setStepPosition(step);
        
        double start = 0;
        if (!streamer.readDouble(start)) return kResultFalse;
        e.setSlicePosition(start);
        
        double slice = 0;
        if (!streamer.readDouble(slice)) return kResultFalse;
        e.setSliceLength(slice);
        
        double direction = 0;
        if (!streamer.readDouble(direction)) return kResultFalse;
        e.setDirection(direction);
        
        double repeats = 0;
        if (!streamer.readDouble(repeats)) return kResultFalse;
        e.setRepeats(repeats);
        
        double retrigger = 0;
        if (!streamer.readDouble(retrigger)) return kResultFalse;
        e.setRetrigger(retrigger);
        
        double retriggerChance = 0;
        if (!streamer.readDouble(retriggerChance)) return kResultFalse;
        e.setRetriggerChance(retriggerChance);
        
        
        double jitterAmount = 0;
        if (!streamer.readDouble(jitterAmount)) return kResultFalse;
        e.setJitterAmount(jitterAmount);
        
        double jitterRate = 0;
        if (!streamer.readDouble(jitterRate)) return kResultFalse;
        e.setJitterRate(jitterRate);
        
        bool on = false;
        if (!streamer.readBool(on)) return kResultFalse;
        e.setIsOn(on);
        
        bool declick = false;
        if (!streamer.readBool(declick)) return kResultFalse;
        e.setDeclick(declick);
        
        bool frozen = false;
        if (!streamer.readBool(frozen)) return kResultFalse;
        e.setFrozen(frozen);
        
        double level = 0;
        if (!streamer.readDouble(level)) return kResultFalse;
        _core->setVolume(level, i);
        
        bool cascade = false;
        if (!streamer.readBool(cascade)) return kResultFalse;
        _core->setCascade(cascade, i);
        
        bool ownBus = 0;
        if (!streamer.readBool(ownBus)) return kResultFalse;
        _core->sendToOwnBus(ownBus, i);
    }
    
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachProcessor::getState (IBStream* state) {
	IBStreamer streamer (state, kLittleEndian);
    
    streamer.writeInt32(1); //version
    streamer.writeBool(_bypass);
    
    auto cr = _core->rawParameters();
    streamer.writeInt32(cr.mutex);
    streamer.writeDouble(cr.mix);
    streamer.writeDouble(cr.mainVol);
    
    for (int i = 0; i < _core->enginesCount(); i++) {
        spotykach::Engine& e = _core->engineAt(i);
        auto r = e.rawParameters();
        streamer.writeDouble(r.grid);
        streamer.writeDouble(r.shift);
        streamer.writeDouble(r.stepGridPosition);
        streamer.writeDouble(r.slicePosition);
        streamer.writeDouble(r.sliceLength);
        streamer.writeDouble(r.direction);
        streamer.writeDouble(r.repeats);
        streamer.writeDouble(r.retrigger);
        streamer.writeDouble(r.retriggerChance);
        streamer.writeDouble(r.jitterAmount);
        streamer.writeDouble(r.jitterRate);
        streamer.writeBool(r.on);
        streamer.writeBool(r.declick);
        streamer.writeBool(r.frozen);
        streamer.writeDouble(cr.vol[i]);
        streamer.writeBool(cr.cascade[i]);
        streamer.writeBool(cr.ownBus[i]);
    }
    
	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace vlly
