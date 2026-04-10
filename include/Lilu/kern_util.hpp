#ifndef LILU_KERN_UTIL_HPP
#define LILU_KERN_UTIL_HPP

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void IOLog(const char* format, ...);

#ifdef __cplusplus
}

#define OSDynamicCast(type, obj) (static_cast<type*>(obj))

#endif

#endif
