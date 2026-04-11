/**
 * ForceACL - Framebuffer Patching Engine Header
 * Replaces WhateverGreen functionality with native patching
 */

#ifndef FORCEACL_FRAMEBUFFERPATCHER_HPP
#define FORCEACL_FRAMEBUFFERPATCHER_HPP

#include <libkern/libkern.h>
#include <libkern/OSBase.h>
#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/graphics/IOGraphicsInterface.h>

#include <stdint.h>

#include "ForceACL/WGCompat.hpp"

// Forward declare KernelPatcher for integration
namespace KernelPatcher {
    struct RouteRequest;
    struct SolveRequest;
}

// Framebuffer patch types
enum class FramebufferPatchType {
    None = 0,
    ConnectorFix = 1,
    PipeMapping = 2,
    BusIDFix = 3,
    VRAMFix = 4,
    MemoryPatch = 5,
    MetalPatch = 6,
    ConsoleFix = 7,
    DPtoHDMI = 8,
    LVDSFix = 9,
    eDPFix = 10
};

// Framebuffer connector types
enum class ConnectorType {
    Unknown = 0,
    LVDS = 1,
    DP = 2,
    HDMI = 3,
    DVI = 4,
    VGA = 5,
    eDP = 6,
    DSI = 7
};

// Connector patch structure (mirrors WhateverGreen)
struct ConnectorPatch {
    uint8_t connType;      // Connector type
    uint8_t busID;        // Bus ID
    uint8_t pipe;         // Pipe number
    uint8_t laneCount;    // Lane count (0-3 for DP, 0-1 for others)
    uint16_t flags;       // Flags
    uint32_t features;    // Feature flags
};

// Framebuffer patch structure
struct FramebufferPatch {
    uint32_t platformID;
    uint32_t patches[16]; // Patch data
    uint32_t patchCount;
    ConnectorPatch connectors[6];
    uint8_t connectorCount;
    uint8_t portCount;
    uint8_t pipeCount;
    uint8_t cursorVRAM;   // MB for cursor
    uint32_t stolenMem;   // Bytes for stolen memory
    uint32_t fbMem;       // Bytes for framebuffer
    uint32_t unifiedMem; // Bytes for unified memory
    bool patchEnabled;
    bool hdmi20Enabled;
    bool forceOnline;
};

// Framebuffer config for each generation
struct FramebufferConfig {
    const char* generation;
    uint8_t maxPorts;
    uint8_t maxPipes;
    uint32_t defaultStolenMem;
    uint32_t defaultFbMem;
    uint32_t defaultUnifiedMem;
    uint8_t cursorVRAM;
    bool hasHotPlug;
    bool hasBacklight;
};

// VRAM detection and patching
struct VRAMInfo {
    uint32_t detectedMB;
    uint32_t correctedMB;
    bool isValid;
    bool needsPatch;
};

// Framebuffer patch result
struct PatchResult {
    bool success;
    uint32_t platformID;
    uint32_t patchesApplied;
    uint32_t connectorsFixed;
    bool accelerationEnabled;
    bool metalEnabled;
    uint32_t vramMB;
    char statusMessage[256];
};

class FramebufferPatcher {
public:
    FramebufferPatcher();
    ~FramebufferPatcher();

    // Main patching API
    bool patchFramebuffer(IOPCIDevice* gpuDevice, uint32_t platformID);
    bool applyPatches(IOPCIDevice* device, uint32_t platformID);
    bool validateAcceleration();

    // Stage 2: Framebuffer patching
    bool patchFramebufferMemory(uint32_t platformID);
    bool patchFramebufferTables(IOService* framebuffer);
    bool patchFramebufferConnectors(uint32_t platformID);

    // Stage 3: Connector fixes
    bool fixConnectors(IOService* framebuffer, uint32_t platformID);
    bool fixDPtoHDMI(uint8_t connectorIndex);
    bool fixLVDS(uint8_t connectorIndex);
    bool fixEDP(uint8_t connectorIndex);
    bool applyConnectorPatch(uint8_t index, const ConnectorPatch* patch);

    // Property injection
    bool injectFramebufferProperties(IOPCIDevice* device, uint32_t platformID);
    bool injectConnectorProperties(IOService* framebuffer, uint32_t platformID);
    
    // Property generation
    uint32_t generateStolenMem(uint32_t base, uint32_t gpuRAM);
    uint32_t generateFbMem(uint32_t gpuRAM);
    uint32_t generateUnifiedMem(uint32_t gpuRAM);
    bool generateDynamicProperties(uint32_t platformID, IOService* framebuffer);

    // VRAM fixes
    VRAMInfo detectVRAM();
    bool fixVRAM(uint32_t expectedMB);
    bool applyVRAMPatch(uint32_t platformID);

    // Configuration
    const FramebufferConfig* getConfigForGeneration(const char* generation);
    bool loadPatchProfile(uint32_t platformID);
    bool selectBestPatchProfile(uint32_t deviceID);

    // Pipeline execution
    bool executePatchPipeline(IOPCIDevice* device, uint32_t platformID);
    PatchResult getLastResult() { return m_lastResult; }

    // Debug
    void dumpFramebufferInfo(IOService* framebuffer);
    void logPatchDetails();
    void setVerboseLogging(bool verbose) { m_verboseLogging = verbose; }
    
    // Set framebuffer service for connector patching
    void setFramebufferService(IOService* service) { m_framebufferService = service; }
    
    // Get WGCompat platform info
    const WGPlatformEntry* getWGCompatInfo(uint32_t platformID) const;

private:
    // Patch database
    static const FramebufferConfig m_generationConfigs[];
    static const size_t m_configCount;
    
    // Current state
    FramebufferPatch m_currentPatch;
    PatchResult m_lastResult;
    bool m_verboseLogging;
    bool m_patchApplied;
    uint32_t m_currentPlatformID;
    IOService* m_framebufferService;

    // Internal methods
    bool initializePatchProfile(uint32_t platformID);
    bool detectFramebufferVersion(IOService* framebuffer);
    bool patchFramebufferInternal(IOService* framebuffer, uint32_t platformID);
    bool modifyFramebufferTable(uint32_t offset, const void* data, size_t size);
    bool readFramebufferTable(uint32_t offset, void* buffer, size_t size);
    const FramebufferPatch* findMatchingPatch(uint32_t platformID, uint16_t deviceID);
    bool applyHeuristicPatches(uint32_t platformID);
    
    // Connector detection
    uint8_t detectConnectorCount(IOService* framebuffer);
    bool mapConnectorsToPipes();
    bool validateConnectorLayout();
};

// Logging macros
#define FB_PATCH_LOG(fmt, args...) \
    do { IOLog("ForceACL: [FBPatch] " fmt "\n", ##args); } while(0)

#define FB_PATCH_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [FBPatch-VERBOSE] " fmt "\n", ##args); } while(0)

#define FB_PATCH_LOG_ERROR(fmt, args...) \
    do { IOLog("ForceACL: [FBPatch-ERROR] " fmt "\n", ##args); } while(0)

// Helper macros for framebuffer property names
#define FB_PROP_PATCH_ENABLE "framebuffer-patch-enable"
#define FB_PROP_STOLENMEM "framebuffer-stolenmem"
#define FB_PROP_FBMEM "framebuffer-fbmem"
#define FB_PROP_UNIFIEDMEM "framebuffer-unifiedmem"
#define FB_PROP_CONN_ENABLE "framebuffer-con%u-enable"
#define FB_PROP_CONN_TYPE "framebuffer-con%u-type"
#define FB_PROP_CONN_BUSID "framebuffer-con%u-busid"
#define FB_PROP_CONN_PIPE "framebuffer-con%u-pipe"
#define FB_PROP_CONN_LANES "framebuffer-con%u-lanes"
#define FB_PROP_HDMI20 "enable-hdmi20"
#define FB_PROP_FORCE_ONLINE "force-online"
#define FB_PROP_CURSOR_VRAM "framebuffer-cursorvmr"
#define FB_PROP_PORT_COUNT "framebuffer-portcount"
#define FB_PROP_PIPE_COUNT "framebuffer-pipecount"
#define FB_PROP_AAPL_VERSION "AAPL,glass-version"
#define FB_PROP_FB_NAME "IOFBDependentIndex"

#endif // FORCEACL_FRAMEBUFFERPATCHER_HPP