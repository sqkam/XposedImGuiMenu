#pragma once

#include <sys/mman.h>
#include <unistd.h>
#include <dobby.h>

namespace Tools {
    // Hook function with memory protection - based on aov-hook implementation
    void Hook(void * const target, void * const replace, void ** const backup);
}
