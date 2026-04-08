/**
 * ForceACL - Community Reports Template
 * Template for adding new community compatibility reports
 *
 * To add a new report, copy this structure and fill in the details:
 *
 * struct CommunityReport {
 *     uint32_t platformId;      // The platform ID used
 *     uint16_t deviceId;        // GPU device ID (e.g., 0x3EA5 for UHD 630)
 *     const char* gpuModel;     // GPU model name
 *     const char* macModel;     // Mac model it was tested on
 *     const char* osVersion;    // macOS version (e.g., "12.6.1")
 *     uint32_t successRate;     // Success rate 0-100 (based on stability)
 *     const char* issues;       // Any issues encountered
 *     const char* source;       // Where the report came from
 *     const char* reporter;     // Username or identifier
 *     uint32_t reportDate;      // Date in YYYYMMDD format
 * };
 *
 * Example additions:
 */

// Example: New report for UHD 630 on Z390 motherboard
/*
{0x3E9B0007, 0x3E9B, "UHD Graphics 630", "iMac19,1", "12.6.1", 96,
 "Perfect acceleration, HDMI audio works", "Reddit r/hackintosh", "user123", 20230115}
*/

// Example: Report for Iris Xe on Tiger Lake
/*
{0x9A500000, 0x9A50, "Iris Xe Graphics G7", "MacBookPro17,1", "12.6.1", 89,
 "Requires device-id spoof to 0x591B", "Olarila", "tiger_user", 20230220}
*/

// Example: Alder Lake P-core + E-core setup
/*
{0x46A00000, 0x46A0, "Iris Xe Graphics G7", "MacBookPro18,2", "13.2.1", 92,
 "Proper P+E core mapping required", "TonyMac", "alder_expert", 20230510}
*/

/*
 * Adding reports:
 * 1. Test the platform ID on your hardware
 * 2. Note any issues, success rate, and macOS version
 * 3. Add to the appropriate generation array in CommunityKnowledgeBase.cpp
 * 4. Update the count in communityReportCounts array
 * 5. Test the build and verify the AI engine uses the new data
 *
 * Sources for reports:
 * - Reddit r/hackintosh
 * - Olarila forums
 * - TonyMac x99
 * - InsanelyMac
 * - WhateverGreen issues
 * - OpenCore Legacy Patcher discussions
 */