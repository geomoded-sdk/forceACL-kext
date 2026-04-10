/**
 * ForceACL - Error Handler Implementation
 * Reports and recovers from GPU patching failures.
 */

#include <libkern/libkern.h>
#include <libkern/OSBase.h>
#include "ForceACL/ErrorHandler.hpp"

const ErrorEntry ErrorHandler::m_errorDatabase[] = {
    {ERROR_GPU_NOT_DETECTED, "GPU_NOT_DETECTED", "No GPU device was detected.", "The Intel GPU was not enumerated.", "Verify PCI device path and firmware.", true},
    {ERROR_GPU_INJECTION_FAILED, "GPU_INJECTION_FAILED", "Platform ID injection failed.", "Unable to write properties to the GPU device.", "Check patching flow and Lilu hooks.", true},
    {ERROR_GPU_NO_PLATFORM_ID, "GPU_NO_PLATFORM_ID", "No platform ID was selected.", "AI failed to choose a valid platform ID.", "Use fallback ID or update the database.", true},
    {ERROR_GPU_NO_ACCELERATION, "GPU_NO_ACCELERATION", "Graphics acceleration did not enable.", "Framebuffer patching or platform ID is invalid.", "Retry with alternate platform ID.", false},
    {ERROR_GPU_FRAMEBUFFER_INIT, "GPU_FRAMEBUFFER_INIT", "Framebuffer initialization failed.", "Driver framebuffer resources did not load.", "Check connector and memory patches.", true},
    {ERROR_GPU_VRAM_EXHAUSTED, "GPU_VRAM_EXHAUSTED", "Insufficient VRAM reported.", "The driver reports only 4MB or invalid VRAM.", "Inject corrected VRAM properties.", false}
};

const size_t ErrorHandler::m_errorCount = sizeof(ErrorHandler::m_errorDatabase) / sizeof(ErrorHandler::m_errorDatabase[0]);

ErrorHandler::ErrorHandler()
    : m_errorCount_history(0)
{
}

ErrorHandler::~ErrorHandler() {}

void ErrorHandler::reportError(uint32_t errorCode, const char* details) {
    const ErrorEntry* entry = findErrorEntry(errorCode);
    if (entry) {
        ERROR_LOG("%s: %s", entry->name, entry->description);
        if (details) {
            ERROR_LOG("Details: %s", details);
        }
        addErrorRecord(errorCode, details ? details : "No details");
    } else {
        ERROR_LOG("Unknown error 0x%08X: %s", errorCode, details ? details : "No details");
    }
}

void ErrorHandler::reportWarning(uint32_t errorCode, const char* details) {
    const ErrorEntry* entry = findErrorEntry(errorCode);
    if (entry) {
        WARNING_LOG("%s: %s", entry->name, entry->description);
        if (details) {
            WARNING_LOG("Details: %s", details);
        }
        addErrorRecord(errorCode, details ? details : "No details");
    }
}

void ErrorHandler::reportInfo(const char* message) {
    INFO_LOG("%s", message ? message : "");
}

bool ErrorHandler::handleError(uint32_t errorCode) {
    const ErrorEntry* entry = findErrorEntry(errorCode);
    if (!entry) {
        return false;
    }
    if (entry->isFatal) {
        reportError(errorCode, "Fatal error encountered.");
        return false;
    }
    reportWarning(errorCode, "Recoverable error encountered.");
    return attemptRecovery(errorCode);
}

bool ErrorHandler::isFatalError(uint32_t errorCode) {
    const ErrorEntry* entry = findErrorEntry(errorCode);
    return entry ? entry->isFatal : false;
}

const char* ErrorHandler::getErrorName(uint32_t errorCode) {
    const ErrorEntry* entry = findErrorEntry(errorCode);
    return entry ? entry->name : "UNKNOWN_ERROR";
}

const char* ErrorHandler::getErrorDescription(uint32_t errorCode) {
    const ErrorEntry* entry = findErrorEntry(errorCode);
    return entry ? entry->description : "Unknown error description";
}

const char* ErrorHandler::getSuggestedFix(uint32_t errorCode) {
    const ErrorEntry* entry = findErrorEntry(errorCode);
    return entry ? entry->suggestedFix : "No suggested fix available";
}

void ErrorHandler::logErrorHistory() {
    for (uint32_t i = 0; i < m_errorCount_history; i++) {
        const ErrorRecord& record = m_errorHistory[i];
        ERROR_LOG("[%u] 0x%08X @%llu: %s", i, record.errorCode, record.timestamp, record.details);
    }
}

void ErrorHandler::clearErrorHistory() {
    m_errorCount_history = 0;
}

bool ErrorHandler::attemptRecovery(uint32_t errorCode) {
    switch (errorCode) {
        case ERROR_GPU_NO_ACCELERATION:
        case ERROR_GPU_VRAM_EXHAUSTED:
            WARNING_LOG("Attempting GPU recovery for error 0x%08X", errorCode);
            return true;
        case ERROR_GPU_INJECTION_FAILED:
            WARNING_LOG("Retrying platform injection");
            return true;
        default:
            WARNING_LOG("No recovery strategy for error 0x%08X", errorCode);
            return false;
    }
}

bool ErrorHandler::resetGPUState() {
    WARNING_LOG("Resetting GPU state is not supported in this context");
    return false;
}

bool ErrorHandler::clearGPUProperties() {
    WARNING_LOG("Clearing GPU properties is not supported in this context");
    return false;
}

void ErrorHandler::addErrorRecord(uint32_t code, const char* details) {
    if (m_errorCount_history >= sizeof(m_errorHistory) / sizeof(m_errorHistory[0])) {
        return;
    }

    m_errorHistory[m_errorCount_history].errorCode = code;
    m_errorHistory[m_errorCount_history].timestamp = mach_absolute_time();
    strlcpy(m_errorHistory[m_errorCount_history].details, details, sizeof(m_errorHistory[m_errorCount_history].details));
    m_errorCount_history++;
}

const ErrorEntry* ErrorHandler::findErrorEntry(uint32_t code) {
    for (size_t i = 0; i < m_errorCount; i++) {
        if (m_errorDatabase[i].errorCode == code) {
            return &m_errorDatabase[i];
        }
    }
    return nullptr;
}
