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

#include "ForceACL/ForceACL.hpp"

extern "C" {
int PE_parse_boot_arg_num(const char* arg, void* value);
}

static const char* disableArg[] = { "-ffacloff", "-noffacl" };
static const char* debugArg[] = { "ffacl_debug", "ffacl=0" };
static const char* betaArg[] = { "-ffaclbeta" };

void pluginStart() {
    auto* plugin = ForceACLPlugin::getInstance();
    if (plugin) {
        IOLog("ForceACL v%s - Initializing...\n", PLUGIN_VERSION);
        plugin->init();
        plugin->start();
    }
}

extern "C" PluginConfiguration ADDPR(config) = {
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
    pluginStart
};
