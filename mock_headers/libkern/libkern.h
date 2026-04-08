// Mock libkern/libkern.h
#ifndef libkern_h
#define libkern_h

#include <libkern/OSBase.h>
#include <stdint.h>
#include <string.h>

// Mock kernel functions
static inline void* memcpy(void* dst, const void* src, size_t len) {
    return ::memcpy(dst, src, len);
}

static inline void* memset(void* dst, int c, size_t len) {
    return ::memset(dst, c, len);
}

static inline int strcmp(const char* s1, const char* s2) {
    return ::strcmp(s1, s2);
}

static inline size_t strlen(const char* s) {
    return ::strlen(s);
}

// Mock IOLog
#define IOLog printf

// Mock IOSleep
static inline void IOSleep(uint32_t milliseconds) {
    // Mock sleep
}

#endif