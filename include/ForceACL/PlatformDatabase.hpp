/**
 * ForceACL - Platform ID Database
 * Contains 500+ platform IDs for Intel GPUs
 */

#ifndef FORCEACL_PLATFORM_DATABASE_HPP
#define FORCEACL_PLATFORM_DATABASE_HPP

#include <stdint.h>
#include <stddef.h>

enum class GPUGeneration;

struct PlatformIDEntry {
    uint32_t id = 0;
    uint16_t deviceId = 0;
    const char* generation = nullptr;
    const char* name = nullptr;
    uint32_t flags = 0;
    uint32_t confidence = 0;
    uint32_t communityVerified = 0;
    uint32_t successCount = 0;
    uint32_t failCount = 0;
};

#define PLATFORM_FLAG_HASWELL    0x00000001
#define PLATFORM_FLAG_BROADWELL  0x00000002
#define PLATFORM_FLAG_SKYLAKE    0x00000004
#define PLATFORM_FLAG_KABYLAKE  0x00000008
#define PLATFORM_FLAG_COFFEELAKE 0x00000010
#define PLATFORM_FLAG_COMETLAKE  0x00000020
#define PLATFORM_FLAG_ICELAKE    0x00000040
#define PLATFORM_FLAG_TIGERLAKE  0x00000080
#define PLATFORM_FLAG_ROCKETLAKE 0x00000100
#define PLATFORM_FLAG_ALDERLAKE  0x00000200
#define PLATFORM_FLAG_METEORLAKE 0x00000400
#define PLATFORM_FLAG_LUNARLAKE  0x00000800
#define PLATFORM_FLAG_IVYBRIDGE  0x00001000
#define PLATFORM_FLAG_SANDYBRIDGE 0x00002000
#define PLATFORM_FLAG_TESTED    0x10000000
#define PLATFORM_FLAG_WORKING   0x20000000

class PlatformIDDatabase {
public:
    PlatformIDDatabase();
    ~PlatformIDDatabase();
    
    size_t getCount() const;
    const PlatformIDEntry* findPlatformID(uint32_t id) const;
    const PlatformIDEntry* findPlatformIDByDevice(uint16_t deviceId) const;
    const PlatformIDEntry* getNextPlatformID(const PlatformIDEntry* current) const;
    const PlatformIDEntry* findBestPlatformID(uint16_t deviceId, GPUGeneration gen) const;
    const PlatformIDEntry** getPlatformIDsForDevice(uint16_t deviceId, size_t* count) const;
    
private:
    size_t m_count;
    const PlatformIDEntry* m_database;
};

#endif
