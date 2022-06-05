cmake_minimum_required(VERSION 3.14.0)

set(vst3sdk_SOURCE_DIR /Users/vlly/DSP/vst3sdk)
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(spotykach_vst)
  
set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(spotykach_vst     
    source/version.h
    source/cids.h
    source/processor.h
    source/processor.cpp
    source/controller.h
    source/controller.cpp
    source/entry.cpp
)

#- VSTGUI Wanted ----
if(SMTG_ADD_VSTGUI)
    target_sources(spotykach_vst
        PRIVATE
            resource/editor.uidesc
    )
    target_link_libraries(spotykach_vst
        PRIVATE
            vstgui_support
    )
    smtg_add_vst3_resource(spotykach_vst "resource/editor.uidesc")
endif()
# -------------------

smtg_add_vst3_snapshot(spotykach_vst resource/CA6B6D35BB785CA7989D6777E35465F9_snapshot.png)
smtg_add_vst3_snapshot(spotykach_vst resource/CA6B6D35BB785CA7989D6777E35465F9_snapshot_2.0x.png)

target_link_libraries(spotykach_vst
    PRIVATE
        sdk
)

if(SMTG_MAC)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)
    smtg_set_bundle(spotykach_vst
		BUNDLE_IDENTIFIER com.vlly.spotykach.vst
		INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS
    )
    smtg_set_debug_executable(spotykach_vst
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(spotykach_vst PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT spotykach_vst)
    
        smtg_set_debug_executable(spotykach_vst
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif()
