//------------------------------------------------------------------------
// Copyright(c) 2022 vlly.
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "vstparams.h"
#include "Parameters.h"

namespace vlly {

//------------------------------------------------------------------------
//  SpotykachController
//------------------------------------------------------------------------
class SpotykachController : public Steinberg::Vst::EditControllerEx1,
                            public VSTGUI::VST3EditorDelegate
{
public:
//------------------------------------------------------------------------
//	SpotykachController () = default;
//	~SpotykachController () SMTG_OVERRIDE = default;
    // Create function
	static Steinberg::FUnknown* createInstance (void* /*context*/)
	{
		return (Steinberg::Vst::IEditController*) new SpotykachController;
	}

    using CView = VSTGUI::CView;
    using UTF8StringPtr = VSTGUI::UTF8StringPtr;
    using IUIDescription = VSTGUI::IUIDescription;
    using UIAttributes = VSTGUI::UIAttributes;
    using VST3Editor = VSTGUI::VST3Editor;
    
	// IPluginBase
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;
    
	// EditController
	Steinberg::tresult PLUGIN_API setComponentState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setParamNormalized (Steinberg::Vst::ParamID tag,
                                                      Steinberg::Vst::ParamValue value) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getParamStringByValue (Steinberg::Vst::ParamID tag,
                                                         Steinberg::Vst::ParamValue valueNormalized,
                                                         Steinberg::Vst::String128 string) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getParamValueByString (Steinberg::Vst::ParamID tag,
                                                         Steinberg::Vst::TChar* string,
                                                         Steinberg::Vst::ParamValue& valueNormalized) SMTG_OVERRIDE;
    
    void PLUGIN_API update(FUnknown *changedUnknown, Steinberg::int32 message) SMTG_OVERRIDE;

    CView* createCustomView (UTF8StringPtr name, const UIAttributes& attributes,
                             const IUIDescription* description, VST3Editor* editor) SMTG_OVERRIDE;
    
    
 	//---Interface---------
	DEFINE_INTERFACES
		// Here you can add more supported VST3 interfaces
		// DEF_INTERFACE (Vst::IXXX)
        DEF_INTERFACE(IUnitInfo)
	END_DEFINE_INTERFACES (EditController)
    DELEGATE_REFCOUNT (EditController)
    
    
private:
    spotykach::Grid _u1_grid = spotykach::kGrid_Even;
    spotykach::Grid _u2_grid = spotykach::kGrid_Even;
    spotykach::Grid _u3_grid = spotykach::kGrid_Even;
    spotykach::Grid _u4_grid = spotykach::kGrid_Even;
    Steinberg::Vst::ParamID _dirtyTag { 0 };
};

//------------------------------------------------------------------------
} // namespace vlly
