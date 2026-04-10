/**
 * ForceACL - NVRAM Manager Implementation
 * Manages platform ID caching and AI state in NVRAM.
 */

#include <libkern/libkern.h>
#include <IOKit/IOKitKeys.h>
#include <IOKit/IORegistryEntry.h>
#include <libkern/OSData.h>
#include <libkern/OSString.h>

#include "ForceACL/NVRAMManager.hpp"

NVRAMManager::NVRAMManager()
    : m_cachedPlatformId(0)
    , m_cachedWorking(false)
    , m_lastAttemptedId(0)
    , m_triedIdsCount(0)
    , m_aiDecisionCount(0)
{
    for (size_t i = 0; i < sizeof(m_triedIds)/sizeof(m_triedIds[0]); i++) {
        m_triedIds[i] = 0;
    }
}

NVRAMManager::~NVRAMManager() {}

IORegistryEntry* NVRAMManager::getOptionsEntry() {
    return IORegistryEntry::fromPath("/options", gIOServicePlane);
}

OSObject* NVRAMManager::readNVRAMKey(const char* key) {
    if (!key) {
        return nullptr;
    }

    IORegistryEntry* options = getOptionsEntry();
    if (!options) {
        return nullptr;
    }

    OSObject* value = options->getProperty(key);
    options->release();
    return value;
}

bool NVRAMManager::writeNVRAMKey(const char* key, const void* data, size_t length) {
    if (!key || !data || length == 0) {
        return false;
    }

    IORegistryEntry* options = getOptionsEntry();
    if (!options) {
        return false;
    }

    OSData* osData = OSData::withBytes(data, length);
    if (!osData) {
        options->release();
        return false;
    }

    bool success = options->setProperty(key, osData);
    osData->release();
    options->release();
    return success;
}

uint32_t NVRAMManager::getCachedPlatformID() {
    return m_cachedPlatformId;
}

void NVRAMManager::setCachedPlatformID(uint32_t platformId) {
    m_cachedPlatformId = platformId;
    writeNVRAMKey("ffacl_cached_platform_id", &m_cachedPlatformId, sizeof(m_cachedPlatformId));
}

bool NVRAMManager::isCachedWorking() {
    return m_cachedWorking;
}

void NVRAMManager::setCachedWorking(bool working) {
    m_cachedWorking = working;
    uint32_t value = working ? 1 : 0;
    writeNVRAMKey("ffacl_cached_working", &value, sizeof(value));
}

uint32_t NVRAMManager::getLastAttemptedID() {
    return m_lastAttemptedId;
}

void NVRAMManager::setLastAttemptedID(uint32_t platformId) {
    m_lastAttemptedId = platformId;
    writeNVRAMKey("ffacl_last_attempted_id", &m_lastAttemptedId, sizeof(m_lastAttemptedId));
}

uint32_t NVRAMManager::getTriedIDsCount() {
    return m_triedIdsCount;
}

void NVRAMManager::addTriedID(uint32_t platformId) {
    if (m_triedIdsCount >= sizeof(m_triedIds)/sizeof(m_triedIds[0])) {
        return;
    }

    m_triedIds[m_triedIdsCount++] = platformId;
}

bool NVRAMManager::hasTriedID(uint32_t platformId) {
    for (uint32_t i = 0; i < m_triedIdsCount; i++) {
        if (m_triedIds[i] == platformId) {
            return true;
        }
    }
    return false;
}

void NVRAMManager::clearTriedIDs() {
    m_triedIdsCount = 0;
    memset(m_triedIds, 0, sizeof(m_triedIds));
}

uint32_t NVRAMManager::getAIDecisionCount() {
    return m_aiDecisionCount;
}

void NVRAMManager::incrementAIDecisionCount() {
    m_aiDecisionCount++;
}

void NVRAMManager::logNVRAMStatus() {
    NVRAM_LOG("Cached platform ID: 0x%08X", m_cachedPlatformId);
    NVRAM_LOG("Cached working: %u", m_cachedWorking);
    NVRAM_LOG("Tried IDs: %u", m_triedIdsCount);
    NVRAM_LOG("AI decisions: %u", m_aiDecisionCount);
}

void NVRAMManager::clearAllCache() {
    m_cachedPlatformId = 0;
    m_cachedWorking = false;
    m_lastAttemptedId = 0;
    clearTriedIDs();
    m_aiDecisionCount = 0;
}
