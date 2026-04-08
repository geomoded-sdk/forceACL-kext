/**
 * ForceACL - GPU Detector Module
 * Detects Intel GPU generations and device IDs
 */

#include <Lilu/kern_api.hpp>
#include <Lilu/kern_util.hpp>
#include <IOKit/pci/IOPCIDevice.h>

#include <libkern/libkern.h>
#include <libkern/OSBase.h>

#include <cstring>
#include <cstdio>

#ifndef FORCEACL_GPUDETECTOR_HPP
#define FORCEACL_GPUDETECTOR_HPP

// GPU Generation enumeration
enum class GPUGeneration {
    Unknown = 0,
    SandyBridge = 1,
    IvyBridge = 2,
    Haswell = 3,
    Broadwell = 4,
    Skylake = 5,
    KabyLake = 6,
    CoffeeLake = 7,
    CometLake = 8,
    IceLake = 9,
    TigerLake = 10,
    RocketLake = 11,
    AlderLakeS = 12,
    AlderLakeP = 13,
    MeteorLake = 14,
    LunarLake = 15
};

// Device ID to generation mapping
struct DeviceIDMapping {
    uint16_t deviceId;
    GPUGeneration generation;
    const char* name;
    const char* marketingName;
};

// GPU Detector class
class GPUDetector {
public:
    GPUDetector();
    ~GPUDetector();
    
    GPUGeneration detectGeneration(uint16_t deviceId);
    const char* generationToString(GPUGeneration gen);
    const char* getMarketingName(uint16_t deviceId);
    bool isSupported(uint16_t deviceId);
    bool isHaswell(uint16_t deviceId);
    bool isIntel(uint16_t vendorId);
    
    // Graphics acceleration detection
    bool isGraphicsAccelerationActive(IOPCIDevice* device);
    bool checkIOGraphicsAcceleration(IOService* graphicsService);
    bool verifyQE_CI_Enabled(IOPCIDevice* device);
    bool testGraphicsMemoryAccess(IOPCIDevice* device);
    uint32_t getGraphicsMemorySize(IOPCIDevice* device);
    bool isGraphicsDeviceReady(IOPCIDevice* device);
    
private:
    static const DeviceIDMapping m_deviceMappings[];
    static const size_t m_mappingCount;
    
    GPUGeneration matchDeviceId(uint16_t deviceId);
    const char* lookupMarketingName(GPUGeneration gen);
};

// Logging helpers
#define GPU_DETECT_LOG(fmt, args...) \
    do { IOLog("ForceACL: [GPUDetector] " fmt "\n", ##args); } while(0)

#define GPU_DETECT_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [GPUDetector-VERBOSE] " fmt "\n", ##args); } while(0)

#endif // FORCEACL_GPUDETECTOR_HPP
