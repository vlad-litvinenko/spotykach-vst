//------------------------------------------------------------------------
// Copyright(c) 2022 vlly.
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "Spotykach.h"
#include "Parameters.h"
#include "vstparams.h"

using namespace Steinberg;
using namespace Vst;

namespace vlly {


//------------------------------------------------------------------------
//  SpotykachProcessor
//------------------------------------------------------------------------
class SpotykachProcessor : public Steinberg::Vst::AudioEffect
{
public:
	SpotykachProcessor ();
	~SpotykachProcessor () SMTG_OVERRIDE;

    // Create function
	static FUnknown* createInstance (void* /*context*/)
	{ 
		return (IAudioProcessor*)new SpotykachProcessor;
	}

	//--- ---------------------------------------------------------------------
	// AudioEffect overrides:
	//--- ---------------------------------------------------------------------
	/** Called at first after constructor */
	tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	
	/** Called at the end before destructor */
	tresult PLUGIN_API terminate () SMTG_OVERRIDE;
	
    tresult PLUGIN_API setBusArrangements (SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;
    
	/** Switch the Plug-in on/off */
	tresult PLUGIN_API setActive (TBool state) SMTG_OVERRIDE;

	/** Will be called before any process call */
	tresult PLUGIN_API setupProcessing (ProcessSetup& newSetup) SMTG_OVERRIDE;
	
	/** Asks if a given sample size is supported see SymbolicSampleSizes. */
	tresult PLUGIN_API canProcessSampleSize (int32 symbolicSampleSize) SMTG_OVERRIDE;

	/** Here we go...the process call */
	tresult PLUGIN_API process (ProcessData& data) SMTG_OVERRIDE;
		
	/** For persistence */
	tresult PLUGIN_API setState (IBStream* state) SMTG_OVERRIDE;
	tresult PLUGIN_API getState (IBStream* state) SMTG_OVERRIDE;

//------------------------------------------------------------------------
private:
    spotykach::Spotykach* _core;
    int _pointsCount[4] = { 0, 0, 0, 0 };
    
    bool _bypass;
    
    void createAudioIO();
    void readParameters(IParameterChanges& ipc);
    void writeParamteers(IParameterChanges& opc);
    tresult doProcess(ProcessData& data);
};

//------------------------------------------------------------------------
} // namespace vlly
