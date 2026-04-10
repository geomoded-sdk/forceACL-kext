/**
 * ForceACL - GPU Properties Injector (WhateverGreen Integrated)
 * Expands GPUInjector with full WhateverGreen-compatible property injection
 */

#ifndef FORCEACL_GPU_PROPERTIES_HPP
#define FORCEACL_GPU_PROPERTIES_HPP

#include <stdint.h>
#include <stddef.h>
#include <IOKit/IOService.h>

class IOPCIDevice;
class IORegistryEntry;

/**
 * GPU Properties Manager - Compatible with WhateverGreen property format
 */
class GPUPropertiesManager {
public:
    GPUPropertiesManager();
    ~GPUPropertiesManager();
    
    // === Core Property Injection ===
    
    /**
     * Inject essential IGPU properties for GPU acceleration
     * Mimics WhateverGreen IGPU property injection
     */
    bool injectIGPUProperties(IORegistryEntry* gpuDevice, uint32_t platformId);
    
    /**
     * Inject framebuffer-specific properties
     * Properties: framebuffer-patch-enable, framebuffer-stolenmem, etc.
     */
    bool injectFramebufferProperties(IORegistryEntry* gpuDevice, uint32_t platformId);
    
    /**
     * Inject connector properties for display configuration
     * Properties: framebuffer-conX-enable, framebuffer-conX-busid, type, etc.
     */
    bool injectConnectorProperties(IORegistryEntry* gpuDevice, uint32_t platformId);
    
    /**
     * Inject HDMI-specific properties
     * Properties: enable-hdmi20, disable-hdmi-patch, etc.
     */
    bool injectHDMIProperties(IORegistryEntry* gpuDevice);
    
    /**
     * Inject Metal acceleration properties
     */
    bool injectMetalProperties(IORegistryEntry* gpuDevice);
    
    /**
     * Inject display pipe properties
     */
    bool injectDisplayPipeProperties(IORegistryEntry* gpuDevice);
    
    /**
     * Inject VRAM correction properties
     */
    bool injectVRAMProperties(IORegistryEntry* gpuDevice, uint32_t correctedVRAMMB);
    
    /**
     * Inject DVMT pre-allocated memory properties
     */
    bool injectDVMTProperties(IORegistryEntry* gpuDevice);
    
    /**
     * Inject audio/HDMI audio properties
     */
    bool injectHDMIAudioProperties(IORegistryEntry* gpuDevice);
    
    /**
     * Inject backlight properties for laptops
     */
    bool injectBacklightProperties(IORegistryEntry* gpuDevice);
    
    // === Advanced Features ===
    
    /**
     * Inject LSPCON support properties for some systems
     */
    bool injectLSPCONProperties(IORegistryEntry* gpuDevice);
    
    /**
     * Inject FBC (Framebuffer Compression) properties
     */
    bool injectFBCProperties(IORegistryEntry* gpuDevice);
    
    /**
     * Inject power management properties
     */
    bool injectPowerManagementProperties(IORegistryEntry* gpuDevice);
    
    // === Helper Methods ===
    
    /**
     * Set verbose logging for debugging
     */
    void setVerboseLogging(bool verbose);
    
private:
    bool m_verboseLogging;
    
    // Internal helpers
    bool setProperty(IORegistryEntry* device, const char* key, uint32_t value);
    bool setProperty(IORegistryEntry* device, const char* key, const char* value);
    bool setProperty(IORegistryEntry* device, const char* key, const uint8_t* data, size_t size);
    
    /**
     * Generate connector busId based on generation and platform
     */
    uint8_t generateConnectorBusId(uint32_t platformId, uint8_t connectorIndex);
    
    /**
     * Generate connector pipe mapping
     */
    uint8_t generateConnectorPipe(uint32_t platformId, uint8_t connectorIndex);
    
    /**
     * Get VRAM size recommendation for platform
     */
    uint32_t getRecommendedVRAM(uint32_t platformId);
    
    /**
     * Get DVMT pre-allocated memory for platform
     */
    uint32_t getRecommendedDVMT(uint32_t platformId);
};

// Logging macros
#define GPU_PROPS_LOG(fmt, args...) \
    do { IOLog("ForceACL: [GPUProps] " fmt "\n", ##args); } while(0)

#define GPU_PROPS_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [GPUProps-VERBOSE] " fmt "\n", ##args); } while(0)

extern bool gForceACLVerbose;

#endif // FORCEACL_GPU_PROPERTIES_HPP
