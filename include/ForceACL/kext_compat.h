#ifndef FORCEACL_KEXT_COMPAT_H
#define FORCEACL_KEXT_COMPAT_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
#include <new>
#endif

#ifdef KERNEL
#include <libkern/libkern.h>
#include <libkern/OSBase.h>
#include <mach/mach_time.h>
#include <mach/vm_map.h>
#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOKitKeys.h>

extern "C" {
int PE_parse_boot_arg_num(const char* arg, void* value);
int PE_parse_boot_arg(const char* arg, char* buf, size_t* bufSize);
}

#ifndef PLUGIN_VERSION
#define PLUGIN_VERSION "2.0.0"
#endif

#endif

#ifdef __cplusplus

#include <stddef.h>

namespace std {
    template<typename T> class vector {
        T* data_;
        size_t size_;
        size_t capacity_;
    public:
        vector() : data_(nullptr), size_(0), capacity_(0) {}
        ~vector() { delete[] data_; }
        
        void push_back(const T& val) {
            if (size_ >= capacity_) {
                size_t newCap = capacity_ == 0 ? 4 : capacity_ * 2;
                T* newData = new T[newCap];
                for (size_t i = 0; i < size_; i++) newData[i] = data_[i];
                delete[] data_;
                data_ = newData;
                capacity_ = newCap;
            }
            data_[size_++] = val;
        }
        
        size_t size() const { return size_; }
        T& operator[](size_t idx) { return data_[idx]; }
        const T& operator[](size_t idx) const { return data_[idx]; }
        T* data() { return data_; }
    };
}

#endif

#endif
