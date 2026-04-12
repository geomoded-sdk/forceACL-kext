/**
 * ForceACL Plugin Entry Point
 * Lilu plugin for Lilu 1.7.x
 */

#include <mach/mach_time.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

#include "plugin_start.hpp"
#include "kern_version.hpp"
#include "kern_util.hpp"

/**
 * ForceACL Plugin - Lilu 1.7.x compatible
 */
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

#include "ForceACL/ForceACL.hpp"

extern "C" void ForceACL_start() {
    auto* plugin = ForceACLPlugin::getInstance();
    if (plugin) {
        IOLog("ForceACL loaded\n");
        plugin->init();
        plugin->start();
    }
}

static const char* disableArg[] = { "-ffacloff", "-noffacl" };
static const char* debugArg[] = { "ffacl_debug", "ffacl=0" };
static const char* betaArg[] = { "-ffaclbeta" };

extern "C" {
    void forceACLStart() {
        auto* plugin = ForceACLPlugin::getInstance();
        if (plugin) {
            IOLog("ForceACL v" xStringify(MODULE_VERSION) " - Initializing...\n");
            plugin->init();
            plugin->start();
        }
    }
    
    PluginConfiguration ADDPR(config) = {
        xStringify(PRODUCT_NAME),
        parseModuleVersion(xStringify(MODULE_VERSION)),
        LiluAPI::AllowNormal,
        disableArg,
        2,
        debugArg,
        2,
        betaArg,
        1,
        KernelVersion::HighSierra,
        KernelVersion::Sonoma,
        forceACLStart
    };
}
