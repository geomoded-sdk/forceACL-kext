/**
 * ForceACL Plugin Entry Point
 * Lilu plugin bootstrap and initialization
 */

#include <mach/mach_time.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

#include <Headers/plugin_start.hpp>
#include <Headers/kern_version.hpp>
#include <Headers/kern_util.hpp>

#include "ForceACL/ForceACL.hpp"

// Global verbose and debug flags
bool gForceACLVerbose = false;
bool gForceACLDebug = false;

extern "C" {
int PE_parse_boot_arg_num(const char* arg, void* value);
}

// Disable arguments
static const char* disableArg[] = {
    "-ffacloff",
    "-noffacl"
};

// Debug arguments
static const char* debugArg[] = {
    "ffacl_debug",
    "ffacl=0"
};

// Beta arguments  
static const char* betaArg[] = {
    "-ffaclbeta"
};

// Plugin start function
extern "C" void pluginStart() {
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

// Plugin configuration for Lilu 1.7.x
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
    KernelVersion::Ventura,
    pluginStart
};
