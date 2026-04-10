/**
 * ForceACL - GPU Detector Module
 * Detects Intel GPU generations and device IDs
 */

#ifndef FORCEACL_GPUDETECTOR_HPP
#define FORCEACL_GPUDETECTOR_HPP

#include <IOKit/pci/IOPCIDevice.h>
#include <libkern/libkern.h>
#include <libkern/OSBase.h>

#include "ForceACL/ForceACL.hpp"

struct DeviceIDMapping {
    uint16_t deviceId;
    GPUGeneration generation;
    const char* shortName;
    const char* fullName;
    const char* marketingName;
};

class GPUDetector {
public:
    static const DeviceIDMapping m_deviceMappings[];
    static const size_t m_deviceMappingCount;
    
    GPUDetector();
    ~GPUDetector();
    
    GPUGeneration detectGeneration(uint16_t deviceId);
    const char* generationToString(GPUGeneration gen);
    bool isIntelGPU(uint16_t vendorId);
    bool isIntel(uint16_t vendorId);
    uint16_t getSubVendorId(IOPCIDevice* device);
    uint16_t getSubDeviceId(IOPCIDevice* device);
    uint8_t getRevisionId(IOPCIDevice* device);
    void dumpPCIDevice(IOPCIDevice* device);
    
    const char* getMarketingName(uint16_t deviceId);
    bool isSupported(uint16_t deviceId);
    bool isHaswell(uint16_t deviceId);
    GPUGeneration matchDeviceId(uint16_t deviceId, uint16_t matchId);
    const char* lookupMarketingName(GPUGeneration gen);
    bool isGraphicsAccelerationActive(IOPCIDevice* device);
    bool verifyQE_CI_Enabled(IOPCIDevice* device);
    bool testGraphicsMemoryAccess(IOPCIDevice* device);
    bool isGraphicsDeviceReady(IOPCIDevice* device);
    uint32_t getGraphicsMemorySize(IOPCIDevice* device);
    bool checkIOGraphicsAcceleration(IOService* device);
    
    const char* getDeviceName(uint16_t deviceId);
    bool checkIOGraphicsAcceleration(IOPCIDevice* device);
    
private:
    static const DeviceIDMapping* findMapping(uint16_t deviceId);
};

#define GPU_DETECT_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [GPU-DETECT] " fmt "\n", ##args); } while(0)

#define GPU_DETECT_LOG(fmt, args...) \
    do { IOLog("ForceACL: [GPU-DETECT] " fmt "\n", ##args); } while(0)

#define GPU_DETECT_LOG_ERROR(fmt, args...) \
    do { IOLog("ForceACL: [GPU-DETECT-ERROR] " fmt "\n", ##args); } while(0)

#define GPU_DETECT_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [GPU-DETECT-VERBOSE] " fmt "\n", ##args); } while(0)

#endif
