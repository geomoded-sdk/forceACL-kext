/**
 * ForceACL - Main Plugin Implementation
 * Core plugin logic and management
 */

#include <Lilu/kern_api.hpp>
#include <Lilu/kern_util.hpp>
#include <Lilu/kern_iokit.hpp>

#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOKitKeys.h>
#include <IOKit/graphics/IOGraphicsInterface.h>

#include <libkern/libkern.h>
#include <libkern/OSBase.h>
#include <mach/mach_time.h>

#include <cstdint>
#include <cstring>
#include <cstdio>

#include "ForceACL/ForceACL.hpp"
#include "ForceACL/PlatformDatabase.hpp"
#include "ForceACL/GPUDetector.hpp"
#include "ForceACL/GPUInjector.hpp"
#include "ForceACL/NVRAMManager.hpp"
#include "ForceACL/HookManager.hpp"
#include "ForceACL/ErrorHandler.hpp"
#include "ForceACL/AIDecisionEngine.hpp"

// Global instances
bool gForceACLVerbose = false;
bool gForceACLDebug = false;

// Plugin instance
ForceACLPlugin* ForceACLPlugin::m_instance = nullptr;

ForceACLPlugin* ForceACLPlugin::getInstance() {
    if (!m_instance) {
        m_instance = new ForceACLPlugin();
    }
    return m_instance;
}

ForceACLPlugin::ForceACLPlugin()
    : m_state(PluginState::Uninitialized)
    , m_mode(FFACLMode::Unset)
    , m_verboseBoot(false)
    , m_oclpDetected(false)
    , m_injectionComplete(false)
    , m_platformDB(nullptr)
    , m_gpuDetector(nullptr)
    , m_gpuInjector(nullptr)
    , m_aiEngine(nullptr)
    , m_nvramManager(nullptr)
    , m_hookManager(nullptr)
    , m_errorHandler(nullptr)
{
    FORCEACL_LOG("Constructor called");
}

ForceACLPlugin::~ForceACLPlugin() {
    stop();
    
    if (m_platformDB) delete m_platformDB;
    if (m_gpuDetector) delete m_gpuDetector;
    if (m_gpuInjector) delete m_gpuInjector;
    if (m_aiEngine) delete m_aiEngine;
    if (m_nvramManager) delete m_nvramManager;
    if (m_hookManager) delete m_hookManager;
    if (m_errorHandler) delete m_errorHandler;
}

bool ForceACLPlugin::init() {
    FORCEACL_LOG("=== ForceACL v%s Initializing ===", PLUGIN_VERSION);
    FORCEACL_LOG("==========================================");
    
    m_state = PluginState::Initializing;
    
    // Parse boot arguments
    parseBootArguments();
    
    // Detect OCLP
    detectOCLP();
    
    // Initialize components
    initializeComponents();
    
    // Hook IOServices
    hookIOServices();
    
    FORCEACL_LOG_VERBOSE("Initialization complete");
    
    return true;
}

bool ForceACLPlugin::start() {
    FORCEACL_LOG("Starting ForceACL plugin...");
    
    // Handle mode
    handleMode();
    
    // Early GPU detection and injection (BEFORE framebuffer initialization)
    performEarlyGPUInjection();
    
    // Process GPUs (fallback)
    processGPUs();
    
    m_state = PluginState::Active;
    
    FORCEACL_LOG("ForceACL started successfully");
    
    return true;
}

void ForceACLPlugin::stop() {
    if (m_state == PluginState::Terminated) {
        return;
    }
    
    FORCEACL_LOG("Stopping ForceACL plugin...");
    
    m_state = PluginState::Terminated;
    
    FORCEACL_LOG("ForceACL stopped");
}

void ForceACLPlugin::parseBootArguments() {
    FORCEACL_LOG_VERBOSE("Parsing boot arguments...");
    
    // Check for verbose boot
    if (PE_parse_boot_arg_num("v", nullptr)) {
        m_verboseBoot = true;
        gForceACLVerbose = true;
        FORCEACL_LOG_VERBOSE("Verbose boot detected");
    }
    
    // Check for debug mode
    if (PE_parse_boot_arg_num("ffacl_debug", nullptr)) {
        gForceACLDebug = true;
        FORCEACL_LOG_VERBOSE("Debug mode enabled");
    }
    
    // Check ffacl boot argument
    int modeValue = -1;
    if (PE_parse_boot_arg_num("ffacl", &modeValue)) {
        m_mode = static_cast<FFACLMode>(modeValue);
        FORCEACL_LOG("Boot argument ffacl = %d", modeValue);
        FORCEACL_LOG_VERBOSE("FFACL mode set to: %s", 
            m_mode == FFACLMode::Enabled ? "ENABLED" : 
            m_mode == FFACLMode::Disabled ? "DISABLED" : "UNSET");
    } else {
        FORCEACL_LOG_VERBOSE("Boot argument ffacl not set - using default");
        m_mode = FFACLMode::Unset;
    }
    
    // Additional boot arguments for fine-tuning
    char bootArgBuf[256];
    size_t bufSize = sizeof(bootArgBuf);
    
    if (PE_parse_boot_arg("ffacl_platform_id", bootArgBuf, &bufSize)) {
        if (bufSize >= sizeof(bootArgBuf)) {
            FORCEACL_LOG_ERROR("Boot argument ffacl_platform_id too long, truncated");
        }
        FORCEACL_LOG("Custom platform ID specified: %s", bootArgBuf);
    }
    
    if (PE_parse_boot_arg_num("ffacl_no_cache", nullptr)) {
        FORCEACL_LOG_VERBOSE("Cache disabled via boot argument");
    }
    
    if (PE_parse_boot_arg_num("ffacl_no_ai", nullptr)) {
        FORCEACL_LOG_VERBOSE("AI engine disabled via boot argument");
    }
    
    FORCEACL_LOG_VERBOSE("Boot argument parsing complete");
}

void ForceACLPlugin::detectOCLP() {
    FORCEACL_LOG_VERBOSE("Detecting OpenCore Legacy Patcher...");
    
    IORegistryEntry* options = IORegistryEntry::fromPath("/options", gIOServicePlane);
    if (!options) {
        FORCEACL_LOG_VERBOSE("Could not access /options NVRAM path");
        return;
    }
    
    // Check OCLP-Settings key
    OSObject* oclpSettings = options->getProperty("OCLP-Settings");
    if (oclpSettings) {
        m_oclpDetected = true;
        FORCEACL_LOG("*** OCLP DETECTED ***");
        FORCEACL_LOG("Found NVRAM key: OCLP-Settings");
        FORCEACL_LOG_VERBOSE("OCLP version: checking for version info...");
        
        // Try to get OCLP version if available
        OSObject* oclpVersion = options->getProperty("OCLP-Version");
        if (oclpVersion) {
            FORCEACL_LOG_VERBOSE("OCLP Version property found");
        }
    }
    
    // Check revpatch key
    if (!m_oclpDetected) {
        OSObject* revpatch = options->getProperty("revpatch");
        if (revpatch) {
            m_oclpDetected = true;
            FORCEACL_LOG("*** OCLP DETECTED (revpatch) ***");
            FORCEACL_LOG("Found NVRAM key: revpatch");
        }
    }
    
    // Check revblock key
    if (!m_oclpDetected) {
        OSObject* revblock = options->getProperty("revblock");
        if (revblock) {
            m_oclpDetected = true;
            FORCEACL_LOG("*** OCLP DETECTED (revblock) ***");
            FORCEACL_LOG("Found NVRAM key: revblock");
        }
    }
    
    // Check for OCLP-specific device properties
    if (!m_oclpDetected) {
        OSObject* oclpPatchUSB = options->getProperty("OCLP-Patch-USB");
        if (oclpPatchUSB) {
            FORCEACL_LOG_VERBOSE("OCLP patch mode detected (USB)");
        }
    }
    
    options->release();
    
    if (!m_oclpDetected) {
        FORCEACL_LOG_VERBOSE("OCLP not detected - proceeding normally");
    }
    
    FORCEACL_LOG_VERBOSE("OCLP detection complete");
}

void ForceACLPlugin::initializeComponents() {
    FORCEACL_LOG_VERBOSE("Initializing components...");
    
    // Initialize NVRAM Manager
    m_nvramManager = new NVRAMManager();
    if (!m_nvramManager) {
        FORCEACL_LOG_ERROR("Failed to initialize NVRAM Manager");
        return;
    }
    FORCEACL_LOG_VERBOSE("  - NVRAM Manager initialized");
    
    // Initialize GPU Detector
    m_gpuDetector = new GPUDetector();
    if (!m_gpuDetector) {
        FORCEACL_LOG_ERROR("Failed to initialize GPU Detector");
        return;
    }
    FORCEACL_LOG_VERBOSE("  - GPU Detector initialized");
    
    // Initialize Platform Database
    m_platformDB = new PlatformIDDatabase();
    if (!m_platformDB) {
        FORCEACL_LOG_ERROR("Failed to initialize Platform Database");
        return;
    }
    FORCEACL_LOG_VERBOSE("  - Platform Database initialized");
    FORCEACL_LOG_VERBOSE("    Database contains %lu platform IDs", 
        static_cast<unsigned long>(m_platformDB->getCount()));
    
    // Initialize AI Decision Engine
    m_aiEngine = new AIDecisionEngine();
    if (!m_aiEngine) {
        FORCEACL_LOG_ERROR("Failed to initialize AI Decision Engine");
        return;
    }
    FORCEACL_LOG_VERBOSE("  - AI Decision Engine initialized");
    
    // Initialize GPU Injector
    m_gpuInjector = new GPUInjector(m_platformDB);
    if (!m_gpuInjector) {
        FORCEACL_LOG_ERROR("Failed to initialize GPU Injector");
        return;
    }
    FORCEACL_LOG_VERBOSE("  - GPU Injector initialized");
    
    // Initialize Hook Manager
    m_hookManager = new HookManager();
    if (!m_hookManager) {
        FORCEACL_LOG_ERROR("Failed to initialize Hook Manager");
        return;
    }
    FORCEACL_LOG_VERBOSE("  - Hook Manager initialized");
    
    // Initialize Error Handler
    m_errorHandler = new ErrorHandler();
    if (!m_errorHandler) {
        FORCEACL_LOG_ERROR("Failed to initialize Error Handler");
        return;
    }
    FORCEACL_LOG_VERBOSE("  - Error Handler initialized");
    
    FORCEACL_LOG_VERBOSE("All components initialized");
}

void ForceACLPlugin::performEarlyGPUInjection() {
    FORCEACL_LOG("Performing early GPU injection (before framebuffer init)...");
    
    if (m_mode == FFACLMode::Disabled) {
        FORCEACL_LOG("Safe mode - skipping early GPU injection");
        return;
    }
    
    // Find Intel iGPU in IORegistry before driver initialization
    IOPCIDevice* igpuDevice = findIntelIGPU();
    if (!igpuDevice) {
        FORCEACL_LOG_VERBOSE("No Intel iGPU found in early detection");
        return;
    }
    
    uint16_t deviceID = igpuDevice->configRead16(2);
    FORCEACL_LOG("Early detection: Intel iGPU 0x%04X found", deviceID);
    
    // Decide platform ID using AI engine (ONE decision per boot)
    uint32_t platformId = decidePlatformIDForIGPU(deviceID);
    if (platformId == 0) {
        FORCEACL_LOG_ERROR("Failed to decide platform ID for iGPU");
        return;
    }
    
    FORCEACL_LOG("Selected platform ID for early injection: 0x%08X", platformId);
    
    // Inject properties directly into the device BEFORE driver loads
    bool success = injectPlatformIDEarly(igpuDevice, platformId);
    
    if (success) {
        FORCEACL_LOG("*** EARLY PLATFORM INJECTION SUCCESSFUL ***");
        FORCEACL_LOG("  Platform ID: 0x%08X injected before framebuffer init", platformId);
        
        // Cache the successful injection
        if (m_nvramManager) {
            m_nvramManager->setCachedPlatformID(platformId);
            m_nvramManager->setCachedWorking(true);
        }
        
        m_injectionComplete = true;
    } else {
        FORCEACL_LOG_ERROR("Early platform injection failed");
        
        // Delay boot to allow user intervention
        if (m_aiEngine && m_aiEngine->shouldDelayBoot()) {
            m_aiEngine->performBootDelay();
        }
    }
    
    igpuDevice->release();
}

IOPCIDevice* ForceACLPlugin::findIntelIGPU() {
    // Search for Intel iGPU in PCI space
    // Typical path: PciRoot(0x0)/Pci(0x2,0x0) or /PCI0@0/IGPU@2
    
    // Method 1: Use IORegistry to find PCI devices
    IORegistryIterator* iterator = IORegistryIterator::iterateOver(
        gIOServicePlane, kIORegistryIterateRecursively);
    
    if (!iterator) {
        FORCEACL_LOG_VERBOSE("Could not create IORegistry iterator");
        return nullptr;
    }
    
    IORegistryEntry* entry;
    while ((entry = iterator->getNextObject())) {
        IOPCIDevice* device = OSDynamicCast(IOPCIDevice, entry);
        if (device) {
            uint16_t vendorID = device->configRead16(0);
            uint16_t deviceID = device->configRead16(2);
            uint8_t classCode = device->configRead8(0x0B);
            uint8_t subclassCode = device->configRead8(0x0A);
            
            // Check for Intel GPU (vendor 0x8086, class 0x03)
            if (vendorID == 0x8086 && classCode == 0x03 && 
                (subclassCode == 0x00 || subclassCode == 0x02)) {
                
                FORCEACL_LOG_VERBOSE("Found Intel GPU: 0x%04X at %s", 
                    deviceID, device->getName());
                
                iterator->release();
                return device; // Retain count increased by OSDynamicCast
            }
        }
    }
    
    iterator->release();
    
    // Method 2: Try common PCI paths
    const char* commonPaths[] = {
        "/PCI0@0/IGPU@2",
        "/PCI0@0/IGPU@0,2",
        "/PciRoot(0x0)/Pci(0x2,0x0)",
        nullptr
    };
    
    for (int i = 0; commonPaths[i]; i++) {
        IORegistryEntry* entry = IORegistryEntry::fromPath(commonPaths[i], gIOServicePlane);
        if (entry) {
            IOPCIDevice* device = OSDynamicCast(IOPCIDevice, entry);
            if (device) {
                uint16_t vendorID = device->configRead16(0);
                if (vendorID == 0x8086) {
                    FORCEACL_LOG_VERBOSE("Found Intel iGPU at path: %s", commonPaths[i]);
                    return device;
                }
            }
            entry->release();
        }
    }
    
    FORCEACL_LOG_VERBOSE("Intel iGPU not found in early detection");
    return nullptr;
}

uint32_t ForceACLPlugin::decidePlatformIDForIGPU(uint16_t deviceID) {
    // Check cached working ID first
    if (m_nvramManager) {
        uint32_t cachedId = m_nvramManager->getCachedPlatformID();
        if (m_nvramManager->isCachedWorking() && cachedId != 0) {
            FORCEACL_LOG("Using cached working platform ID: 0x%08X", cachedId);
            return cachedId;
        }
    }
    
    // Use AI engine to make ONE decision
    if (m_aiEngine && m_platformDB) {
        uint32_t decidedId = m_aiEngine->decidePlatformID(deviceID, m_platformDB);
        if (decidedId != 0) {
            FORCEACL_LOG("AI Engine decided platform ID: 0x%08X", decidedId);
            return decidedId;
        }
    }
    
    // Fallback to community knowledge
    if (m_aiEngine) {
        uint32_t communityId = m_aiEngine->findBestCommunityPlatformId(deviceID);
        if (communityId != 0) {
            FORCEACL_LOG("Using community-recommended platform ID: 0x%08X", communityId);
            return communityId;
        }
    }
    
    // Last resort: generation-specific defaults
    GPUGeneration gen = m_gpuDetector ? m_gpuDetector->detectGeneration(deviceID) : GPUGeneration::Unknown;
    switch (gen) {
        case GPUGeneration::SandyBridge: return 0x00030010;
        case GPUGeneration::IvyBridge: return 0x01660009;
        case GPUGeneration::Haswell: return 0x0D220003;
        case GPUGeneration::Broadwell: return 0x16060000;
        case GPUGeneration::Skylake: return 0x19160000;
        case GPUGeneration::KabyLake: return 0x59160000;
        case GPUGeneration::CoffeeLake: return 0x3EA50000;
        case GPUGeneration::CometLake: return 0x9BC80003;
        case GPUGeneration::IceLake: return 0x8A530000;
        case GPUGeneration::TigerLake: return 0x9A500000;
        case GPUGeneration::RocketLake: return 0x4C610000;
        case GPUGeneration::AlderLakeS: return 0x46800000;
        case GPUGeneration::MeteorLake: return 0x7D450000;
        case GPUGeneration::LunarLake: return 0x64A00000;
        default: return 0x0D220003; // Haswell fallback
    }
}

bool ForceACLPlugin::injectPlatformIDEarly(IOPCIDevice* device, uint32_t platformId) {
    if (!device) {
        FORCEACL_LOG_ERROR("Early injection: null device");
        return false;
    }
    
    // Create platform-id data (big-endian)
    uint32_t platformIdBE = OSSwapHostToBigInt32(platformId);
    OSData* platformData = OSData::withBytes(&platformIdBE, sizeof(platformIdBE));
    if (!platformData) {
        FORCEACL_LOG_ERROR("Failed to create platform-id data");
        return false;
    }
    
    // Inject AAPL,ig-platform-id property
    bool success = device->setProperty("AAPL,ig-platform-id", platformData);
    
    if (success) {
        FORCEACL_LOG("Injected AAPL,ig-platform-id: 0x%08X", platformId);
        
        // Also inject other common properties for better compatibility
        injectAdditionalIGPUProperties(device, platformId);
    } else {
        FORCEACL_LOG_ERROR("Failed to inject AAPL,ig-platform-id");
    }
    
    platformData->release();
    return success;
}

void ForceACLPlugin::injectAdditionalIGPUProperties(IOPCIDevice* device, uint32_t platformId) {
    // Inject device-id if needed (for spoofing)
    uint32_t deviceId = 0;
    switch (platformId & 0x0000FFFF) { // Extract device ID from platform ID
        case 0x1916: deviceId = 0x19168086; break; // Skylake
        case 0x5916: deviceId = 0x59168086; break; // Kaby Lake
        case 0x3EA5: deviceId = 0x3EA58086; break; // Coffee Lake
        case 0x9BC8: deviceId = 0x9BC88086; break; // Comet Lake
        case 0x8A53: deviceId = 0x8A538086; break; // Ice Lake
        case 0x9A50: deviceId = 0x9A508086; break; // Tiger Lake
        case 0x4C61: deviceId = 0x4C618086; break; // Rocket Lake
        case 0x4680: deviceId = 0x46808086; break; // Alder Lake
        default: break;
    }
    
    if (deviceId != 0) {
        uint32_t deviceIdBE = OSSwapHostToBigInt32(deviceId);
        OSData* deviceData = OSData::withBytes(&deviceIdBE, sizeof(deviceIdBE));
        if (deviceData) {
            device->setProperty("device-id", deviceData);
            FORCEACL_LOG("Injected device-id: 0x%08X", deviceId);
            deviceData->release();
        }
    }
    
    // Inject AAPL,GfxYTile to enable graphics acceleration
    OSData* gfxTileData = OSData::withBytes("1", 1);
    device->setProperty("AAPL,GfxYTile", gfxTileData);
    gfxTileData->release();
    
    FORCEACL_LOG("Injected additional iGPU properties");
}

void ForceACLPlugin::hookIOServices() {
    FORCEACL_LOG_VERBOSE("Hooking IOServices...");
    
    if (m_mode == FFACLMode::Disabled) {
        FORCEACL_LOG_VERBOSE("Safe mode - not hooking GPU services");
        return;
    }
    
    // Hook into PCI device enumeration using Lilu
    if (LiluAPI::Version::atLeast(LiluAPI::Version::Parse(1, 6, 0))) {
        FORCEACL_LOG_VERBOSE("Using Lilu 1.6+ PCI hook");
        
        lilu.onPciDevice(IOPCIDevice::serviceName(), 
            [](IOPCIDevice* device) -> bool {
                return ForceACLPlugin::getInstance()->handlePCIDevice(device);
            });
        
        FORCEACL_LOG_VERBOSE("PCI device hook registered");
    } else {
        // Fallback for older Lilu versions
        FORCEACL_LOG_VERBOSE("Using legacy IOService hook");
        
        lilu.onIOService("IOPCIDevice", 
            [](IOService* service) -> bool {
                IOPCIDevice* device = OSDynamicCast(IOPCIDevice, service);
                if (device) {
                    return ForceACLPlugin::getInstance()->handlePCIDevice(device);
                }
                return false;
            });
        
        FORCEACL_LOG_VERBOSE("Legacy IOService hook registered");
    }
    
    // Also hook into IOService for additional detection
    lilu.onIOService("IOFramebuffer", 
        [](IOService* service) -> bool {
            FORCEACL_LOG_VERBOSE("IOFramebuffer detected: %s", 
                service->getName());
            return false;
        });
    
    FORCEACL_LOG_VERBOSE("IOServices hooks complete");
}

bool ForceACLPlugin::handlePCIDevice(IOPCIDevice* device) {
    if (!device) return false;
    
    auto* instance = getInstance();
    if (!instance) return false;
    
    // If early injection already completed, skip
    if (instance->m_injectionComplete) {
        FORCEACL_LOG_VERBOSE("Early injection already completed, skipping PCI hook");
        return false;
    }
    
    if (instance->m_mode == FFACLMode::Disabled) {
        FORCEACL_LOG_VERBOSE("Safe mode - ignoring PCI device");
        return false;
    }
    
    // Read PCI configuration
    uint16_t vendorID = device->configRead16(0);
    uint16_t deviceID = device->configRead16(2);
    
    FORCEACL_LOG_VERBOSE("PCI Device: vendor=0x%04X, device=0x%04X", 
        vendorID, deviceID);
    
    // Check if this is an Intel GPU
    if (vendorID != 0x8086) {
        FORCEACL_LOG_VERBOSE("Not an Intel device (vendor=0x%04X)", vendorID);
        return false;
    }
    
    // Check class code (0x03 = display controller, 0x00 = VGA)
    uint8_t classCode = device->configRead8(0x0B);
    uint8_t subclassCode = device->configRead8(0x0A);
    
    FORCEACL_LOG_VERBOSE("Class: 0x%02X, Subclass: 0x%02X", 
        classCode, subclassCode);
    
    bool isGPU = (classCode == 0x03 && (subclassCode == 0x00 || subclassCode == 0x02));
    
    if (!isGPU) {
        FORCEACL_LOG_VERBOSE("Not a GPU (class=0x%02X, subclass=0x%02X)", 
            classCode, subclassCode);
        return false;
    }
    
    // This is an Intel GPU - perform late injection as fallback
    FORCEACL_LOG("*** INTEL GPU DETECTED (Late Hook) ***");
    FORCEACL_LOG("  Vendor: 0x%04X (Intel)", vendorID);
    FORCEACL_LOG("  Device: 0x%04X", deviceID);
    FORCEACL_LOG("  Class: 0x%02X%02X00", classCode, subclassCode);
    
    // Detect GPU generation
    GPUGeneration gen = instance->m_gpuDetector->detectGeneration(deviceID);
    FORCEACL_LOG("  Generation: %s", 
        instance->m_gpuDetector->generationToString(gen));
    
    // Create GPU device info
    GPUDeviceInfo gpuInfo = {};
    gpuInfo.vendorId = vendorID;
    gpuInfo.deviceId = deviceID;
    gpuInfo.generation = gen;
    gpuInfo.detected = true;
    
    instance->m_detectedGPUs.push_back(gpuInfo);
    
    // Perform late injection as fallback (should not happen if early injection worked)
    instance->performLateGPUInjection(device);
    
    return true;
}

void ForceACLPlugin::performLateGPUInjection(IOPCIDevice* device) {
    FORCEACL_LOG("Performing late GPU injection (fallback - should not happen if early injection worked)");
    
    uint16_t deviceID = device->configRead16(2);
    
    // Try to get cached platform ID
    uint32_t platformId = 0;
    if (m_nvramManager) {
        platformId = m_nvramManager->getCachedPlatformID();
        if (m_nvramManager->isCachedWorking() && platformId != 0) {
            FORCEACL_LOG("Using cached platform ID for late injection: 0x%08X", platformId);
        }
    }
    
    if (platformId == 0) {
        // Fallback decision
        platformId = decidePlatformIDForIGPU(deviceID);
        FORCEACL_LOG("Late injection platform ID: 0x%08X", platformId);
    }
    
    // Attempt late injection
    bool success = injectPlatformIDEarly(device, platformId);
    
    if (success) {
        FORCEACL_LOG("*** LATE PLATFORM INJECTION SUCCESSFUL ***");
        m_injectionComplete = true;
    } else {
        FORCEACL_LOG_ERROR("Late platform injection failed - graphics may not work");
        
        // Delay boot as last resort
        if (m_aiEngine && m_aiEngine->shouldDelayBoot()) {
            m_aiEngine->performBootDelay();
        }
    }
}
            }
        }
    }
}

void ForceACLPlugin::injectPlatformProperties(IOPCIDevice* device, uint32_t platformId) {
    if (!device) {
        FORCEACL_LOG_ERROR("injectPlatformProperties: null device");
        return;
    }
    
    FORCEACL_LOG_VERBOSE("Injecting platform ID 0x%08X", platformId);
    
    // Inject AAPL,ig-platform-id
    OSData* platformData = OSData::withBytes(&platformId, sizeof(platformId));
    if (platformData) {
        device->setProperty("AAPL,ig-platform-id", platformData);
        platformData->release();
        FORCEACL_LOG_VERBOSE("  Injected: AAPL,ig-platform-id = 0x%08X", platformId);
    }
    
    // Inject additional properties
    const char* model = "Intel HD Graphics";
    OSData* modelData = OSData::withBytes(model, strlen(model) + 1);
    if (modelData) {
        device->setProperty("model", modelData);
        modelData->release();
        FORCEACL_LOG_VERBOSE("  Injected: model");
    }
    
    const char* slotName = "Built-in";
    OSData* slotData = OSData::withBytes(slotName, strlen(slotName) + 1);
    if (slotData) {
        device->setProperty("AAPL,slot-name", slotData);
        slotData->release();
        FORCEACL_LOG_VERBOSE("  Injected: AAPL,slot-name");
    }
    
    // Inject class-code override for proper matching
    uint32_t classCode = 0x030000;
    OSData* classData = OSData::withBytes(&classCode, sizeof(classCode));
    if (classData) {
        device->setProperty("class-code", classData);
        classData->release();
        FORCEACL_LOG_VERBOSE("  Injected: class-code");
    }
    
    // Inject device-type
    const char* deviceType = "display";
    OSData* typeData = OSData::withBytes(deviceType, strlen(deviceType) + 1);
    if (typeData) {
        device->setProperty("device_type", typeData);
        typeData->release();
        FORCEACL_LOG_VERBOSE("  Injected: device_type");
    }
}

void ForceACLPlugin::processGPUs() {
    FORCEACL_LOG_VERBOSE("Processing detected GPUs...");
    
    for (size_t i = 0; i < m_detectedGPUs.size(); i++) {
        GPUDeviceInfo& gpu = m_detectedGPUs[i];
        
        FORCEACL_LOG_VERBOSE("GPU %zu: device=0x%04X, generation=%s", 
            i, gpu.deviceId, 
            m_gpuDetector ? m_gpuDetector->generationToString(gpu.generation) : "unknown");
    }
    
    FORCEACL_LOG_VERBOSE("GPU processing complete");
}

void ForceACLPlugin::handleMode() {
    FORCEACL_LOG_VERBOSE("Handling mode: %s", 
        m_mode == FFACLMode::Enabled ? "ENABLED" : 
        m_mode == FFACLMode::Disabled ? "DISABLED" : "UNSET");
    
    if (m_mode == FFACLMode::Unset) {
        if (m_oclpDetected) {
            FORCEACL_LOG("******************************************");
            FORCEACL_LOG("*** WARNING: OCLP DETECTED! ***");
            FORCEACL_LOG("******************************************");
            FORCEACL_LOG("OCLP was detected without override mode");
            FORCEACL_LOG("Use ffacl=1 to force override");
            FORCEACL_LOG("Use ffacl=0 for safe mode (skip injection)");
            FORCEACL_LOG("******************************************");
            
            if (m_verboseBoot) {
                FORCEACL_LOG("*** VERBOSE BOOT - TRIGGERING PANIC ***");
                panic("ForceACL: OCLP detected without override. Use ffacl=1 or ffacl=0.");
            } else {
                FORCEACL_LOG("Non-verbose boot - continuing without panic");
            }
        } else {
            FORCEACL_LOG_VERBOSE("OCLP not detected - proceeding normally");
        }
    } else if (m_mode == FFACLMode::Disabled) {
        FORCEACL_LOG("*** SAFE MODE (ffacl=0) ***");
        FORCEACL_LOG("GPU injection is DISABLED");
        FORCEACL_LOG("Boot will continue normally");
    } else if (m_mode == FFACLMode::Enabled) {
        FORCEACL_LOG("*** OVERRIDE MODE (ffacl=1) ***");
        FORCEACL_LOG("Forcing GPU injection regardless of OCLP");
        FORCEACL_LOG("Proceeding with platform ID injection");
    }
}
