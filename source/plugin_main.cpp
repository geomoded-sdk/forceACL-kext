/**
 * ForceACL Plugin - Lilu 1.7.x
 * Fully compatible with Lilu plugin API
 */

#include <mach/mach_time.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

#include "ForceACL/ForceACL.hpp"

extern "C" void forceACLStart() {
    auto* plugin = ForceACLPlugin::getInstance();
    if (plugin) {
        IOLog("ForceACL v1.0.0 loaded\n");
        plugin->init();
        plugin->start();
    }
}