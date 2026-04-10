// Mock IOKit/IOMessage.h
#ifndef IOMessage_h
#define IOMessage_h

const uint32_t kIOMessageServiceIsTerminated = 0x00000001;
const uint32_t kIOMessageServiceIsSuspended = 0x00000002;
const uint32_t kIOMessageServiceIsResumed = 0x00000003;

#endif