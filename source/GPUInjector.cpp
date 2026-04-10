/**
 * ForceACL - GPU Injector Implementation
 * Injects platform IDs and GPU properties
 */

#include <libkern/libkern.h>
#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>

#include "ForceACL/GPUInjector.hpp"
#include "ForceACL/PlatformDatabase.hpp"

GPUInjector::GPUInjector(PlatformIDDatabase* db)
    : m_platformDB(db)
    , m_verboseLogging(false)
{
    GPU_INJECT_LOG_VERBOSE("GPUInjector initialized");
}

GPUInjector::~GPUInjector() {
    GPU_INJECT_LOG_VERBOSE("GPUInjector destroyed");
}

bool GPUInjector::injectProperties(IOPCIDevice* device, uint32_t platformId) {
    if (!device) {
        GPU_INJECT_LOG("injectProperties: null device");
        return false;
    }

    GPU_INJECT_LOG_VERBOSE("Injecting properties for platform ID 0x%08X", platformId);

    bool success = true;

    // Inject platform ID
    success &= injectPlatformId(device, platformId);

    // Inject framebuffer properties
    success &= injectFramebufferProperties(device, platformId);

    // Inject AAPL properties
    success &= injectAAPLProperties(device);

    // Inject device properties
    success &= injectDeviceProperties(device);

    GPU_INJECT_LOG_VERBOSE("Property injection %s", success ? "successful" : "failed");
    return success;
}

bool GPUInjector::injectFramebufferProperties(IOPCIDevice* device, uint32_t platformId) {
    if (!device) return false;

    GPU_INJECT_LOG_VERBOSE("Injecting framebuffer properties");

    // Inject framebuffer memory size (fake VRAM)
    uint32_t fbMemorySize = 0; // Will be set based on platform

    // Determine VRAM size based on platform ID generation
    if (platformId >= 0x01660000 && platformId <= 0x0166FFFF) { // Sandy Bridge
        fbMemorySize = 512 * 1024 * 1024; // 512MB
    } else if (platformId >= 0x01620000 && platformId <= 0x0162FFFF) { // Ivy Bridge
        fbMemorySize = 1024 * 1024 * 1024; // 1GB
    } else if (platformId >= 0x0D220000 && platformId <= 0x0D22FFFF) { // Haswell
        fbMemorySize = 1536 * 1024 * 1024; // 1.5GB
    } else if (platformId >= 0x19020000 && platformId <= 0x1926FFFF) { // Skylake+
        fbMemorySize = 1536 * 1024 * 1024; // 1.5GB
    } else {
        fbMemorySize = 1024 * 1024 * 1024; // 1GB default
    }

    // Inject VRAM size override
    if (!injectProperty32(device, "AAPL,snb-platform-id", platformId)) {
        GPU_INJECT_LOG_VERBOSE("Failed to inject AAPL,snb-platform-id");
    }

    // Inject framebuffer memory
    if (!injectProperty32(device, "AAPL,VRAM,totalMB", fbMemorySize / (1024 * 1024))) {
        GPU_INJECT_LOG_VERBOSE("Failed to inject VRAM size");
    }

    return true;
}

bool GPUInjector::injectAAPLProperties(IOPCIDevice* device) {
    if (!device) return false;

    GPU_INJECT_LOG_VERBOSE("Injecting AAPL properties");

    // Inject graphics control properties
    injectPropertyString(device, "AAPL,GfxYTile", "1");

    // Inject backlight control
    injectProperty32(device, "AAPL,backlight-control", 0x01);

    // Inject display pipe
    injectProperty32(device, "AAPL00,DualLink", 0x01);

    return true;
}

bool GPUInjector::injectDeviceProperties(IOPCIDevice* device) {
    if (!device) return false;

    GPU_INJECT_LOG_VERBOSE("Injecting device properties");

    // Inject device ID override if needed
    injectDeviceId(device, 0x01268086); // Default Intel HD 3000

    // Inject class code
    injectClassCode(device);

    // Inject vendor/device names
    injectVendorName(device);
    injectModelName(device);

    // Inject IO name
    injectIOName(device);

    // Inject compatible
    injectCompatible(device);

    // Inject driver properties
    injectDriverProperties(device);

    return true;
}

bool GPUInjector::injectProperty(IOPCIDevice* device, const char* key, const void* data, size_t length) {
    if (!device || !key || !data) return false;

    OSData* osData = OSData::withBytes(data, length);
    if (!osData) return false;

    device->setProperty(key, osData);
    osData->release();

    GPU_INJECT_LOG_VERBOSE("Injected property: %s", key);
    return true;
}

bool GPUInjector::injectProperty32(IOPCIDevice* device, const char* key, uint32_t value) {
    return injectProperty(device, key, &value, sizeof(value));
}

bool GPUInjector::injectProperty16(IOPCIDevice* device, const char* key, uint16_t value) {
    return injectProperty(device, key, &value, sizeof(value));
}

bool GPUInjector::injectPropertyString(IOPCIDevice* device, const char* key, const char* value) {
    if (!value) return false;
    return injectProperty(device, key, value, strlen(value) + 1);
}

void GPUInjector::setVerboseLogging(bool verbose) {
    m_verboseLogging = verbose;
}

bool GPUInjector::injectPlatformId(IOPCIDevice* device, uint32_t platformId) {
    return injectProperty32(device, "AAPL,ig-platform-id", platformId);
}

bool GPUInjector::injectDeviceId(IOPCIDevice* device, uint32_t deviceId) {
    return injectProperty32(device, "device-id", deviceId);
}

bool GPUInjector::injectClassCode(IOPCIDevice* device) {
    uint32_t classCode = 0x030000; // VGA compatible controller
    return injectProperty32(device, "class-code", classCode);
}

bool GPUInjector::injectVendorName(IOPCIDevice* device) {
    return injectPropertyString(device, "vendor-name", "Intel");
}

bool GPUInjector::injectModelName(IOPCIDevice* device) {
    return injectPropertyString(device, "model", "Intel HD Graphics");
}

bool GPUInjector::injectIOName(IOPCIDevice* device) {
    return injectPropertyString(device, "IOName", "IntelAccelerator");
}

bool GPUInjector::injectCompatible(IOPCIDevice* device) {
    const char* compatible[] = {"pci8086,1626", "pci8086,162b", "pci8086,1622"};
    OSArray* compatArray = OSArray::withCapacity(3);

    for (int i = 0; i < 3; i++) {
        OSString* compatStr = OSString::withCString(compatible[i]);
        if (compatStr) {
            compatArray->setObject(i, compatStr);
            compatStr->release();
        }
    }

    device->setProperty("compatible", compatArray);
    compatArray->release();

    return true;
}

bool GPUInjector::injectDriverProperties(IOPCIDevice* device) {
    // Inject properties to help driver matching
    injectProperty32(device, "AAPL,ig-platform-id", 0x0166000B); // Default platform ID
    injectPropertyString(device, "AAPL,GfxName", "Intel HD Graphics 4000");

    return true;
}