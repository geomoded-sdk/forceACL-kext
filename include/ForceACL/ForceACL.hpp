/**
 * ForceACL - Core Plugin Header
 * Main plugin entry point and management
 */

#ifndef FORCEACL_PLUGIN_HPP
#define FORCEACL_PLUGIN_HPP

#include <libkern/libkern.h>
#include <libkern/OSBase.h>
#include <mach/mach_time.h>
#include <mach/vm_map.h>
#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOKitKeys.h>

#include <sys/types.h>

#ifndef PLUGIN_VERSION
#define PLUGIN_VERSION "2.0.0"
#endif

// Debug logging macros
#define FORCEACL_LOG(fmt, args...) \
    do { IOLog("ForceACL: " fmt "\n", ##args); } while(0)

#define FORCEACL_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [VERBOSE] " fmt "\n", ##args); } while(0)

#define FORCEACL_LOG_ERROR(fmt, args...) \
    do { IOLog("ForceACL: [ERROR] " fmt "\n", ##args); } while(0)

#define FORCEACL_LOG_DEBUG(fmt, args...) \
    do { if (gForceACLDebug) IOLog("ForceACL: [DEBUG] " fmt "\n", ##args); } while(0)

// Global verbose flag
extern bool gForceACLVerbose;
extern bool gForceACLDebug;

// Forward declarations
class ForceACLPlugin;
class PlatformIDDatabase;
class GPUDetector;
class GPUInjector;
class AIDecisionEngine;
class NVRAMManager;
class HookManager;
class ErrorHandler;
class GPUPropertiesManager;
class ConnectorFixer;
class FramebufferPatcher;

// Plugin state
enum class PluginState {
    Uninitialized,
    Initializing,
    Active,
    Suspended,
    Terminated
};

// FFACL Mode
enum class FFACLMode {
    Unset = -1,
    Disabled = 0,
    Enabled = 1,
    BrutalModeCompatibility = 4  // ffacl=4 - Aggressive GPU forcing mode
};

// GPU Generation detection
enum class GPUGeneration {
    Unknown = 0,
    SandyBridge = 0x01,
    IvyBridge = 0x02,
    Haswell = 0x03,
    Broadwell = 0x04,
    Skylake = 0x05,
    KabyLake = 0x06,
    CoffeeLake = 0x07,
    CometLake = 0x08,
    IceLake = 0x09,
    TigerLake = 0x0A,
    RocketLake = 0x0B,
    AlderLakeS = 0x0C,
    AlderLakeP = 0x0D,
    MeteorLake = 0x0E,
    LunarLake = 0x0F
};

// Platform ID structure
struct PlatformIDInfo {
    uint32_t platformId;
    GPUGeneration generation;
    const char* name;
    bool tested;
    bool working;
    uint32_t successCount;
    uint32_t failCount;
};

// GPU Device information
struct GPUDeviceInfo {
    uint16_t vendorId;
    uint16_t deviceId;
    uint16_t subsystemId;
    uint16_t revisionId;
    GPUGeneration generation;
    char classCode[16];
    char name[64];
    bool detected;
    bool accelerated;
};

// Simple vector for kernel use
template<typename T>
class ForceACLVector {
    T* data_;
    size_t size_;
    size_t capacity_;
public:
    ForceACLVector() : data_(nullptr), size_(0), capacity_(0) {}
    ~ForceACLVector() { 
        if (data_) delete[] data_; 
    }
    
    void push_back(const T& val) {
        if (size_ >= capacity_) {
            size_t newCap = capacity_ == 0 ? 4 : capacity_ * 2;
            T* newData = new T[newCap];
            for (size_t i = 0; i < size_; i++) newData[i] = data_[i];
            if (data_) delete[] data_;
            data_ = newData;
            capacity_ = newCap;
        }
        data_[size_++] = val;
    }
    
    size_t size() const { return size_; }
    T& operator[](size_t idx) { return data_[idx]; }
    const T& operator[](size_t idx) const { return data_[idx]; }
    T* data() { return data_; }
};

// Error codes for GPU acceleration
#define ERROR_GPU_NOT_DETECTED      0xE0010001
#define ERROR_GPU_NO_VENDOR        0xE0020002
#define ERROR_GPU_NO_DEVICE        0xE0030003
#define ERROR_GPU_NOT_SUPPORTED    0xE0040004
#define ERROR_GPU_INJECTION_FAILED 0xE0050005
#define ERROR_GPU_NO_PLATFORM_ID   0xE0060006
#define ERROR_GPU_NO_ACCELERATION  0xE0070007
#define ERROR_GPU_AMFI_FAILED      0xE0080008
#define ERROR_GPU_CONTEXT_FAILED   0xE0090009
#define ERROR_GPUFramebuffer_INIT 0xE00A000A

// Main plugin class
class ForceACLPlugin {
public:
    static ForceACLPlugin* getInstance();
    
    ForceACLPlugin();
    ~ForceACLPlugin();
    
    bool init();
    bool start();
    void stop();
    
    void processGPUDevice(IOPCIDevice* device);
    void injectPlatformProperties(IOPCIDevice* device, uint32_t platformId);
    
private:
    static ForceACLPlugin* m_instance;
    PluginState m_state;
    
    FFACLMode m_mode;
    bool m_verboseBoot;
    bool m_oclpDetected;
    bool m_injectionComplete;
    
    ForceACLVector<GPUDeviceInfo> m_detectedGPUs;
    ForceACLVector<PlatformIDInfo> m_platformIDs;
    
    PlatformIDDatabase* m_platformDB;
    GPUDetector* m_gpuDetector;
    GPUInjector* m_gpuInjector;
    AIDecisionEngine* m_aiEngine;
    NVRAMManager* m_nvramManager;
    HookManager* m_hookManager;
    ErrorHandler* m_errorHandler;
    
    // WhateverGreen-compatible modules
    GPUPropertiesManager* m_gpuPropertiesManager;
    ConnectorFixer* m_connectorFixer;
    FramebufferPatcher* m_framebufferPatcher;
    
    // Brutal Mode state
    bool m_brutalModeActive;
    bool m_brutalModeFailed;
    int m_brutalRetryCount;
    
    void parseBootArguments();
    void detectOCLP();
    void initializeComponents();
    void hookIOServices();
    void processGPUs();
    void handleMode();
    
    // ========== BRUTAL MODE METHODS ==========
    void handleGPUBrutal(IOPCIDevice* device);
    void spoofToKnownWorkingGPU(IOPCIDevice* device);
    uint32_t forceUniversalPlatform();
    void injectAllProperties(IOPCIDevice* device, uint32_t platformId);
    void setupBrutalModeHooks();
    void applyAllPatches(IOPCIDevice* device);
    void emulateEverything();
    bool autoRecover(IOPCIDevice* device);
    void checkBrutalModeBootFail();
    void saveBrutalModeBootFail();
    void forceFramebufferInit(IOService* framebuffer);
    void forceAcceleratorInit(IOService* accelerator);
    
    IOPCIDevice* findIntelIGPU();
    uint32_t decidePlatformIDForIGPU(uint16_t deviceID);
    bool injectPlatformIDEarly(IOPCIDevice* device, uint32_t platformId);
    void injectAdditionalIGPUProperties(IOPCIDevice* device, uint32_t platformId);
    bool handlePCIDevice(IOPCIDevice* device);
    void performEarlyGPUInjection();
    void performLateGPUInjection(IOPCIDevice* device);
    
    // Brutal Mode internal methods
    void disableGPUBlacklist();
    void forceAcceleration();
    void overrideDriverChecks();
};

#endif // FORCEACL_PLUGIN_HPP
