/**
 * ForceACL - Connector Fixer Module
 * Based on WhateverGreen connector patching logic
 * Fixes connector types, bus IDs, pipes, and display issues
 */

#ifndef FORCEACL_CONNECTORFIXER_HPP
#define FORCEACL_CONNECTORFIXER_HPP

#include <stdint.h>
#include <stddef.h>

// Connector type definitions (from WhateverGreen framebuffer)
#define ConnectorTypeUnknown  0x0000
#define ConnectorTypeLVDS     0x0100
#define ConnectorTypeVGA      0x0010
#define ConnectorTypeDisplay  0x0020
#define ConnectorTypeHDMI     0x0800
#define ConnectorTypeDP       0x0400
#define ConnectorTypeDVI      0x0200
#define ConnectorTypeLANECOUNT 0x000F

// Connector flags
#define ConnectorFlagEnabled  0x00000080
#define ConnectorFlagHotPlug  0x00000004

/**
 * Connector data structure (generic framebuffer)
 */
struct ConnectorInfo {
    uint8_t index;
    uint8_t bus_id;
    uint32_t pipe;
    uint32_t type;
    uint32_t flags;
};

/**
 * ConnectorFixer class
 * Fixes connector issues like:
 * - DP to HDMI conversion
 * - Bus ID corrections
 * - Pipe mapping fixes
 * - Port count adjustments
 */
class ConnectorFixer {
public:
    ConnectorFixer();
    ~ConnectorFixer();
    
    /**
     * Fix DP connector as HDMI (common on unsupported systems)
     * Converts 0x0400 (DP) to 0x0800 (HDMI)
     */
    bool fixDPtoHDMI(ConnectorInfo* connectors, size_t connectorCount);
    
    /**
     * Fix bus ID for connectors (typically 0x04, 0x05, 0x06, 0x07)
     * @param connectors Connector array
     * @param connectorCount Number of connectors
     * @param baseBusId Starting bus ID (usually 0x04 for iGPU)
     */
    bool fixBusIDs(ConnectorInfo* connectors, size_t connectorCount, uint8_t baseBusId);
    
    /**
     * Fix pipe mapping (0, 1, 2)
     * Ensures proper pipe assignment for each connector
     */
    bool fixPipes(ConnectorInfo* connectors, size_t connectorCount);
    
    /**
     * Disable unused connectors
     * Sets type to 0 for unused ports
     */
    bool disableUnusedConnectors(ConnectorInfo* connectors, size_t connectorCount, uint8_t activeCount);
    
    /**
     * Fix LVDS connector for laptops
     * Ensures LVDS has proper settings
     */
    bool fixLVDSConnector(ConnectorInfo* connectors, size_t connectorCount);
    
    /**
     * Apply common fixes for a generation
     */
    bool applyGenerationFixes(ConnectorInfo* connectors, size_t connectorCount, 
                             uint32_t platformId);
    
    /**
     * Fix connector array from framebuffer data
     * @param fbData Framebuffer binary data
     * @param maxSize Max search size
     * @param generation Platform generation indicator
     */
    bool fixConnectorsInFramebuffer(uint8_t* fbData, size_t maxSize, uint32_t platformId);
    
    /**
     * Validate connector array
     */
    bool validateConnectors(const ConnectorInfo* connectors, size_t connectorCount);
    
    /**
     * Set verbose logging
     */
    void setVerboseLogging(bool verbose);
    
private:
    bool m_verboseLogging;
    
    /**
     * Detect connector type from port characteristics
     */
    uint32_t detectConnectorType(const ConnectorInfo* connector, uint8_t index);
    
    /**
     * Check if connector is active/enabled
     */
    bool isConnectorActive(const ConnectorInfo* connector);
    
    /**
     * Find LVDS connector index (for laptops)
     */
    int findLVDSConnector(const ConnectorInfo* connectors, size_t connectorCount);
    
    /**
     * Count active connectors
     */
    uint8_t countActiveConnectors(const ConnectorInfo* connectors, size_t connectorCount);
};

// Logging macros
#define CONN_FIX_LOG(fmt, args...) \
    do { IOLog("ForceACL: [ConnFixer] " fmt "\n", ##args); } while(0)

#define CONN_FIX_LOG_VERBOSE(fmt, args...) \
    do { if (gForceACLVerbose) IOLog("ForceACL: [ConnFixer-VERBOSE] " fmt "\n", ##args); } while(0)

extern bool gForceACLVerbose;

#endif // FORCEACL_CONNECTORFIXER_HPP
