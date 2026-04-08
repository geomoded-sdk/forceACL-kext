/**
 * ForceACL Plugin Entry Point
 * Lilu plugin bootstrap and initialization
 */

#include <Library/Library.h>
#include <Lilu/kern_api.hpp>
#include <Lilu/kern_util.hpp>
#include <Lilu/kern_iokit.hpp>

#include "ForceACL/ForceACL.hpp"

// Global verbose and debug flags
bool gForceACLVerbose = false;
bool gForceACLDebug = false;

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

// Alternative entry point for direct load
extern "C" {
    
    kern_return_t ForceACL_start(kmod_info_t* ki, void* d) {
        IOLog("ForceACL v%s - Starting via kmod\n", PLUGIN_VERSION);
        
        if (PE_parse_boot_arg_num("v", nullptr)) {
            gForceACLVerbose = true;
        }
        
        auto* plugin = ForceACLPlugin::getInstance();
        if (plugin) {
            plugin->init();
            plugin->start();
        }
        
        return KERN_SUCCESS;
    }
    
    kern_return_t ForceACL_stop(kmod_info_t* ki, void* d) {
        IOLog("ForceACL v%s - Stopping\n", PLUGIN_VERSION);
        
        auto* plugin = ForceACLPlugin::getInstance();
        if (plugin) {
            plugin->stop();
        }
        
        return KERN_SUCCESS;
    }
    
}
