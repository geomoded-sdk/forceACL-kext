/**
 * ForceACL Plugin Entry Point
 * Lilu plugin bootstrap and initialization
 */

#include <libkern/libkern.h>
#include <mach/mach_time.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

#include "ForceACL/ForceACL.hpp"

// Global verbose and debug flags
bool gForceACLVerbose = false;
bool gForceACLDebug = false;

extern "C" {
int PE_parse_boot_arg_num(const char* arg, void* value);
}

// Plugin entry point - called by Lilu
void PLUGIN_ENTRY() {
    // Check for verbose boot
    if (PE_parse_boot_arg_num("v", nullptr)) {
        gForceACLVerbose = true;
    }
    
    // Check for debug mode
    if (PE_parse_boot_arg_num("ffacl_debug", nullptr)) {
        gForceACLDebug = true;
    }
    
    // Initialize plugin
    auto* plugin = ForceACLPlugin::getInstance();
    if (plugin) {
        IOLog("ForceACL v%s - Initializing...\n", PLUGIN_VERSION);
        plugin->init();
        plugin->start();
    }
}
