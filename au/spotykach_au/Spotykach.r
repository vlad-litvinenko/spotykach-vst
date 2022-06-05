#include <AudioUnit/AudioUnit.r>
#include "SpotykachVersion.h"

#define RES_ID              1000
#define COMP_TYPE           'aumf'
#define COMP_SUBTYPE        'btrp'
#define COMP_MANUF          'Vlly'

#define VERSION             kSpotykachVersion
#define NAME                "Vlly: Spotykach"
#define DESCRIPTION         "Spotykach AU"
#define ENTRY_POINT         "SpotykachEntry"

#include "AUResources.r"
