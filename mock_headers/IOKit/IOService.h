// Mock IOKit/IOService.h
#ifndef IOService_h
#define IOService_h

#include <libkern/OSBase.h>

class OSObject {
public:
    virtual ~OSObject() {}
    virtual bool isEqualTo(const OSObject* obj) const { return false; }
};

class OSDictionary : public OSObject {
public:
    virtual ~OSDictionary() {}
};

class OSBoolean : public OSObject {
public:
    OSBoolean(bool value) : m_value(value) {}
    virtual ~OSBoolean() {}
    bool getValue() const { return m_value; }
private:
    bool m_value;
};

class OSString : public OSObject {
public:
    OSString(const char* str) : m_str(str) {}
    virtual ~OSString() {}
    const char* getCStringNoCopy() const { return m_str; }
private:
    const char* m_str;
};

class IOService : public OSObject {
public:
    virtual ~IOService() {}
    virtual OSObject* getProperty(const char* key) { return nullptr; }
    virtual IOReturn getResources(void) { return kIOReturnSuccess; }
    static IOService* fromRegistryEntryID(uint64_t entryID) { return nullptr; }
};

typedef int IOReturn;
const IOReturn kIOReturnSuccess = 0;

#endif