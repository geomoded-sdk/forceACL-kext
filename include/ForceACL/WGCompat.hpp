/**
 * ForceACL - WhateverGreen Compatibility Layer
 * Adapts key WhateverGreen patches for native ForceACL implementation
 */

#ifndef FORCEACL_WGCOMPAT_HPP
#define FORCEACL_WGCOMPAT_HPP

#include <stdint.h>

// ============================================================================
// FRAMEBUFFER FLAGS (from WhateverGreen kern_fb.hpp)
// ============================================================================

union WGFBFlags {
    struct FlagBits {
        uint8_t FBAvoidFastLinkTraining    :1;
        uint8_t FBUnknownFlag_2             :1;
        uint8_t FBFramebufferCompression  :1;
        uint8_t FBEnableSliceFeatures       :1;
        uint8_t FBDynamicFBCEnable          :1;
        uint8_t FBUseVideoTurbo             :1;
        uint8_t FBForcePowerAlwaysConnected :1;
        uint8_t FBDisableHighBitrateMode2  :1;
        uint8_t FBBoostPixelFrequencyLimit  :1;
        uint8_t FBLimit4KSourceSize         :1;
        uint8_t FBAlternatePWMIncrement1   :1;
        uint8_t FBAlternatePWMIncrement2    :1;
        uint8_t FBDisableFeatureIPS         :1;
        uint8_t FBUnknownFlag_2000          :1;
        uint8_t FBAllowConnectorRecover     :1;
        uint8_t FBUnknownFlag_8000          :1;
        uint8_t FBUnknownFlag_10000        :1;
        uint8_t FBUnknownFlag_20000        :1;
        uint8_t FBDisableGFMPPFM            :1;
        uint8_t FBUnknownFlag_80000        :1;
        uint8_t FBUnknownFlag_100000      :1;
        uint8_t FBEnableDynamicCDCLK       :1;
        uint8_t FBUnknownFlag_400000       :1;
        uint8_t FBSupport5KSourceSize       :1;
    } bits;
    uint32_t value;
};

// ============================================================================
// CONNECTOR TYPES (from WhateverGreen kern_fb.hpp)
// ============================================================================

enum WGConnectorType : uint32_t {
    WGConnectorZero        = 0x0,
    WGConnectorDummy       = 0x1,
    WGConnectorLVDS         = 0x2,
    WGConnectorDigitalDVI  = 0x4,
    WGConnectorSVID         = 0x8,
    WGConnectorVGA          = 0x10,
    WGConnectorDP           = 0x400,
    WGConnectorHDMI         = 0x800,
    WGConnectorAnalogDVI    = 0x2000
};

// ============================================================================
// CONNECTOR FLAGS (from WhateverGreen kern_fb.hpp)
// ============================================================================

union WGConnectorFlags {
    struct ConnFlagBits {
        uint8_t CNAlterAppertureRequirements :1;
        uint8_t CNUnknownFlag_2              :1;
        uint8_t CNUnknownFlag_4              :1;
        uint8_t CNConnectorAlwaysConnected    :1;
        uint8_t CNUnknownFlag_10            :1;
        uint8_t CNUnknownFlag_20            :1;
        uint8_t CNDisableBlitTranslationTable :1;
        uint8_t CNUseMiscIoPowerWell        :1;
        uint8_t CNUsePowerWell2             :1;
        uint8_t CNUnknownFlag_200           :1;
        uint8_t CNUnknownFlag_400           :1;
        uint8_t CNIncreaseLaneCount         :1;
        uint8_t CNUnknownFlag_1000          :1;
        uint8_t CNUnknownFlag_2000          :1;
        uint8_t CNUnknownFlag_4000          :1;
        uint8_t CNUnknownFlag_8000          :1;
    } bits;
    uint16_t value;
};

// ============================================================================
// INTEL GPU KEXTS (from WhateverGreen kern_igfx_kexts.cpp)
// ============================================================================

struct IntelKextInfo {
    const char* bundleID;
    const char* path;
};

// Known Intel GPU framebuffer kexts
static const IntelKextInfo wgIntelKexts[] = {
    {"com.apple.driver.AppleIntelHDGraphics", "/System/Library/Extensions/AppleIntelHDGraphics.kext/Contents/MacOS/AppleIntelHDGraphics"},
    {"com.apple.driver.AppleIntelHDGraphicsFB", "/System/Library/Extensions/AppleIntelHDGraphicsFB.kext/Contents/MacOS/AppleIntelHDGraphicsFB"},
    {"com.apple.driver.AppleIntelHD3000Graphics", "/System/Library/Extensions/AppleIntelHD3000Graphics.kext/Contents/MacOS/AppleIntelHD3000Graphics"},
    {"com.apple.driver.AppleIntelSNBGraphicsFB", "/System/Library/Extensions/AppleIntelSNBGraphicsFB.kext/Contents/MacOS/AppleIntelSNBGraphicsFB"},
    {"com.apple.driver.AppleIntelHD4000Graphics", "/System/Library/Extensions/AppleIntelHD4000Graphics.kext/Contents/MacOS/AppleIntelHD4000Graphics"},
    {"com.apple.driver.AppleIntelFramebufferCapri", "/System/Library/Extensions/AppleIntelFramebufferCapri.kext/Contents/MacOS/AppleIntelFramebufferCapri"},
    {"com.apple.driver.AppleIntelHD5000Graphics", "/System/Library/Extensions/AppleIntelHD5000Graphics.kext/Contents/MacOS/AppleIntelHD5000Graphics"},
    {"com.apple.driver.AppleIntelFramebufferAzul", "/System/Library/Extensions/AppleIntelFramebufferAzul.kext/Contents/MacOS/AppleIntelFramebufferAzul"},
    {"com.apple.driver.AppleIntelBDWGraphics", "/System/Library/Extensions/AppleIntelBDWGraphics.kext/Contents/MacOS/AppleIntelBDWGraphics"},
    {"com.apple.driver.AppleIntelBDWGraphicsFramebuffer", "/System/Library/Extensions/AppleIntelBDWGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelBDWGraphicsFramebuffer"},
    {"com.apple.driver.AppleIntelSKLGraphics", "/System/Library/Extensions/AppleIntelSKLGraphics.kext/Contents/MacOS/AppleIntelSKLGraphics"},
    {"com.apple.driver.AppleIntelSKLGraphicsFramebuffer", "/System/Library/Extensions/AppleIntelSKLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelSKLGraphicsFramebuffer"},
    {"com.apple.driver.AppleIntelKBLGraphics", "/System/Library/Extensions/AppleIntelKBLGraphics.kext/Contents/MacOS/AppleIntelKBLGraphics"},
    {"com.apple.driver.AppleIntelKBLGraphicsFramebuffer", "/System/Library/Extensions/AppleIntelKBLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelKBLGraphicsFramebuffer"},
    {"com.apple.driver.AppleIntelCFLGraphicsFramebuffer", "/System/Library/Extensions/AppleIntelCFLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelCFLGraphicsFramebuffer"},
    {"com.apple.driver.AppleIntelCNLGraphics", "/System/Library/Extensions/AppleIntelCNLGraphics.kext/Contents/MacOS/AppleIntelCNLGraphics"},
    {"com.apple.driver.AppleIntelCNLGraphicsFramebuffer", "/System/Library/Extensions/AppleIntelCNLGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelCNLGraphicsFramebuffer"},
    {"com.apple.driver.AppleIntelICLGraphics", "/System/Library/Extensions/AppleIntelICLGraphics.kext/Contents/MacOS/AppleIntelICLGraphics"},
    {"com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", "/System/Library/Extensions/AppleIntelICLLPGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelICLLPGraphicsFramebuffer"},
    {"com.apple.driver.AppleIntelICLHPGraphicsFramebuffer", "/System/Library/Extensions/AppleIntelICLHPGraphicsFramebuffer.kext/Contents/MacOS/AppleIntelICLHPGraphicsFramebuffer"},
};

// ============================================================================
// PLATFORM DATABASE (adapted from WhateverGreen knowledge base)
// ============================================================================

struct WGPlatformEntry {
    uint32_t platformId;
    uint16_t deviceId;
    const char* generation;
    const char* kextBundleId;
    const char* fbKextBundleId;
    uint32_t defaultFBMem;
    uint32_t defaultStolenMem;
    uint8_t defaultPorts;
    uint8_t recommendedFlags;
};

// Platform entries for each generation
static const WGPlatformEntry wgPlatformDatabase[] = {
    // Sandy Bridge
    {0x01020003, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x01020004, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x01020005, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x01020006, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x01020007, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x01020008, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x01020009, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x0102000A, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x0102000B, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x0102000C, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x0102000D, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x0102000E, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    {0x0102000F, 0x0102, "SandyBridge", "com.apple.driver.AppleIntelHD3000Graphics", "com.apple.driver.AppleIntelSNBGraphicsFB", 0x00300000, 0x00100000, 2, 0x01},
    
    // Ivy Bridge
    {0x01620003, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x01620004, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x01620005, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x01620006, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x01620007, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x01620008, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x01620009, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x0162000A, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x0162000B, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x0162000C, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x0162000D, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x0162000E, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    {0x0162000F, 0x0162, "IvyBridge", "com.apple.driver.AppleIntelHD4000Graphics", "com.apple.driver.AppleIntelFramebufferCapri", 0x00400000, 0x00100000, 2, 0x01},
    
    // Haswell
    {0x0D220003, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220004, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220005, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220006, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220007, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220008, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220009, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22000A, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22000B, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22000C, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22000D, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22000E, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22000F, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220010, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220011, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220012, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220013, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220014, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220015, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220016, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220017, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220018, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220019, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22001A, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22001B, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22001C, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22001D, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22001E, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D22001F, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    {0x0D220020, 0x0412, "Haswell", "com.apple.driver.AppleIntelHD5000Graphics", "com.apple.driver.AppleIntelFramebufferAzul", 0x00600000, 0x00180000, 3, 0x00},
    
    // Broadwell
    {0x0D160003, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160005, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160006, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160007, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160008, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160009, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16000A, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16000B, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16000C, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16000D, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16000E, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16000F, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160010, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160011, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160012, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160013, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160014, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160015, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160016, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160017, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160018, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160019, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16001A, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16001B, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16001C, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16001D, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16001E, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D16001F, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    {0x0D160020, 0x1600, "Broadwell", "com.apple.driver.AppleIntelBDWGraphics", "com.apple.driver.AppleIntelBDWGraphicsFramebuffer", 0x00800000, 0x00180000, 3, 0x00},
    
    // Skylake
    {0x0A160003, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160004, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160005, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160006, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160007, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160008, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160009, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16000A, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16000B, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16000C, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16000D, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16000E, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16000F, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160010, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160011, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160012, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160013, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160014, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160015, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160016, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160017, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160018, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160019, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16001A, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16001B, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16001C, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16001D, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16001E, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A16001F, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    {0x0A160020, 0x1902, "Skylake", "com.apple.driver.AppleIntelSKLGraphics", "com.apple.driver.AppleIntelSKLGraphicsFramebuffer", 0x00A00000, 0x00190000, 3, 0x00},
    
    // Kaby Lake
    {0x0A5E0003, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E0004, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E0005, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E0006, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E0007, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E0008, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E0009, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E000A, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E000B, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E000C, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E000D, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E000E, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    {0x0A5E000F, 0x5912, "KabyLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelKBLGraphicsFramebuffer", 0x00C00000, 0x00190000, 3, 0x00},
    
    // Coffee Lake
    {0x3E9B0003, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0004, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0005, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0006, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0007, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0008, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0009, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B000A, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B000B, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B000C, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B000D, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B000E, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B000F, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0010, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0011, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0012, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0013, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0014, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0015, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0016, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0017, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0018, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0019, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B001A, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B001B, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B001C, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B001D, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B001E, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B001F, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    {0x3E9B0020, 0x3E9B, "CoffeeLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 3, 0x00},
    
    // Ice Lake
    {0x8A700003, 0x8A70, "IceLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", 0x02000000, 0x00280000, 4, 0x00},
    {0x8A700004, 0x8A70, "IceLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", 0x02000000, 0x00280000, 4, 0x00},
    {0x8A700005, 0x8A70, "IceLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", 0x02000000, 0x00280000, 4, 0x00},
    {0x8A700006, 0x8A70, "IceLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", 0x02000000, 0x00280000, 4, 0x00},
    {0x8A700007, 0x8A70, "IceLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", 0x02000000, 0x00280000, 4, 0x00},
    {0x8A700008, 0x8A70, "IceLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", 0x02000000, 0x00280000, 4, 0x00},
    {0x8A700009, 0x8A70, "IceLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", 0x02000000, 0x00280000, 4, 0x00},
    {0x8A70000A, 0x8A70, "IceLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", 0x02000000, 0x00280000, 4, 0x00},
    
    // Tiger Lake
    {0x9A600003, 0x9A60, "TigerLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLHPGraphicsFramebuffer", 0x03000000, 0x00300000, 4, 0x00},
    {0x9A600004, 0x9A60, "TigerLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLHPGraphicsFramebuffer", 0x03000000, 0x00300000, 4, 0x00},
    {0x9A600005, 0x9A60, "TigerLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLHPGraphicsFramebuffer", 0x03000000, 0x00300000, 4, 0x00},
    {0x9A600006, 0x9A60, "TigerLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLHPGraphicsFramebuffer", 0x03000000, 0x00300000, 4, 0x00},
    {0x9A600007, 0x9A60, "TigerLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLHPGraphicsFramebuffer", 0x03000000, 0x00300000, 4, 0x00},
    {0x9A600008, 0x9A60, "TigerLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLHPGraphicsFramebuffer", 0x03000000, 0x00300000, 4, 0x00},
    {0x9A600009, 0x9A60, "TigerLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLHPGraphicsFramebuffer", 0x03000000, 0x00300000, 4, 0x00},
    {0x9A60000A, 0x9A60, "TigerLake", "com.apple.driver.AppleIntelICLGraphics", "com.apple.driver.AppleIntelICLHPGraphicsFramebuffer", 0x03000000, 0x00300000, 4, 0x00},
    
    // Comet Lake
    {0x9BA40003, 0x9BA4, "CometLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 4, 0x00},
    {0x9BA40004, 0x9BA4, "CometLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 4, 0x00},
    {0x9BA40005, 0x9BA4, "CometLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 4, 0x00},
    {0x9BA40006, 0x9BA4, "CometLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 4, 0x00},
    {0x9BA40007, 0x9BA4, "CometLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 4, 0x00},
    {0x9BA40008, 0x9BA4, "CometLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 4, 0x00},
    {0x9BA40009, 0x9BA4, "CometLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 4, 0x00},
    {0x9BA4000A, 0x9BA4, "CometLake", "com.apple.driver.AppleIntelKBLGraphics", "com.apple.driver.AppleIntelCFLGraphicsFramebuffer", 0x01000000, 0x00200000, 4, 0x00},
};

static const size_t wgPlatformDatabaseCount = sizeof(wgPlatformDatabase) / sizeof(wgPlatformDatabase[0]);

// Helper functions
static inline const WGPlatformEntry* wgFindPlatformById(uint32_t platformId) {
    for (size_t i = 0; i < wgPlatformDatabaseCount; i++) {
        if (wgPlatformDatabase[i].platformId == platformId) {
            return &wgPlatformDatabase[i];
        }
    }
    return nullptr;
}

static inline const WGPlatformEntry* wgFindPlatformByDeviceId(uint16_t deviceId) {
    for (size_t i = 0; i < wgPlatformDatabaseCount; i++) {
        if (wgPlatformDatabase[i].deviceId == deviceId) {
            return &wgPlatformDatabase[i];
        }
    }
    return nullptr;
}

static inline const char* wgGetGenerationForPlatformId(uint32_t platformId) {
    const WGPlatformEntry* entry = wgFindPlatformById(platformId);
    return entry ? entry->generation : "Unknown";
}

static inline uint32_t wgGetDefaultFBMem(uint32_t platformId) {
    const WGPlatformEntry* entry = wgFindPlatformById(platformId);
    return entry ? entry->defaultFBMem : 0x00600000;
}

static inline uint32_t wgGetDefaultStolenMem(uint32_t platformId) {
    const WGPlatformEntry* entry = wgFindPlatformById(platformId);
    return entry ? entry->defaultStolenMem : 0x00180000;
}

static inline uint8_t wgGetDefaultPorts(uint32_t platformId) {
    const WGPlatformEntry* entry = wgFindPlatformById(platformId);
    return entry ? entry->defaultPorts : 3;
}

#endif // FORCEACL_WGCOMPAT_HPP