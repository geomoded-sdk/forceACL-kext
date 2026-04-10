// Mock libkern/OSBase.h
#ifndef OSBase_h
#define OSBase_h

#include <stdint.h>
#include <stddef.h>

typedef unsigned int UInt32;
typedef signed int SInt32;
typedef unsigned short UInt16;
typedef signed short SInt16;
typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned long long UInt64;
typedef signed long long SInt64;

typedef UInt32 IOOptionBits;
typedef SInt32 OSComparisonResult;

class OSMetaClassBase {
public:
    virtual ~OSMetaClassBase() {}
};

#endif