/**
 * ForceACL - GPU Properties Manager Implementation
 * WhateverGreen-compatible property injection engine
 */

#include <libkern/libkern.h>
#include <libkern/OSBase.h>
#include <IOKit/IOService.h>
#include <IOKit/IORegistryEntry.h>

#include "ForceACL/GPUPropertiesManager.hpp"

GPUPropertiesManager::GPUPropertiesManager()
    : m_verboseLogging(false) {
    GPU_PROPS_LOG_VERBOSE("GPUPropertiesManager initialized");
}

GPUPropertiesManager::~GPUPropertiesManager() {
    GPU_PROPS_LOG_VERBOSE("GPUPropertiesManager destroyed");
}

bool GPUPropertiesManager::injectIGPUProperties(IORegistryEntry* gpuDevice, uint32_t platformId) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting IGPU properties for platform ID 0x%08X", platformId);

    // Platform ID (primary)
    if (!setProperty(gpuDevice, "AAPL,ig-platform-id", platformId)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject AAPL,ig-platform-id");
    }

    // Inject device-id (may be spoofed for compatibility)
    uint32_t deviceId = platformId & 0xFFFF;
    if (!setProperty(gpuDevice, "device-id", deviceId)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject device-id");
    }

    // Graphics model name
    const char* modelName = "Intel HD Graphics";
    if (!setProperty(gpuDevice, "model", modelName)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject model name");
    }

    // Compatible property
    if (!setProperty(gpuDevice, "compatible", "pci8086,0042")) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject compatible");
    }

    // Enable graphics
    uint32_t enable = 1;
    if (!setProperty(gpuDevice, "IODisplayEnable", enable)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject IODisplayEnable");
    }

    return true;
}

bool GPUPropertiesManager::injectFramebufferProperties(IORegistryEntry* gpuDevice, uint32_t platformId) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting framebuffer properties for platform ID 0x%08X", platformId);

    // Enable framebuffer patching
    uint32_t fbPatchEnable = 1;
    if (!setProperty(gpuDevice, "framebuffer-patch-enable", fbPatchEnable)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject framebuffer-patch-enable");
    }

    // Stolen memory (DVMT pre-allocated)
    uint32_t stolenMem = 19 * 1024 * 1024; // 19MB default
    if (!setProperty(gpuDevice, "framebuffer-stolenmem", stolenMem)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject framebuffer-stolenmem");
    }

    // Framebuffer memory
    uint32_t fbMem = 16 * 1024 * 1024; // 16MB default
    if (!setProperty(gpuDevice, "framebuffer-fbmem", fbMem)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject framebuffer-fbmem");
    }

    // Unified memory
    uint32_t unifiedMem = 128 * 1024 * 1024; // 128MB default
    if (!setProperty(gpuDevice, "framebuffer-unifiedmem", unifiedMem)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject framebuffer-unifiedmem");
    }

    // Cursor memory (Haswell specific)
    uint32_t cursorMem = 16 * 1024 * 1024; // 16MB for cursor
    if (!setProperty(gpuDevice, "framebuffer-cursormem", cursorMem)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject framebuffer-cursormem");
    }

    // Port count (adjust per generation)
    uint32_t portCount = 3;
    if (!setProperty(gpuDevice, "framebuffer-portcount", portCount)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject framebuffer-portcount");
    }

    // Pipe count
    uint32_t pipeCount = 3;
    if (!setProperty(gpuDevice, "framebuffer-pipecount", pipeCount)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject framebuffer-pipecount");
    }

    GPU_PROPS_LOG("Framebuffer properties injected");
    return true;
}

bool GPUPropertiesManager::injectConnectorProperties(IORegistryEntry* gpuDevice, uint32_t platformId) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting connector properties");

    // Inject up to 8 connectors (max typical)
    for (uint8_t i = 0; i < 8; i++) {
        char name[64];

        // Enable connector
        snprintf(name, sizeof(name), "framebuffer-con%u-enable", i);
        uint32_t enable = (i < 3) ? 1 : 0; // First 3 typically enabled
        setProperty(gpuDevice, name, enable);

        // Bus ID (typically 0x04, 0x05, 0x06 for first three)
        snprintf(name, sizeof(name), "framebuffer-con%u-busid", i);
        uint8_t busId = generateConnectorBusId(platformId, i);
        setProperty(gpuDevice, name, busId);

        // Pipe (0, 1, 2 mapping)
        snprintf(name, sizeof(name), "framebuffer-con%u-pipe", i);
        uint8_t pipe = generateConnectorPipe(platformId, i);
        setProperty(gpuDevice, name, pipe);

        // Connector type (DP, HDMI, DVI, etc.)
        snprintf(name, sizeof(name), "framebuffer-con%u-type", i);
        uint32_t connType;
        switch (i) {
            case 0: connType = 0x0400; break; // DP
            case 1: connType = 0x0800; break; // HDMI
            case 2: connType = 0x0200; break; // DVI
            default: connType = 0x0000; break; // Disabled
        }
        setProperty(gpuDevice, name, connType);

        // Flags
        snprintf(name, sizeof(name), "framebuffer-con%u-flags", i);
        uint32_t flags = (i < 3) ? 0x87 : 0x00; // Enable, hotplug, etc.
        setProperty(gpuDevice, name, flags);
    }

    GPU_PROPS_LOG("Connector properties injected");
    return true;
}

bool GPUPropertiesManager::injectHDMIProperties(IORegistryEntry* gpuDevice) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting HDMI properties");

    // Enable HDMI 2.0 support
    uint32_t hdmi20 = 1;
    if (!setProperty(gpuDevice, "enable-hdmi20", hdmi20)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject enable-hdmi20");
    }

    // Force complete modeset for HDMI
    uint32_t forceMod = 1;
    if (!setProperty(gpuDevice, "force-complete-modeset", forceMod)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject force-complete-modeset");
    }

    // Enable HDMI audio
    uint32_t hdmiAudio = 1;
    if (!setProperty(gpuDevice, "enable-hdmi-audio", hdmiAudio)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject enable-hdmi-audio");
    }

    return true;
}

bool GPUPropertiesManager::injectMetalProperties(IORegistryEntry* gpuDevice) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting Metal support properties");

    // Metal support requires QE/CI enabled
    uint32_t enableQECI = 1;
    if (!setProperty(gpuDevice, "enable-gva", enableQECI)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject enable-gva");
    }

    // Enable software acceleration if needed
    uint32_t swAccel = 1;
    if (!setProperty(gpuDevice, "enable-metal", swAccel)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject enable-metal");
    }

    // Disable GPU blacklisting
    uint32_t disableGPUBlacklist = 1;
    if (!setProperty(gpuDevice, "disable-gfxfirmware", disableGPUBlacklist)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject disable-gfxfirmware");
    }

    return true;
}

bool GPUPropertiesManager::injectDisplayPipeProperties(IORegistryEntry* gpuDevice) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting display pipe properties");

    // Pixel clock limit override
    uint64_t maxPixelClock = 675000000; // 675 MHz for Skylake+
    if (!setProperty(gpuDevice, "vid-cfg0", (uint32_t)(maxPixelClock & 0xFFFFFFFF))) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject pixel clock config");
    }

    // Display data buffer optimization
    uint32_t ddbOpt = 1;
    if (!setProperty(gpuDevice, "enable-display-data-buffer-optimizer", ddbOpt)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject display buffer optimizer");
    }

    return true;
}

bool GPUPropertiesManager::injectVRAMProperties(IORegistryEntry* gpuDevice, uint32_t correctedVRAMMB) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting VRAM correction properties: %u MB", correctedVRAMMB);

    // Total VRAM (fake/corrected)
    if (!setProperty(gpuDevice, "AAPL,VRAM,totalMB", correctedVRAMMB)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject AAPL,VRAM,totalMB");
    }

    // Alternative VRAM property
    if (!setProperty(gpuDevice, "VRAM,totalMB", correctedVRAMMB)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject VRAM,totalMB");
    }

    // Disable stolen memory limitation
    uint32_t noLimit = 1;
    if (!setProperty(gpuDevice, "disable-stolen-mem-limit", noLimit)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject disable-stolen-mem-limit");
    }

    return true;
}

bool GPUPropertiesManager::injectDVMTProperties(IORegistryEntry* gpuDevice) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting DVMT properties");

    // DVMT pre-allocated 
    uint32_t dvmtPreAlloc = 64; // 64MB
    if (!setProperty(gpuDevice, "AAPL,DVMT,preallocMB", dvmtPreAlloc)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject AAPL,DVMT,preallocMB");
    }

    // DVMT total
    uint32_t dvmtTotal = 1024; // 1GB
    if (!setProperty(gpuDevice, "AAPL,DVMT,totalMB", dvmtTotal)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject AAPL,DVMT,totalMB");
    }

    return true;
}

bool GPUPropertiesManager::injectHDMIAudioProperties(IORegistryEntry* gpuDevice) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting HDMI audio properties");

    // Audio codec enable
    uint32_t audioEnable = 1;
    if (!setProperty(gpuDevice, "enable-hdmi-audio-codec", audioEnable)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject audio codec enable");
    }

    return true;
}

bool GPUPropertiesManager::injectBacklightProperties(IORegistryEntry* gpuDevice) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting backlight properties");

    // Backlight control
    uint32_t blControl = 1;
    if (!setProperty(gpuDevice, "AAPL,backlight-control", blControl)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject backlight control");
    }

    // PWM maximum
    uint32_t pwmMax = 0x0AD9; // Typical max PWM
    if (!setProperty(gpuDevice, "AppleBacklightMaximumLevel", pwmMax)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject backlight max");
    }

    return true;
}

bool GPUPropertiesManager::injectLSPCONProperties(IORegistryEntry* gpuDevice) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting LSPCON support properties");

    // LSPCON support for Type-C systems
    uint32_t lspconEnable = 1;
    if (!setProperty(gpuDevice, "enable-lspcon-support", lspconEnable)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject LSPCON support");
    }

    return true;
}

bool GPUPropertiesManager::injectFBCProperties(IORegistryEntry* gpuDevice) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting FBC properties");

    // Framebuffer compression enable
    uint32_t fbcEnable = 1;
    if (!setProperty(gpuDevice, "enable-fbc", fbcEnable)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject FBC enable");
    }

    return true;
}

bool GPUPropertiesManager::injectPowerManagementProperties(IORegistryEntry* gpuDevice) {
    if (!gpuDevice) return false;

    GPU_PROPS_LOG("Injecting power management properties");

    // RC6 (power state management)
    uint32_t rc6 = 1;
    if (!setProperty(gpuDevice, "enable-rc6", rc6)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject RC6 enable");
    }

    // GPU turbo boost
    uint32_t turbo = 1;
    if (!setProperty(gpuDevice, "enable-turbo-freq", turbo)) {
        GPU_PROPS_LOG_VERBOSE("Failed to inject turbo freq");
    }

    return true;
}

void GPUPropertiesManager::setVerboseLogging(bool verbose) {
    m_verboseLogging = verbose;
}

bool GPUPropertiesManager::setProperty(IORegistryEntry* device, const char* key, uint32_t value) {
    if (!device || !key) return false;

    OSData* data = OSData::withBytes(&value, sizeof(value));
    if (!data) return false;

    device->setProperty(key, data);
    data->release();

    GPU_PROPS_LOG_VERBOSE("Set property %s = 0x%08X", key, value);
    return true;
}

bool GPUPropertiesManager::setProperty(IORegistryEntry* device, const char* key, const char* value) {
    if (!device || !key || !value) return false;

    OSString* str = OSString::withCString(value);
    if (!str) return false;

    device->setProperty(key, str);
    str->release();

    GPU_PROPS_LOG_VERBOSE("Set property %s = %s", key, value);
    return true;
}

bool GPUPropertiesManager::setProperty(IORegistryEntry* device, const char* key, const uint8_t* data, size_t size) {
    if (!device || !key || !data) return false;

    OSData* osData = OSData::withBytes(data, size);
    if (!osData) return false;

    device->setProperty(key, osData);
    osData->release();

    GPU_PROPS_LOG_VERBOSE("Set property %s (%lu bytes)", key, size);
    return true;
}

uint8_t GPUPropertiesManager::generateConnectorBusId(uint32_t platformId, uint8_t connectorIndex) {
    // Generate bus IDs typically: 0x04, 0x05, 0x06 for IGD
    static uint8_t busIds[] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};
    if (connectorIndex < sizeof(busIds)/sizeof(busIds[0])) {
        return busIds[connectorIndex];
    }
    return 0x04 + connectorIndex;
}

uint8_t GPUPropertiesManager::generateConnectorPipe(uint32_t platformId, uint8_t connectorIndex) {
    // Standard pipe mapping (0, 1, 2, repeat)
    return connectorIndex % 3;
}

uint32_t GPUPropertiesManager::getRecommendedVRAM(uint32_t platformId) {
    // Based on generation
    uint32_t highByte = (platformId >> 24) & 0xFF;

    // Modern GPUs: 1.5-2GB
    if (highByte >= 0x19) return 2048;  // Skylake+
    
    // Mid-range: 1-1.5GB
    if (highByte >= 0x0D) return 1536;  // Haswell+
    
    // Older: 512MB-1GB
    return 1024;
}

uint32_t GPUPropertiesManager::getRecommendedDVMT(uint32_t platformId) {
    uint32_t highByte = (platformId >> 24) & 0xFF;

    // Newer requires more
    if (highByte >= 0x9A) return 96;  // Modern (Ice Lake+)
    if (highByte >= 0x59) return 64;  // Kaby Lake+
    
    return 32; // Older
}