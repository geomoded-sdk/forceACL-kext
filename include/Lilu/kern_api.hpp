#ifndef LILU_KERN_API_HPP
#define LILU_KERN_API_HPP

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void IOLog(const char* format, ...);

int PE_parse_boot_arg_num(const char* arg, void* value);
int PE_parse_boot_arg(const char* arg, char* buf, size_t maxSize);

#ifdef __cplusplus
}

class LiluAPI {
public:
    class Version {
    public:
        static bool atLeast(const Version& v) { (void)v; return false; }
        static Version Parse(int major, int minor, int patch) { (void)major; (void)minor; (void)patch; return Version(); }
        bool operator>=(size_t v) const { (void)v; return false; }
    };
    
    enum Error {
        NoError = 0,
        LockError = 1,
        MemoryError = 2,
        UnsupportedFeature = 3,
        IncompatibleOS = 4,
        Disabled = 5,
        TooLate = 6,
        Offline = 7
    };
    
    enum RunningMode : uint32_t {
        RunningNormal = 1,
        AllowNormal = RunningNormal,
        RunningInstallerRecovery = 2,
        AllowInstallerRecovery = RunningInstallerRecovery,
        RunningSafeMode = 4,
        AllowSafeMode = RunningSafeMode
    };
    
    Error init() { return NoError; }
    Error deinit() { return NoError; }
    Error requestAccess(size_t version=0, bool check=false) { (void)version; (void)check; return NoError; }
    Error releaseAccess() { return NoError; }
    uint32_t getRunMode() { return RunningNormal; }
    bool atLeast(size_t version) const { (void)version; return true; }
    
    void onPciDevice(const char* name, bool (*callback)(void*)) {
        (void)name; (void)callback;
    }
    
    void onIOService(const char* name, bool (*callback)(void*)) {
        (void)name; (void)callback;
    }
};

extern LiluAPI lilu;

namespace Lilu {
    inline void onPciDevice(const char* name, bool (*callback)(void*)) {
        lilu.onPciDevice(name, callback);
    }
    
    inline void onIOService(const char* name, bool (*callback)(void*)) {
        lilu.onIOService(name, callback);
    }
}

#define EXPORT __attribute__((visibility("default")))

#ifndef PLUGIN_VERSION
#define PLUGIN_VERSION "2.0.0"
#endif

inline int PE_parse_boot_arg_num(const char* arg, void* value) {
    (void)arg; (void)value;
    return 0;
}

inline int PE_parse_boot_arg(const char* arg, char* buf, size_t maxSize) {
    (void)arg; (void)buf; (void)maxSize;
    return 0;
}

inline const char* IOPCIDevice_serviceName() { return "IOPCIDevice"; }

#endif

#endif
