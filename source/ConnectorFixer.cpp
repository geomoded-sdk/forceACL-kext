/**
 * ForceACL - Connector Fixer Implementation
 * Fixes connector types, bus IDs, and pipe mappings
 * Based on WhateverGreen connector patching approach
 */

#include <libkern/libkern.h>
#include "ForceACL/ConnectorFixer.hpp"

ConnectorFixer::ConnectorFixer()
    : m_verboseLogging(false) {
    CONN_FIX_LOG_VERBOSE("ConnectorFixer initialized");
}

ConnectorFixer::~ConnectorFixer() {
    CONN_FIX_LOG_VERBOSE("ConnectorFixer destroyed");
}

bool ConnectorFixer::fixDPtoHDMI(ConnectorInfo* connectors, size_t connectorCount) {
    if (!connectors || connectorCount == 0) return false;

    CONN_FIX_LOG("Fixing DP to HDMI conversions...");
    bool fixed = false;

    for (size_t i = 0; i < connectorCount; i++) {
        if (connectors[i].type == ConnectorTypeDP) {
            CONN_FIX_LOG_VERBOSE("Converting connector %lu from DP (0x%04X) to HDMI (0x%04X)",
                i, ConnectorTypeDP, ConnectorTypeHDMI);

            connectors[i].type = ConnectorTypeHDMI;
            fixed = true;
        }
    }

    if (fixed) {
        CONN_FIX_LOG("DP-to-HDMI conversion complete");
    }

    return fixed;
}

bool ConnectorFixer::fixBusIDs(ConnectorInfo* connectors, size_t connectorCount, uint8_t baseBusId) {
    if (!connectors || connectorCount == 0) return false;

    CONN_FIX_LOG("Fixing bus IDs (base: 0x%02X)...", baseBusId);
    bool fixed = false;

    for (size_t i = 0; i < connectorCount; i++) {
        // Inactive connectors might have wrong bus ID
        if (connectors[i].type != 0x0000) {
            uint8_t expectedBusId = baseBusId + i;

            if (connectors[i].bus_id != expectedBusId) {
                CONN_FIX_LOG_VERBOSE("Connector %lu: bus ID 0x%02X -> 0x%02X",
                    i, connectors[i].bus_id, expectedBusId);

                connectors[i].bus_id = expectedBusId;
                fixed = true;
            }
        }
    }

    if (fixed) {
        CONN_FIX_LOG("Bus ID fixup complete");
    }

    return fixed;
}

bool ConnectorFixer::fixPipes(ConnectorInfo* connectors, size_t connectorCount) {
    if (!connectors || connectorCount == 0) return false;

    CONN_FIX_LOG("Fixing connector pipes...");
    bool fixed = false;

    uint8_t nextPipe = 0;
    for (size_t i = 0; i < connectorCount; i++) {
        // Only active connectors get pipes
        if (connectors[i].type != 0x0000) {
            if (connectors[i].pipe != nextPipe && nextPipe < 3) {
                CONN_FIX_LOG_VERBOSE("Connector %lu: pipe %u -> %u",
                    i, connectors[i].pipe, nextPipe);

                connectors[i].pipe = nextPipe;
                nextPipe = (nextPipe + 1) % 3;
                fixed = true;
            } else if (nextPipe < 3) {
                nextPipe = (nextPipe + 1) % 3;
            }
        }
    }

    if (fixed) {
        CONN_FIX_LOG("Pipe fixup complete");
    }

    return fixed;
}

bool ConnectorFixer::disableUnusedConnectors(ConnectorInfo* connectors, size_t connectorCount,
                                            uint8_t activeCount) {
    if (!connectors || connectorCount == 0) return false;

    CONN_FIX_LOG("Disabling unused connectors (keeping first %u active)...", activeCount);
    bool fixed = false;

    for (uint8_t i = activeCount; i < connectorCount; i++) {
        if (connectors[i].type != 0x0000) {
            CONN_FIX_LOG_VERBOSE("Disabling connector %u", i);

            connectors[i].type = 0x0000;
            connectors[i].flags = 0;
            fixed = true;
        }
    }

    if (fixed) {
        CONN_FIX_LOG("Connectors disabled");
    }

    return fixed;
}

bool ConnectorFixer::fixLVDSConnector(ConnectorInfo* connectors, size_t connectorCount) {
    if (!connectors || connectorCount == 0) return false;

    CONN_FIX_LOG("Fixing LVDS connector...");

    int lvdsIndex = findLVDSConnector(connectors, connectorCount);
    if (lvdsIndex < 0) {
        CONN_FIX_LOG_VERBOSE("No LVDS connector found");
        return false;
    }

    // LVDS should be first connector (pipe 0) for laptops
    if (connectors[lvdsIndex].index != 0 || connectors[lvdsIndex].pipe != 0) {
        CONN_FIX_LOG_VERBOSE("Setting LVDS to index 0, pipe 0");

        connectors[lvdsIndex].index = 0;
        connectors[lvdsIndex].pipe = 0;
        connectors[lvdsIndex].bus_id = 0x04; // Standard LVDS bus ID

        CONN_FIX_LOG("LVDS connector fixed");
        return true;
    }

    return false;
}

bool ConnectorFixer::applyGenerationFixes(ConnectorInfo* connectors, size_t connectorCount,
                                         uint32_t platformId) {
    if (!connectors || connectorCount == 0) return false;

    CONN_FIX_LOG("Applying generation-specific fixes for platform 0x%08X", platformId);

    bool changed = false;

    // Get generation from platform ID
    uint32_t gen = (platformId >> 24) & 0xFF;

    // Sandy/Ivy Bridge: May need DP to HDMI
    if (gen >= 0x00 && gen <= 0x01) {
        changed |= fixDPtoHDMI(connectors, connectorCount);
    }

    // Haswell+: More aggressive DP to HDMI
    if (gen >= 0x04 && gen <= 0x0D) {
        changed |= fixDPtoHDMI(connectors, connectorCount);
    }

    // All generations: Fix bus IDs and pipes
    changed |= fixBusIDs(connectors, connectorCount, 0x04);
    changed |= fixPipes(connectors, connectorCount);

    return changed;
}

bool ConnectorFixer::fixConnectorsInFramebuffer(uint8_t* fbData, size_t maxSize, uint32_t platformId) {
    if (!fbData || maxSize == 0) return false;

    CONN_FIX_LOG("Fixing connectors in framebuffer data");

    // This would need platform-specific framebuffer structure knowledge
    // For now, this is a placeholder for future implementation
    // It would search for connector arrays within the framebuffer and fix them

    CONN_FIX_LOG_VERBOSE("Framebuffer connector fixup (stub - requires binary analysis)");
    return false;
}

bool ConnectorFixer::validateConnectors(const ConnectorInfo* connectors, size_t connectorCount) {
    if (!connectors || connectorCount == 0) return true;

    CONN_FIX_LOG_VERBOSE("Validating connector array...");

    uint8_t pipeCount = 0;
    for (size_t i = 0; i < connectorCount; i++) {
        // Count active connectors with valid pipes
        if (connectors[i].type != 0x0000) {
            if (connectors[i].pipe < 3) {
                pipeCount++;
            } else {
                CONN_FIX_LOG_VERBOSE("Connector %lu: invalid pipe %u", i, connectors[i].pipe);
                return false;
            }

            // Validate bus ID
            if (connectors[i].bus_id < 0x04 || connectors[i].bus_id > 0x0F) {
                CONN_FIX_LOG_VERBOSE("Connector %lu: suspicious bus ID 0x%02X", i, connectors[i].bus_id);
                // Not necessarily invalid, but unusual
            }
        }
    }

    CONN_FIX_LOG_VERBOSE("Connector validation passed");
    return true;
}

void ConnectorFixer::setVerboseLogging(bool verbose) {
    m_verboseLogging = verbose;
}

uint32_t ConnectorFixer::detectConnectorType(const ConnectorInfo* connector, uint8_t index) {
    // Heuristic detection based on bus ID and index
    if (connector->bus_id == 0x04) {
        return (index == 0) ? ConnectorTypeLVDS : ConnectorTypeDP;
    }

    if (connector->bus_id == 0x05 || connector->bus_id == 0x06) {
        return ConnectorTypeHDMI;
    }

    if (connector->bus_id == 0x07) {
        return ConnectorTypeDVI;
    }

    return ConnectorTypeUnknown;
}

bool ConnectorFixer::isConnectorActive(const ConnectorInfo* connector) {
    return connector->type != 0x0000 && (connector->flags & ConnectorFlagEnabled);
}

int ConnectorFixer::findLVDSConnector(const ConnectorInfo* connectors, size_t connectorCount) {
    for (size_t i = 0; i < connectorCount; i++) {
        if ((connectors[i].type & 0xFF00) == ConnectorTypeLVDS) {
            return i;
        }
    }

    return -1;
}

uint8_t ConnectorFixer::countActiveConnectors(const ConnectorInfo* connectors, size_t connectorCount) {
    uint8_t count = 0;
    for (size_t i = 0; i < connectorCount; i++) {
        if (isConnectorActive(&connectors[i])) {
            count++;
        }
    }

    return count;
}