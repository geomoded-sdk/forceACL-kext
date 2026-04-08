/**
 * ForceACL - Hook Manager Module
 * Manages IOService hooks and callbacks
 */

#ifndef FORCEACL_HOOKMANAGER_HPP
#define FORCEACL_HOOKMANAGER_HPP

#include <stdint.h>

class HookManager {
public:
    HookManager();
    ~HookManager();
    
    bool registerPCIHook();
    bool registerFramebufferHook();
    bool registerServiceHooks();
    bool unregisterAllHooks();
    
    void setVerboseLogging(bool verbose);
    
private:
    bool m_pciHookRegistered;
    bool m_framebufferHookRegistered;
    bool m_verboseLogging;
    
    static bool pciDeviceCallback(void* device);
    static bool framebufferCallback(void* service);
    static bool serviceMatchingCallback(void* service);
};

// Logging macros
#define HOOK_LOG(fmt, args...) \
    do { IOLog("ForceACL: [Hook] " fmt "\n", ##args); } while(0)

#define HOOK_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [Hook-VERBOSE] " fmt "\n", ##args); } while(0)

#endif // FORCEACL_HOOKMANAGER_HPP
