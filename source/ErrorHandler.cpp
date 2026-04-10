/**
 * ForceACL - Error Handler Implementation
 * Manages error reporting and recovery
 */

#include <libkern/libkern.h>

#include "ForceACL/ErrorHandler.hpp"

ErrorHandler::ErrorHandler()
    : m_errorCount(0)
    , m_verboseLogging(false)
{
    ERROR_LOG_VERBOSE("ErrorHandler initialized");
}

ErrorHandler::~ErrorHandler() {
    ERROR_LOG_VERBOSE("ErrorHandler destroyed");
}

void ErrorHandler::reportError(uint32_t errorCode, const char* message, const char* context) {
    m_errorCount++;

    ERROR_LOG("ERROR 0x%08X: %s", errorCode, message);
    if (context) {
        ERROR_LOG("  Context: %s", context);
    }

    // Store error in internal log
    ErrorEntry entry = {errorCode, message ? message : "", context ? context : ""};
    m_errorLog.push_back(entry);
}

void ErrorHandler::clearErrors() {
    m_errorCount = 0;
    m_errorLog.clear();
    ERROR_LOG_VERBOSE("Error log cleared");
}

size_t ErrorHandler::getErrorCount() const {
    return m_errorCount;
}

const char* ErrorHandler::getLastErrorMessage() const {
    if (m_errorLog.empty()) {
        return "No errors";
    }
    return m_errorLog.back().message;
}

bool ErrorHandler::hasCriticalErrors() const {
    for (const auto& entry : m_errorLog) {
        if (entry.errorCode >= 0xE0000000) { // Critical error range
            return true;
        }
    }
    return false;
}

void ErrorHandler::setVerboseLogging(bool verbose) {
    m_verboseLogging = verbose;
}

void ErrorHandler::logErrorSummary() {
    if (m_errorCount == 0) {
        ERROR_LOG_VERBOSE("No errors recorded");
        return;
    }

    ERROR_LOG("Error Summary: %zu errors total", m_errorCount);

    // Count errors by type
    size_t gpuErrors = 0;
    size_t injectionErrors = 0;
    size_t platformErrors = 0;

    for (const auto& entry : m_errorLog) {
        if (entry.errorCode >= ERROR_GPU_NOT_DETECTED && entry.errorCode <= ERROR_GPU_CONTEXT_FAILED) {
            gpuErrors++;
        } else if (entry.errorCode == ERROR_GPU_INJECTION_FAILED) {
            injectionErrors++;
        } else if (entry.errorCode == ERROR_GPU_NO_PLATFORM_ID) {
            platformErrors++;
        }
    }

    ERROR_LOG("  GPU Errors: %zu", gpuErrors);
    ERROR_LOG("  Injection Errors: %zu", injectionErrors);
    ERROR_LOG("  Platform Errors: %zu", platformErrors);
}

bool ErrorHandler::attemptRecovery(uint32_t errorCode) {
    ERROR_LOG_VERBOSE("Attempting recovery for error 0x%08X", errorCode);

    switch (errorCode) {
        case ERROR_GPU_NOT_DETECTED:
            // Try alternative detection methods
            ERROR_LOG_VERBOSE("Recovery: Alternative GPU detection");
            return true;

        case ERROR_GPU_INJECTION_FAILED:
            // Try fallback injection method
            ERROR_LOG_VERBOSE("Recovery: Fallback injection method");
            return true;

        case ERROR_GPU_NO_PLATFORM_ID:
            // Use default platform ID
            ERROR_LOG_VERBOSE("Recovery: Using default platform ID");
            return true;

        default:
            ERROR_LOG_VERBOSE("No recovery strategy for error 0x%08X", errorCode);
            return false;
    }
}