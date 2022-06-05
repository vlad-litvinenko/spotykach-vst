//
//  vstparams.h
//  spotykach_vst
//
//  Created by Vladyslav Lytvynenko on 06.04.22.
//

#include "pluginterfaces/vst/vsttypes.h"

#ifndef vstparams_h
#define vstparams_h

namespace vlly {
namespace spotykach {
    enum class SpotykachVSTParam {
        kPatternLength  = 71,
        kBypass         = 72
    };

    static inline constexpr Steinberg::Vst::ParamID pid(spotykach::SpotykachVSTParam p) {
        return static_cast<Steinberg::Vst::ParamID>(p); }
    };
};

#endif /* vstparams_h */
