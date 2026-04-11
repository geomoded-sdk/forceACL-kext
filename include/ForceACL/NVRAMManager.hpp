/**
 * ForceACL - NVRAM Manager Module
 * Manages platform ID caching in NVRAM
 */

#ifndef FORCEACL_NVRAMMANAGER_HPP
#define FORCEACL_NVRAMMANAGER_HPP

#include <stdint.h>
#include <stddef.h>
#include <IOKit/IOService.h>

class OSObject;

extern bool gForceACLVerbose;

class NVRAMManager {
public:
    NVRAMManager();
    ~NVRAMManager();
    
    // Platform ID caching
    uint32_t getCachedPlatformID();
    void setCachedPlatformID(uint32_t platformId);
    bool isCachedWorking();
    void setCachedWorking(bool working);
    
    // Last attempted tracking
    uint32_t getLastAttemptedID();
    void setLastAttemptedID(uint32_t platformId);
    
    // Tried IDs tracking (for avoiding repeats)
    uint32_t getTriedIDsCount();
    void addTriedID(uint32_t platformId);
    bool hasTriedID(uint32_t platformId);
    void clearTriedIDs();
    
    // AI decision cache
    uint32_t getAIDecisionCount();
    void incrementAIDecisionCount();
    
    // Status
    void logNVRAMStatus();
    void clearAllCache();
    
private:
    uint32_t m_cachedPlatformId;
    bool m_cachedWorking;
    uint32_t m_lastAttemptedId;
    uint32_t m_triedIds[50];
    uint32_t m_triedIdsCount;
    uint32_t m_aiDecisionCount;
    
    IORegistryEntry* getOptionsEntry();
    OSObject* readNVRAMKey(const char* key);
    bool writeNVRAMKey(const char* key, const void* data, size_t length);
};

// Logging macros
#define NVRAM_LOG(fmt, args...) \
    do { IOLog("ForceACL: [NVRAM] " fmt "\n", ##args); } while(0)

#define NVRAM_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [NVRAM-VERBOSE] " fmt "\n", ##args); } while(0)

#endif // FORCEACL_NVRAMMANAGER_HPP
