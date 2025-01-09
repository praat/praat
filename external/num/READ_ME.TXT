djmw 20250103

Routines 'common.h median_of_ninthers.cpp median_of_ninthers.h'
were downloaded from https://github.com/andralex/MedianOfNinthers

1. we renamed common.h to median_common.h and removed the instrumentation counters and instrumented swap
2. we replaced all assert with Melder_assert
3. we removed the CNT macros
4. we renamed all cswap to std::swap
