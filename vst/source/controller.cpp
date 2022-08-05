//------------------------------------------------------------------------
// Copyright(c) 2022 vlly.
//------------------------------------------------------------------------

#include "controller.h"
#include "cids.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "base/source/fstreamer.h"
#include "Parameters.h"
#include "vstparams.h"
#include <algorithm>
#include "pluginterfaces/base/ustring.h"
#include <string>

using namespace Steinberg;
using namespace Vst;
using namespace vlly;
using namespace spotykach;

namespace vlly {

//------------------------------------------------------------------------
// SpotykachController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachController::initialize (FUnknown* context)
{
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk) {
		return result;
	}
    
    Unit* unit;
    UnitInfo unitInfo;
    auto stepCount = 1;
    auto defaultValue = 0;
    auto paramPID = 0;
    auto f = ParameterInfo::kCanAutomate;
    for (auto u : { u1, u2, u3, u4 }) {
        unitInfo.id = u;
        unitInfo.parentUnitId = kRootUnitId;
        Steinberg::UString(unitInfo.name, USTRINGSIZE(unitInfo.name))
            .assign(USTRING (std::string("Unit").append(std::to_string(u)).data()));
        unitInfo.programListId = kNoProgramListId;
        
        unit = new Unit(unitInfo);
        addUnit(unit);
        
        //In
        parameters.addParameter(new RangeParameter(STR("Grid"), u + kP_Grid, nullptr, 0, 2, kGrid_Even, 2, f, u));
        parameters.addParameter(new RangeParameter(STR("Shift"), u + kP_Shift, nullptr, 0, 15, 0, 15, f, u));
        parameters.addParameter(new RangeParameter(STR("Step"), u + kP_Slice_Step, nullptr, 0, EvenStepsCount - 1, 6, EvenStepsCount - 1, f, u));
        parameters.addParameter(new RangeParameter(STR("Start"), u + kP_Slice_Start, nullptr, 0, 128, 0, 128, f, u));
        parameters.addParameter(new RangeParameter(STR("Slice"), u + kP_Slice_Length, nullptr, 0, 64, 32, 64, f, u));
        parameters.addParameter(new RangeParameter(STR("Repeats"), u + kP_Repeats, nullptr, 0, 8, 8, 8, f, u));
        parameters.addParameter(new RangeParameter(STR("Retrigger"), u + kP_Retrigger, nullptr, 0, 16, 0, 16, f, u));
        parameters.addParameter(new RangeParameter(STR("Direction"), u + kP_Direction, nullptr, 0, 2, 0, 2, f, u));
        parameters.addParameter(new RangeParameter(STR("Level"), u + kP_Level, nullptr, 0, 1, 0.75, 0, f, u));
        parameters.addParameter(new RangeParameter(STR("Position LFO Amp"), u + kP_JitterAmount, nullptr, 0, 1, 0, 0, f, u));
        parameters.addParameter(new RangeParameter(STR("Position LFO Rate"), u + kP_JitterRate, nullptr, 0, 1, 0.25, 0, f, u));
        
        stepCount = 1;
        defaultValue = 0;
        paramPID = u + kP_Declick;
        parameters.addParameter(STR("Declick"), nullptr, stepCount, defaultValue, f, paramPID, u);
        
        stepCount = 1;
        defaultValue = (u == u1) ? 1. : 0.;
        paramPID = u + kP_On;
        parameters.addParameter(STR("On"), nullptr, stepCount, defaultValue, f, paramPID, u);
        
        if (u != u1) {
            stepCount = 1;
            defaultValue = 0.;
            paramPID = u + kP_OwnBus;
            parameters.addParameter(STR("OwnBus"), nullptr, stepCount, defaultValue, f, paramPID, u);
        }
        
        stepCount = 1;
        defaultValue = 0.;
        paramPID = u + kP_Cascade;
        parameters.addParameter(STR("Cascade"), nullptr, stepCount, defaultValue, f, paramPID, u);
        
        stepCount = 1;
        defaultValue = 0;
        paramPID = u + kP_RetriggerChance;
        parameters.addParameter(STR("Retrigger chance"), nullptr, stepCount, defaultValue, f, paramPID, u);
        
        stepCount = 1;
        defaultValue = 0.;
        paramPID = u + kP_Freeze;
        parameters.addParameter(STR("Freeze"), nullptr, stepCount, defaultValue, f, paramPID, u);
        
        //Out
        parameters.addParameter(new RangeParameter(STR("PatternLength"), u + pid(SpotykachVSTParam::kPatternLength), nullptr, 0., 1000., 0., 0., ParameterInfo::kIsHidden | ParameterInfo::kIsReadOnly, u));
        
        //Dependent
        parameters.getParameter(u + pid(SpotykachVSTParam::kPatternLength))->addDependent(this);
        parameters.getParameter(u + kP_Grid)->addDependent(this);
    }
    
    parameters.addParameter(new Parameter(STR("Mix"), u1 + kP_Mix, nullptr, 1., 0, f, u1));
    
    stepCount = 2;
    defaultValue = 0;
    paramPID = kP_Mutex;
    parameters.addParameter(STR16("Mutex"), nullptr, stepCount, defaultValue, f, paramPID);
    
    stepCount = 0;
    defaultValue = 1.0;
    paramPID = kP_Volume;
    parameters.addParameter(new Parameter(STR("Volume"), paramPID, nullptr, 1., 0, f));
    
    stepCount = 1;
    defaultValue = 0;
    auto flags = ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass;
    paramPID = pid(spotykach::SpotykachVSTParam::kBypass);
    parameters.addParameter(STR16("Bypass"), nullptr, stepCount, defaultValue, flags, paramPID);
    
	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachController::terminate ()
{
    for (auto u : { u1, u2, u3, u4 }) {
        parameters.getParameter(u + kP_Slice_Step)->removeDependent(this);
        parameters.getParameter(u + kP_Grid)->removeDependent(this);
        parameters.getParameter(u + kP_Slice_Step)->removeDependent(this);
    }
    
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachController::setComponentState (IBStream* state)
{
    if (!state) return kResultFalse;

    IBStreamer streamer (state, kLittleEndian);

    int32 version = 0;
    if (!streamer.readInt32(version)) return kResultFalse;
    
    bool bypassState = false;
    if (!streamer.readBool(bypassState)) return kResultFalse;
    setParamNormalized(pid(spotykach::SpotykachVSTParam::kBypass), bypassState ? 1. : 0.);
    
    int32 mutex = 0;
    if (!streamer.readInt32(mutex)) return kResultFalse;
    setParamNormalized(kP_Mutex, 0.5 * mutex);
    
    double mix = 0;
    if (!streamer.readDouble(mix)) return kResultFalse;
    setParamNormalized(u1 + kP_Mix, mix);
    
    double volume = 0;
    if (!streamer.readDouble(volume)) return kResultFalse;
    setParamNormalized(kP_Volume, volume);
    
    for (auto u: { u1, u2, u3, u4 }) {
        double grid = 0;
        if (!streamer.readDouble(grid)) return kResultFalse;
        setParamNormalized(u + kP_Grid, grid);
        
        double shift = 0;
        if (!streamer.readDouble(shift)) return kResultFalse;
        setParamNormalized(u + kP_Shift, shift);
        
        double step = 0;
        if (!streamer.readDouble(step)) return kResultFalse;
        setParamNormalized(u + kP_Slice_Step, step);
        
        double start = 0;
        if (!streamer.readDouble(start)) return kResultFalse;
        setParamNormalized(u + kP_Slice_Start, start);
        
        double slice = 0;
        if (!streamer.readDouble(slice)) return kResultFalse;
        setParamNormalized(u + kP_Slice_Length, slice);
        
        double direction = 0;
        if (!streamer.readDouble(direction)) return kResultFalse;
        setParamNormalized(u + kP_Direction, direction);
        
        double repeats = 0;
        if (!streamer.readDouble(repeats)) return kResultFalse;
        setParamNormalized(u + kP_Repeats, repeats);
        
        double retrigger = 0;
        if (!streamer.readDouble(retrigger)) return kResultFalse;
        setParamNormalized(u + kP_Retrigger, retrigger);
        
        double retriggerChance = 0;
        if (!streamer.readDouble(retriggerChance)) return kResultFalse;
        setParamNormalized(u + kP_RetriggerChance, retriggerChance ? 0. : 1.);
        
        double jitterAmount = 0;
        if (!streamer.readDouble(jitterAmount)) return kResultFalse;
        setParamNormalized(u + kP_JitterAmount, jitterAmount);
        
        double jitterRate = 0;
        if (!streamer.readDouble(jitterRate)) return kResultFalse;
        setParamNormalized(u + kP_JitterRate, jitterRate);
        
        bool on = false;
        if (!streamer.readBool(on)) return kResultFalse;
        setParamNormalized(u + kP_On, on ? 1. : 0.);
        
        bool declick = false;
        if (!streamer.readBool(declick)) return kResultFalse;
        setParamNormalized(u + kP_Declick, declick ? 1. : 0.);
        
        bool frozen = false;
        if (!streamer.readBool(frozen)) return kResultFalse;
        setParamNormalized(u + kP_Freeze, frozen ? 1. : 0.);
        
        double level = 0;
        if (!streamer.readDouble(level)) return kResultFalse;
        setParamNormalized(u + kP_Level, level);
        
        bool cascade = false;
        if (!streamer.readBool(cascade)) return kResultFalse;
        setParamNormalized(u + kP_Cascade, cascade ? 1. : 0.);
        
        bool ownBus = false;
        if (!streamer.readBool(ownBus)) return kResultFalse;
        setParamNormalized(u + kP_OwnBus, ownBus ? 1. : 0.);
    }
    
    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachController::setState (IBStream* state)
{
    IBStreamer streamer (state, kLittleEndian);
    for (auto u: { u1, u2, u3, u4 }) {
        
        int32 stepMax = 0;
        if (!streamer.readInt32(stepMax)) return kResultFalse;
        auto stepsParam = FCast<RangeParameter>(parameters.getParameter(u + kP_Slice_Step));
        stepsParam->setMax(stepMax);
        stepsParam->getInfo().stepCount = stepMax;
        
        int32 patternLength = 0;
        if (!streamer.readInt32(patternLength)) return kResultFalse;
        auto repeatsParam = FCast<RangeParameter>(parameters.getParameter(u + kP_Repeats));
        repeatsParam->setMax(patternLength);
        repeatsParam->getInfo().stepCount = patternLength;
        
        auto shiftParam = FCast<RangeParameter>(parameters.getParameter(u + kP_Shift));
        shiftParam->setMax(patternLength);
        shiftParam->getInfo().stepCount = patternLength;
    }

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachController::getState (IBStream* state)
{
    IBStreamer streamer (state, kLittleEndian);
    
    for (auto u: { u1, u2, u3, u4 }) {
        auto stepsParam = FCast<RangeParameter>(parameters.getParameter(u + kP_Slice_Step));
        streamer.writeInt32(stepsParam->getMax());
        
        auto repeatsParam = FCast<RangeParameter>(parameters.getParameter(u + kP_Repeats));
        streamer.writeInt32(repeatsParam->getMax());
    }

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API SpotykachController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VST3Editor (this, "view", "editor.uidesc");
		return view;
	}
	return nullptr;
}

VSTGUI::CView* SpotykachController::createCustomView (VSTGUI::UTF8StringPtr name, const VSTGUI::UIAttributes& attributes, const VSTGUI::IUIDescription* description, VSTGUI::VST3Editor* editor) {
    return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachController::setParamNormalized (Vst::ParamID tag, Vst::ParamValue value)
{
	tresult result = EditControllerEx1::setParamNormalized (tag, value);
	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachController::getParamStringByValue (Vst::ParamID tag, Vst::ParamValue valueNormalized, Vst::String128 string)
{
    auto paramID = decodePID(tag);
    auto unit = paramID.first;
    switch (paramID.second) {
        case kP_Slice_Step: {
            auto stepParam = parameters.getParameter(tag);
            auto index = stepParam->toPlain(stepParam->getNormalized());
            auto gridParam = parameters.getParameter(unit + kP_Grid);
            auto grid = static_cast<Grid>(gridParam->toPlain(gridParam->getNormalized()));
            const char* title;
            switch (grid) {
                case kGrid_Even: title = EvenSteps[index].title; break;
                case kGrid_Odd: title = OddSteps[index].title; break;
                case kGrid_CWord: title = CWords[index].title; break;
            }
            Steinberg::UString(string, 128).fromAscii(title);
            return kResultTrue;
        }
    }
    
	return EditControllerEx1::getParamStringByValue (tag, valueNormalized, string);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SpotykachController::getParamValueByString (Vst::ParamID tag, Vst::TChar* string, Vst::ParamValue& valueNormalized)
{
	return EditControllerEx1::getParamValueByString (tag, string, valueNormalized);
}

void PLUGIN_API SpotykachController::update(FUnknown *changedUnknown, int32 message) {
    if (message != kChanged) return;

    auto param = FCast<Parameter>(changedUnknown);
    if (!param) return;
    auto paramID = spotykach::decodePID(param->getInfo().id);
    auto unit = spotykach::SUnit(paramID.first);
    if (paramID.second == pid(SpotykachVSTParam::kPatternLength)) {
        auto repeatsParam = FCast<RangeParameter>(parameters.getParameter(unit + kP_Repeats));
        if (!repeatsParam) return;
                
        ParamValue newPatternLength = param->toPlain(param->getNormalized());
        repeatsParam->setMax(newPatternLength);
        repeatsParam->getInfo().stepCount = newPatternLength;
        return;
    }
    
    Grid* grids[] = { &_u1_grid, &_u2_grid, &_u3_grid, &_u4_grid };
    Grid& grid = *(grids[unit / 100 - 1]);
    
    if (paramID.second == kP_Grid) {
        auto stepParam = FCast<RangeParameter>(parameters.getParameter(unit + kP_Slice_Step));
        if (!stepParam) return;
        
        grid = spotykach::Grid(param->toPlain(param->getNormalized()));
        
        int newMax = 0;
        int newStepsCount = 0;
        switch (grid) {
            case spotykach::kGrid_Even:
                newMax = newStepsCount = spotykach::EvenStepsCount - 1;
                break;
            
            case spotykach::kGrid_Odd:
                newMax = newStepsCount = spotykach::OddStepsCount - 1;
                break;
                
            case spotykach::kGrid_CWord:
                newMax = newStepsCount = spotykach::CWordsCount - 1;
                break;
        }
        
        stepParam->setMax(newMax);
        stepParam->getInfo().stepCount = newStepsCount;
    }
}
//------------------------------------------------------------------------
} // namespace vlly
