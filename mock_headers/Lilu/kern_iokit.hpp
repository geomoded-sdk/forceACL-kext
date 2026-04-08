// Mock IOKit headers
#ifndef KERN_IOKIT_HPP
#define KERN_IOKIT_HPP

class IOService {
public:
    virtual ~IOService() {}
    virtual OSObject* getProperty(const char* key) { return nullptr; }
};

class IOPCIDevice : public IOService {
public:
    virtual ~IOPCIDevice() {}
    virtual uint32_t configRead32(uint8_t offset) { return 0; }
    virtual uint16_t configRead16(uint8_t offset) { return 0; }
    virtual uint8_t configRead8(uint8_t offset) { return 0; }
    virtual IOMemoryMap* mapDeviceMemoryWithIndex(unsigned int index) { return nullptr; }
    virtual IORegistryEntry* getParentEntry(gIOServicePlane_t plane) { return nullptr; }
};

class IOMemoryMap {
public:
    virtual ~IOMemoryMap() {}
    virtual void* getVirtualAddress() { return nullptr; }
    virtual void release() {}
};

class IORegistryEntry {
public:
    virtual ~IORegistryEntry() {}
};

typedef int gIOServicePlane_t;

#endif