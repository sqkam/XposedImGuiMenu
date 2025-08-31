#include "Tools.h"
#include "log.h"
#include <cstddef>
#include <android/log.h>

namespace Tools {
    void Hook(void * const target, void * const replace, void ** const backup) {
        if (!target || !replace) {
            LOGE("Tools::Hook - Invalid parameters: target=%p, replace=%p", target, replace);
            return;
        }
        
        LOGI("Tools::Hook - Hooking target=%p with replace=%p", target, replace);
        
        // Based on aov-hook implementation with memory protection
        const size_t page_size = static_cast<size_t>(sysconf(_SC_PAGESIZE));
        const size_t size = page_size * sizeof(uintptr_t);
        const uintptr_t target_addr = reinterpret_cast<uintptr_t>(target);
        void * const p = reinterpret_cast<void *>(target_addr - (target_addr % page_size) - page_size);

        // Change memory protection to allow hooking
        if (mprotect(p, size, PROT_EXEC | PROT_READ | PROT_WRITE) == 0) {
            // Perform the actual hook using Dobby
            if (DobbyHook(target, replace, backup) == 0) {  // RS_SUCCESS is 0
                LOGI("Tools::Hook - Hook successful");
                // Restore memory protection (keep write for potential future hooks)
                mprotect(p, size, PROT_EXEC | PROT_READ);
            } else {
                LOGE("Tools::Hook - DobbyHook failed");
                // Restore memory protection on failure
                mprotect(p, size, PROT_EXEC | PROT_READ);
            }
        } else {
            LOGE("Tools::Hook - mprotect failed");
        }
    }
}
