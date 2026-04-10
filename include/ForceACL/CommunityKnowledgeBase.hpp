/**
 * ForceACL - Community Knowledge Base Header
 * Real-world compatibility data from Hackintosh communities
 */

#ifndef FORCEACL_COMMUNITYKNOWLEDGEBASE_HPP
#define FORCEACL_COMMUNITYKNOWLEDGEBASE_HPP

#include <stdint.h>
#include <stddef.h>

// Forward declarations
struct CommunityReport;

// Functions to access community knowledge
const CommunityReport* findCommunityReports(uint16_t deviceId, size_t* count);
uint32_t getAverageSuccessRate(uint16_t deviceId);
uint32_t findBestCommunityPlatformId(uint16_t deviceId);

#endif // FORCEACL_COMMUNITYKNOWLEDGEBASE_HPP