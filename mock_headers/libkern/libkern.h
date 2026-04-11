// Mock libkern.h for Linux syntax checking
#ifndef LIBKERN_LIBKERN_H
#define LIBKERN_LIBKERN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Basic kernel types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// Forward declarations for commonly used types
class OSObject;
class OSString;
class OSNumber;
class OSData;
class OSBoolean;
class OSDictionary;
class OSArray;

// OSObject base class
class OSObject {
public:
    virtual ~OSObject() {}
    virtual const char* getName() const { return "OSObject"; }
    virtual void retain() {}
    virtual void release() {}
    virtual OSObject* getProperty(const char* key) const { (void)key; return nullptr; }
    virtual void setProperty(const char* key, OSObject* value) { (void)key; (void)value; }
};

// OSString
class OSString : public OSObject {
public:
    OSString() {}
    OSString(const char* s) : m_str(s) {}
    const char* getCString() const { return m_str.c_str(); }
    int getLength() const { return (int)m_str.length(); }
private:
    std::string m_str;
};

// OSNumber
class OSNumber : public OSObject {
public:
    OSNumber() : m_val(0) {}
    OSNumber(uint64_t v) : m_val(v) {}
    uint64_t unsigned64BitValue() const { return m_val; }
    uint32_t unsigned32BitValue() const { return (uint32_t)m_val; }
private:
    uint64_t m_val;
};

// OSData
class OSData : public OSObject {
public:
    static OSData* withBytes(const void* bytes, size_t size);
    void* getBytesNoCopy() const { return (void*)m_data; }
    size_t getLength() const { return m_size; }
private:
    const void* m_data;
    size_t m_size;
};

// OSBoolean
class OSBoolean : public OSObject {
public:
    static OSBoolean* withBoolean(bool val);
    bool getValue() const { return m_val; }
private:
    bool m_val;
};

// OSDictionary
class OSDictionary : public OSObject {
public:
    OSDictionary() {}
    OSObject* getObject(const char* key) const { (void)key; return nullptr; }
    void setObject(const char* key, OSObject* obj) { (void)key; (void)obj; }
};

// OSArray
class OSArray : public OSObject {
public:
    OSArray() {}
    OSObject* getObject(size_t idx) const { (void)idx; return nullptr; }
    size_t getCount() const { return 0; }
};

// OSDynamicCast helper
template<typename T>
T* OSDynamicCast(T*, OSObject* obj) {
    return dynamic_cast<T*>(obj);
}

// IOLog stub
#define IOLog(fmt, args...) 

// panic stub
#define panic(msg) do { } while(0)

// OSSwap functions
#define OSSwapHostToBigInt16(x) (((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8)))
#define OSSwapHostToBigInt32(x) ((((x) >> 24) & 0xFF) | (((x) >> 8) & 0xFF00) | (((x) & 0xFF00) << 8) | (((x) & 0xFF) << 24))
#define OSSwapHostToBigInt64(x) ((((x) >> 56) & 0xFF) | (((x) >> 40) & 0xFF00) | (((x) >> 24) & 0xFF0000) | (((x) >> 8) & 0xFF000000) | (((x) & 0xFF000000) << 8) | (((x) & 0xFF0000) << 24) | (((x) & 0xFF00) << 40) | (((x) & 0xFF) << 56))

#endif // LIBKERN_LIBKERN_H