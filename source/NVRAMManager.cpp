/**
 * ForceACL - NVRAM Manager Implementation
 * Manages NVRAM operations for storing GPU settings
 */

#include <libkern/libkern.h>
#include <IOKit/IOService.h>
#include <IOKit/IORegistryEntry.h>

#include "ForceACL/NVRAMManager.hpp"

NVRAMManager::NVRAMManager()
    : m_nvramRoot(nullptr)
    , m_verboseLogging(false)
{
    NVRAM_LOG_VERBOSE("NVRAMManager initialized");

    // Get NVRAM root
    m_nvramRoot = IORegistryEntry::fromPath("/options", gIOServicePlane);
}

NVRAMManager::~NVRAMManager() {
    if (m_nvramRoot) {
        m_nvramRoot->release();
    }
    NVRAM_LOG_VERBOSE("NVRAMManager destroyed");
}

bool NVRAMManager::readProperty(const char* key, void* buffer, size_t* size) {
    if (!m_nvramRoot || !key || !buffer || !size) {
        return false;
    }

    OSObject* value = m_nvramRoot->getProperty(key);
    if (!value) {
        NVRAM_LOG_VERBOSE("NVRAM property not found: %s", key);
        return false;
    }

    OSData* data = OSDynamicCast(OSData, value);
    if (!data) {
        NVRAM_LOG_VERBOSE("NVRAM property not OSData: %s", key);
        return false;
    }

    size_t dataSize = data->getLength();
    if (dataSize > *size) {
        NVRAM_LOG_VERBOSE("NVRAM buffer too small for: %s", key);
        return false;
    }

    memcpy(buffer, data->getBytesNoCopy(), dataSize);
    *size = dataSize;

    NVRAM_LOG_VERBOSE("Read NVRAM property: %s", key);
    return true;
}

bool NVRAMManager::writeProperty(const char* key, const void* data, size_t size) {
    if (!m_nvramRoot || !key || !data) {
        return false;
    }

    OSData* osData = OSData::withBytes(data, size);
    if (!osData) {
        NVRAM_LOG_VERBOSE("Failed to create OSData for: %s", key);
        return false;
    }

    m_nvramRoot->setProperty(key, osData);
    osData->release();

    NVRAM_LOG_VERBOSE("Wrote NVRAM property: %s", key);
    return true;
}

bool NVRAMManager::deleteProperty(const char* key) {
    if (!m_nvramRoot || !key) {
        return false;
    }

    m_nvramRoot->removeProperty(key);

    NVRAM_LOG_VERBOSE("Deleted NVRAM property: %s", key);
    return true;
}

bool NVRAMManager::propertyExists(const char* key) {
    if (!m_nvramRoot || !key) {
        return false;
    }

    return m_nvramRoot->getProperty(key) != nullptr;
}

void NVRAMManager::setVerboseLogging(bool verbose) {
    m_verboseLogging = verbose;
}

bool NVRAMManager::storePlatformID(uint32_t platformId) {
    return writeProperty("ffacl-platform-id", &platformId, sizeof(platformId));
}

bool NVRAMManager::getStoredPlatformID(uint32_t* platformId) {
    size_t size = sizeof(uint32_t);
    return readProperty("ffacl-platform-id", platformId, &size);
}

bool NVRAMManager::storeBootSuccess(bool success) {
    uint32_t successFlag = success ? 1 : 0;
    return writeProperty("ffacl-boot-success", &successFlag, sizeof(successFlag));
}

bool NVRAMManager::getBootSuccess(bool* success) {
    uint32_t successFlag = 0;
    size_t size = sizeof(uint32_t);
    if (readProperty("ffacl-boot-success", &successFlag, &size)) {
        *success = (successFlag != 0);
        return true;
    }
    return false;
}