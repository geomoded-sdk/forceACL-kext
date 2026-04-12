/**
 * ForceACL Plugin Entry Point
 * Standalone (no Lilu dependency for now)
 */

#include <mach/mach_time.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

#include "ForceACL/ForceACL.hpp"

void pluginStart() {
    auto* plugin = ForceACLPlugin::getInstance();
    if (plugin) {
        IOLog("ForceACL v1.0.0 - Initializing...\n");
        plugin->init();
        plugin->start();
    }
}
