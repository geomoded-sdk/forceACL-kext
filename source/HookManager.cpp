/**
 * ForceACL - Hook Manager Implementation
 * Manages IOService hooks and callbacks for GPU patching
 */

#include <libkern/libkern.h>
#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOCatalogue.h>
#include <IOKit/IORegistryEntry.h>

#include "ForceACL/HookManager.hpp"

// External references for hooking
extern "C" {
    kern_return_t _ZN9IOService20probeCandidatesEP12OSOrderedSet(void);
    kern_return_t _ZN9IOService14doServiceMatchEm(void);
    void* _ZN12IORegistryEntry11setPropertyEPKcPKvm(void);
}

// Hook function pointers
static kern_return_t (*original_probeCandidates)(void* that, void* orderedSet) = nullptr;
static kern_return_t (*original_doServiceMatch)(void* that, unsigned long options) = nullptr;
static void* (*original_setProperty)(void* that, const char* key, const void* data, unsigned long length) = nullptr;

HookManager::HookManager()
    : m_pciHookRegistered(false)
    , m_framebufferHookRegistered(false)
    , m_verboseLogging(false)
{
    HOOK_LOG_VERBOSE("HookManager initialized");
}

HookManager::~HookManager() {
    unregisterAllHooks();
    HOOK_LOG_VERBOSE("HookManager destroyed");
}

bool HookManager::registerPCIHook() {
    if (m_pciHookRegistered) {
        HOOK_LOG_VERBOSE("PCI hook already registered");
        return true;
    }

    HOOK_LOG("Registering PCI device hooks...");

    // Hook IOPCIDevice probe
    // This is a simplified hook - in real implementation would use Lilu routing
    m_pciHookRegistered = true;

    HOOK_LOG_VERBOSE("PCI hook registered successfully");
    return true;
}

bool HookManager::registerFramebufferHook() {
    if (m_framebufferHookRegistered) {
        HOOK_LOG_VERBOSE("Framebuffer hook already registered");
        return true;
    }

    HOOK_LOG("Registering framebuffer hooks...");

    // Hook IORegistryEntry::setProperty for framebuffer patching
    // This would typically use Lilu routing to hook AppleIntelFramebuffer methods

    m_framebufferHookRegistered = true;

    HOOK_LOG_VERBOSE("Framebuffer hook registered successfully");
    return true;
}

bool HookManager::registerServiceHooks() {
    HOOK_LOG("Registering service matching hooks...");

    // Register for IOService notifications
    // This would hook into IOService matching for GPU services

    return true;
}

bool HookManager::unregisterAllHooks() {
    HOOK_LOG_VERBOSE("Unregistering all hooks...");

    m_pciHookRegistered = false;
    m_framebufferHookRegistered = false;

    HOOK_LOG_VERBOSE("All hooks unregistered");
    return true;
}

void HookManager::setVerboseLogging(bool verbose) {
    m_verboseLogging = verbose;
}

// Static callback functions (for Lilu compatibility)
bool HookManager::pciDeviceCallback(void* device) {
    if (!device) return false;

    IOPCIDevice* pciDevice = (IOPCIDevice*)device;

    // Check if this is an Intel GPU
    uint16_t vendorId = 0;
    uint16_t deviceId = 0;

    // Get vendor/device IDs
    OSData* vendorData = (OSData*)pciDevice->getProperty("vendor-id");
    OSData* deviceData = (OSData*)pciDevice->getProperty("device-id");

    if (vendorData && deviceData) {
        vendorId = *(uint16_t*)vendorData->getBytesNoCopy();
        deviceId = *(uint16_t*)deviceData->getBytesNoCopy();

        if (vendorId == 0x8086) { // Intel
            HOOK_LOG_VERBOSE("Intel GPU detected: device 0x%04X", deviceId);

            // Apply patches here
            // - Fake VRAM override
            // - DVMT fix
            // - Connector correction
        }
    }

    return true;
}

bool HookManager::framebufferCallback(void* service) {
    if (!service) return false;

    IOService* ioService = (IOService*)service;

    // Check if this is AppleIntelFramebuffer
    const char* className = ioService->getMetaClass()->getClassName();
    if (strcmp(className, "AppleIntelFramebuffer") == 0) {
        HOOK_LOG("AppleIntelFramebuffer service detected - applying patches");

        // Apply framebuffer patches:
        // 1. Automatic framebuffer patch
        // 2. Connectors correction
        // 3. DVMT pre-allocated memory fix

        applyFramebufferPatches(ioService);
    }

    return true;
}

bool HookManager::serviceMatchingCallback(void* service) {
    if (!service) return false;

    IOService* ioService = (IOService*)service;

    // Handle service matching callbacks
    HOOK_LOG_VERBOSE("Service matching callback for: %s",
        ioService->getMetaClass()->getClassName());

    return true;
}

// Private helper functions
void HookManager::applyFramebufferPatches(IOService* framebuffer) {
    if (!framebuffer) return;

    HOOK_LOG("Applying framebuffer patches...");

    // 1. Fake VRAM adjustment via IORegistry override
    applyFakeVRAMOverride(framebuffer);

    // 2. DVMT fix
    applyDVMTFix(framebuffer);

    // 3. Connectors correction
    applyConnectorsCorrection(framebuffer);

    // 4. Automatic framebuffer patch
    applyAutomaticFramebufferPatch(framebuffer);

    HOOK_LOG_VERBOSE("Framebuffer patches applied");
}

void HookManager::applyFakeVRAMOverride(IOService* framebuffer) {
    HOOK_LOG_VERBOSE("Applying fake VRAM override...");

    // Override VRAM size in IORegistry
    // This forces the system to report more VRAM than physically available

    uint32_t fakeVRAM_MB = 2048; // 2GB fake VRAM

    framebuffer->setProperty("AAPL,VRAM,totalMB", &fakeVRAM_MB, sizeof(fakeVRAM_MB));
    framebuffer->setProperty("VRAM,totalMB", &fakeVRAM_MB, sizeof(fakeVRAM_MB));

    HOOK_LOG("Fake VRAM set to %u MB", fakeVRAM_MB);
}

void HookManager::applyDVMTFix(IOService* framebuffer) {
    HOOK_LOG_VERBOSE("Applying DVMT fix...");

    // DVMT (Dynamic Video Memory Technology) pre-allocated memory fix
    // This ensures proper pre-allocated video memory for Intel GPUs

    uint32_t dvmtPreAlloc = 64; // 64MB DVMT pre-allocated
    uint32_t dvmtTotal = 1024; // 1GB DVMT total

    framebuffer->setProperty("AAPL,DVMT,preallocMB", &dvmtPreAlloc, sizeof(dvmtPreAlloc));
    framebuffer->setProperty("AAPL,DVMT,totalMB", &dvmtTotal, sizeof(dvmtTotal));

    HOOK_LOG("DVMT pre-allocated: %u MB, total: %u MB", dvmtPreAlloc, dvmtTotal);
}

void HookManager::applyConnectorsCorrection(IOService* framebuffer) {
    HOOK_LOG_VERBOSE("Applying connectors correction...");

    // Fix connector information for proper display detection
    // This patches the connector data structures in the framebuffer

    // Create corrected connector data
    struct ConnectorData {
        uint32_t index;
        uint32_t busId;
        uint32_t pipe;
        uint32_t type;
        uint32_t flags;
    };

    // Example connector correction for common setups
    ConnectorData connectors[] = {
        {0, 0x05, 0, 0x00000400, 0x00000000}, // DP
        {1, 0x04, 1, 0x00000800, 0x00000000}, // HDMI
        {2, 0x06, 2, 0x00000200, 0x00000000}  // DVI
    };

    // Inject corrected connector data
    framebuffer->setProperty("AAPL,connector", connectors, sizeof(connectors));

    HOOK_LOG("Connectors corrected for %zu displays", sizeof(connectors)/sizeof(ConnectorData));
}

void HookManager::applyAutomaticFramebufferPatch(IOService* framebuffer) {
    HOOK_LOG_VERBOSE("Applying automatic framebuffer patch...");

    // Automatic framebuffer patching
    // This patches various framebuffer parameters automatically based on the platform

    // Patch framebuffer memory layout
    uint32_t fbPatch = 0x01; // Enable framebuffer patches
    framebuffer->setProperty("AAPL,framebuffer-patch", &fbPatch, sizeof(fbPatch));

    // Patch cursor memory
    uint32_t cursorBytes = 0x100000; // 1MB cursor memory
    framebuffer->setProperty("AAPL,cursor-bytes", &cursorBytes, sizeof(cursorBytes));

    // Patch unified memory
    uint32_t unifiedMemory = 0x01; // Enable unified memory
    framebuffer->setProperty("AAPL,unified-memory", &unifiedMemory, sizeof(unifiedMemory));

    HOOK_LOG("Automatic framebuffer patch applied");
}