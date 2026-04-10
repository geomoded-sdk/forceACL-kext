/**
 * ForceACL - Hook Manager Implementation
 * Manages Lilu hook callbacks and framebuffer patch interaction.
 */

#include <libkern/libkern.h>
#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IORegistryEntry.h>
#include <IOKit/IOKitKeys.h>

#include "ForceACL/HookManager.hpp"
#include "ForceACL/FramebufferPatcher.hpp"

HookManager::HookManager(FramebufferPatcher* patcher)
    : m_pciHookRegistered(false)
    , m_framebufferHookRegistered(false)
    , m_verboseLogging(false)
    , m_fbPatcher(patcher)
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

    HOOK_LOG("Registering PCI device hook");
    m_pciHookRegistered = true;
    return true;
}

bool HookManager::registerFramebufferHook() {
    if (m_framebufferHookRegistered) {
        HOOK_LOG_VERBOSE("Framebuffer hook already registered");
        return true;
    }

    HOOK_LOG("Registering framebuffer hook");
    m_framebufferHookRegistered = true;
    return true;
}

bool HookManager::registerServiceHooks() {
    HOOK_LOG("Registering service matching hooks");
    return true;
}

bool HookManager::unregisterAllHooks() {
    HOOK_LOG_VERBOSE("Unregistering hooks");
    m_pciHookRegistered = false;
    m_framebufferHookRegistered = false;
    return true;
}

void HookManager::setVerboseLogging(bool verbose) {
    m_verboseLogging = verbose;
}

bool HookManager::pciDeviceCallback(void* device) {
    if (!device) {
        return false;
    }

    IOPCIDevice* pciDevice = static_cast<IOPCIDevice*>(device);
    if (!pciDevice) {
        return false;
    }

    uint16_t vendorId = pciDevice->configRead16(0);
    uint16_t deviceId = pciDevice->configRead16(2);

    if (vendorId != 0x8086) {
        return false;
    }

    HOOK_LOG("Intel GPU PCI device seen: 0x%04X", deviceId);

    if (m_fbPatcher) {
        m_fbPatcher->injectFramebufferProperties(pciDevice, 0);
    }

    return true;
}

bool HookManager::framebufferCallback(void* service) {
    if (!service) {
        return false;
    }

    IOService* ioService = static_cast<IOService*>(service);
    if (!ioService) {
        return false;
    }

    const char* className = ioService->getMetaClass()->getClassName();
    if (strcmp(className, "AppleIntelFramebuffer") != 0) {
        return false;
    }

    HOOK_LOG("AppleIntelFramebuffer service detected: %s", ioService->getName());

    if (m_fbPatcher) {
        m_fbPatcher->patchFramebufferTables(ioService);
        m_fbPatcher->dumpFramebufferInfo(ioService);
    }

    return false;
}

bool HookManager::serviceMatchingCallback(void* service) {
    if (!service) {
        return false;
    }

    IOService* ioService = static_cast<IOService*>(service);
    if (!ioService) {
        return false;
    }

    HOOK_LOG_VERBOSE("Service matching callback for %s", ioService->getMetaClass()->getClassName());
    return false;
}
