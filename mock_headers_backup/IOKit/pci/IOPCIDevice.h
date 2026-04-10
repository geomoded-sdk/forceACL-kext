// Mock IOKit/pci/IOPCIDevice.h
#ifndef IOPCIDevice_h
#define IOPCIDevice_h

#include <IOKit/IOService.h>

class IOPCIDevice : public IOService {
public:
    virtual ~IOPCIDevice() {}
    virtual uint32_t configRead32(uint8_t offset) { return 0; }
    virtual uint16_t configRead16(uint8_t offset) { return 0; }
    virtual uint8_t configRead8(uint8_t offset) { return 0; }
    virtual IOMemoryMap* mapDeviceMemoryWithIndex(unsigned int index) { return nullptr; }
    virtual IORegistryEntry* getParentEntry(gIOServicePlane_t plane) { return nullptr; }
};

class IOMemoryMap : public OSObject {
public:
    virtual ~IOMemoryMap() {}
    virtual void* getVirtualAddress() { return nullptr; }
    virtual void release() {}
};

class IORegistryEntry : public OSObject {
public:
    virtual ~IORegistryEntry() {}
};

typedef int gIOServicePlane_t;

#endif