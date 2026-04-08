/**
 * ForceACL - GPU Injector Module
 * Injects platform IDs and GPU properties
 */

#ifndef FORCEACL_GPUINJECTOR_HPP
#define FORCEACL_GPUINJECTOR_HPP

#include <stdint.h>

class PlatformIDDatabase;
class IOPCIDevice;

class GPUInjector {
public:
    GPUInjector(PlatformIDDatabase* db);
    ~GPUInjector();
    
    bool injectProperties(IOPCIDevice* device, uint32_t platformId);
    bool injectFramebufferProperties(IOPCIDevice* device, uint32_t platformId);
    bool injectAAPLProperties(IOPCIDevice* device);
    bool injectDeviceProperties(IOPCIDevice* device);
    
    bool injectProperty(IOPCIDevice* device, const char* key, const void* data, size_t length);
    bool injectProperty32(IOPCIDevice* device, const char* key, uint32_t value);
    bool injectProperty16(IOPCIDevice* device, const char* key, uint16_t value);
    bool injectPropertyString(IOPCIDevice* device, const char* key, const char* value);
    
    void setVerboseLogging(bool verbose);
    
private:
    PlatformIDDatabase* m_platformDB;
    bool m_verboseLogging;
    
    bool injectPlatformId(IOPCIDevice* device, uint32_t platformId);
    bool injectDeviceId(IOPCIDevice* device, uint32_t deviceId);
    bool injectClassCode(IOPCIDevice* device);
    bool injectVendorName(IOPCIDevice* device);
    bool injectModelName(IOPCIDevice* device);
    bool injectIOName(IOPCIDevice* device);
    bool injectCompatible(IOPCIDevice* device);
    bool injectDriverProperties(IOPCIDevice* device);
};

// Logging macros
#define GPU_INJECT_LOG(fmt, args...) \
    do { IOLog("ForceACL: [Injector] " fmt "\n", ##args); } while(0)

#define GPU_INJECT_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [Injector-VERBOSE] " fmt "\n", ##args); } while(0)

#endif // FORCEACL_GPUINJECTOR_HPP
