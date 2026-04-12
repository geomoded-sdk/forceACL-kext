/**
 * ForceACL Plugin Entry Point
 * Lilu 1.7.x plugin - Full integration
 */

#include <mach/mach_time.h>
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>

#include "plugin_start.hpp"
#include "kern_version.hpp"
#include "kern_util.hpp"

#include "ForceACL/ForceACL.hpp"

#ifndef PRODUCT_NAME
#define PRODUCT_NAME ForceACL
#endif

#ifndef MODULE_VERSION
#define MODULE_VERSION 1.0.0
#endif

extern "C" {
int PE_parse_boot_arg_num(const char* arg, void* value);
}

static const char* forceacl_disableArg[] = { "-ffacloff", "-noffacl" };
static const char* forceacl_debugArg[] = { "ffacl_debug" };
static const char* forceacl_betaArg[] = { "-ffaclbeta" };

void forceACL_start() {
    auto* plugin = ForceACLPlugin::getInstance();
    if (plugin) {
        IOLog("ForceACL v" xStringify(MODULE_VERSION) " - Initializing...\n");
        plugin->init();
        plugin->start();
    }
}

extern "C" PluginConfiguration ForceACL_config = {
    xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal,
    forceacl_disableArg,
    2,
    forceacl_debugArg,
    1,
    forceacl_betaArg,
    1,
    KernelVersion::HighSierra,
    KernelVersion::Sequoia,
    forceACL_start
};