/**
 * ForceACL - Platform ID Database
 * Contains 500+ platform IDs for Intel GPUs
 */

#ifndef FORCEACL_PLATFORM_DATABASE_HPP
#define FORCEACL_PLATFORM_DATABASE_HPP

#include <stdint.h>
#include <stddef.h>

// Platform ID structure
struct PlatformIDEntry {
    uint32_t id;
    uint16_t deviceId;
    const char* generation;
    const char* name;
    uint32_t flags;
};

// Platform ID flags
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

// Database functions
extern "C" {

const PlatformIDEntry* forceACL_getPlatformIDDatabase(size_t* count);
const PlatformIDEntry* forceACL_findPlatformID(uint32_t id);
const PlatformIDEntry* forceACL_findPlatformIDByDevice(uint16_t deviceId);
const PlatformIDEntry* forceACL_getNextPlatformID(const PlatformIDEntry* current);
const PlatformIDEntry* forceACL_findBestPlatformID(uint16_t deviceId, const char* generation);

} // extern "C"

#endif // FORCEACL_PLATFORM_DATABASE_HPP
