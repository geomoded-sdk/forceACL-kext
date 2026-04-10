#ifndef FORCEACL_KERN_WRAPPER_H
#define FORCEACL_KERN_WRAPPER_H

#include <stdint.h>
#include <stddef.h>

#ifdef KERNEL
#include <libkern/libkern.h>
#include <mach/mach_time.h>
#include <IOKit/IOService.h>

extern "C" {
int PE_parse_boot_arg_num(const char* arg, void* value);
int PE_parse_boot_arg(const char* arg, char* buf, size_t* bufSize);
}

#else
extern "C" {
int PE_parse_boot_arg_num(const char* arg, void* value);
int PE_parse_boot_arg(const char* arg, char* buf, size_t* bufSize);
}
#endif

#endif
