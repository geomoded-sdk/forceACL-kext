/**
 * ForceACL - Error Handler Module
 * Handles and reports GPU acceleration errors
 */

#ifndef FORCEACL_ERRORHANDLER_HPP
#define FORCEACL_ERRORHANDLER_HPP

#include <stdint.h>

// Error codes
#define ERROR_GPU_NOT_DETECTED       0xE0010001
#define ERROR_GPU_NO_VENDOR         0xE0020002
#define ERROR_GPU_NO_DEVICE         0xE0030003
#define ERROR_GPU_NOT_SUPPORTED     0xE0040004
#define ERROR_GPU_INJECTION_FAILED  0xE0050005
#define ERROR_GPU_NO_PLATFORM_ID    0xE0060006
#define ERROR_GPU_NO_ACCELERATION  0xE0070007
#define ERROR_GPU_AMFI_FAILED       0xE0080008
#define ERROR_GPU_CONTEXT_FAILED    0xE0090009
#define ERROR_GPU_FRAMEBUFFER_INIT  0xE00A000A
#define ERROR_GPU_VRAM_EXHAUSTED    0xE00B000B
#define ERROR_GPU_DRIVER_NOT_LOADED 0xE00C000C

struct ErrorEntry {
    uint32_t errorCode;
    const char* name;
    const char* description;
    const char* possibleCause;
    const char* suggestedFix;
    bool isFatal;
};

class ErrorHandler {
public:
    ErrorHandler();
    ~ErrorHandler();
    
    void reportError(uint32_t errorCode, const char* details);
    void reportWarning(uint32_t errorCode, const char* details);
    void reportInfo(const char* message);
    
    bool handleError(uint32_t errorCode);
    bool isFatalError(uint32_t errorCode);
    const char* getErrorName(uint32_t errorCode);
    const char* getErrorDescription(uint32_t errorCode);
    const char* getSuggestedFix(uint32_t errorCode);
    
    void logErrorHistory();
    void clearErrorHistory();
    
    // Error recovery
    bool attemptRecovery(uint32_t errorCode);
    bool resetGPUState();
    bool clearGPUProperties();
    
private:
    static const ErrorEntry m_errorDatabase[];
    static const size_t m_errorCount;
    
    struct ErrorRecord {
        uint32_t errorCode;
        uint64_t timestamp;
        char details[128];
    };
    
    ErrorRecord m_errorHistory[20];
    uint32_t m_errorCount_history;
    
    void addErrorRecord(uint32_t code, const char* details);
    const ErrorEntry* findErrorEntry(uint32_t code);
};

// Logging macros
#define ERROR_LOG(fmt, args...) \
    do { IOLog("ForceACL: [ERROR] " fmt "\n", ##args); } while(0)

#define WARNING_LOG(fmt, args...) \
    do { IOLog("ForceACL: [WARNING] " fmt "\n", ##args); } while(0)

#define INFO_LOG(fmt, args...) \
    do { IOLog("ForceACL: [INFO] " fmt "\n", ##args); } while(0)

#endif // FORCEACL_ERRORHANDLER_HPP
