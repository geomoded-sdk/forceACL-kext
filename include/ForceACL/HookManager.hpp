/**
 * ForceACL - Hook Manager Module
 * Manages IOService hooks and callbacks
 */

#ifndef FORCEACL_HOOKMANAGER_HPP
#define FORCEACL_HOOKMANAGER_HPP

#include <stdint.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

extern bool gForceACLVerbose;

class FramebufferPatcher;

class HookManager {
public:
    HookManager(FramebufferPatcher* patcher = nullptr);
    ~HookManager();
    
    bool registerPCIHook();
    bool registerFramebufferHook();
    bool registerServiceHooks();
    bool unregisterAllHooks();
    
    void setVerboseLogging(bool verbose);
    
    // Public callback methods for external access
    bool pciDeviceCallback(void* device);
    bool framebufferCallback(void* service);
    bool serviceMatchingCallback(void* service);
    
private:
    bool m_pciHookRegistered;
    bool m_framebufferHookRegistered;
    bool m_verboseLogging;
    FramebufferPatcher* m_fbPatcher;
    
    // Private helper methods
    void applyFramebufferPatches(IOService* framebuffer);
    void applyFakeVRAMOverride(IOService* framebuffer);
    void applyDVMTFix(IOService* framebuffer);
    void applyConnectorsCorrection(IOService* framebuffer);
    void applyAutomaticFramebufferPatch(IOService* framebuffer);
};

// Logging macros
#define HOOK_LOG(fmt, args...) \
    do { IOLog("ForceACL: [Hook] " fmt "\n", ##args); } while(0)

#define HOOK_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [Hook-VERBOSE] " fmt "\n", ##args); } while(0)

#endif // FORCEACL_HOOKMANAGER_HPP
