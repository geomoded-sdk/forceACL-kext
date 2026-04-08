// Mock mach/mach_time.h
#ifndef mach_time_h
#define mach_time_h

typedef uint64_t mach_timebase_info_data_t;

struct mach_timebase_info {
    uint32_t numer;
    uint32_t denom;
};

#endif