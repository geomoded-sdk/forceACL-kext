/**
 * ForceACL Plugin Entry Point
 * Lilu plugin bootstrap and initialization
 */

#include <mach/mach_time.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

#include "plugin_start.hpp"
#include "kern_version.hpp"
#include "kern_util.hpp"

#include "ForceACL/ForceACL.hpp"

// Global verbose and debug flags
bool gForceACLVerbose = false;
bool gForceACLDebug = false;

extern "C" {
int PE_parse_boot_arg_num(const char* arg, void* value);
}

// Simple plugin entry
void pluginStart() {
    auto* plugin = ForceACLPlugin::getInstance();
    if (plugin) {
        IOLog("ForceACL v%s - Initializing...\n", PLUGIN_VERSION);
        plugin->init();
        plugin->start();
    }
}
