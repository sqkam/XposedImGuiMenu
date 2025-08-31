#pragma once

// Macro to hook a function with its symbol name
inline void DobbyHookSM(const char* libName, const char* symbol, void* replace, void** result) {
    auto ptr = DobbySymbolResolver(libName, symbol);
    DobbyHook(ptr, replace, result);
}

bool init = false;

using KittyMemory::ProcMap;
using KittyScanner::RegisterNativeFn;

ProcMap g_il2cppBaseMap;
KittyScanner::ElfScanner unityMaps;

std::vector<MemoryPatch> memoryPatches;
std::vector<uint64_t> offsetVector;

std::string user, pass;

int glHeight, glWidth;

// Patching a offset with switch.
void patchOffset(uint64_t offset, std::string hexBytes, bool isOn) {

    MemoryPatch patch;

    //Check if offset exists in the offsetVector
    if (std::find(offsetVector.begin(), offsetVector.end(), offset) != offsetVector.end()) {
        auto itr = std::find(offsetVector.begin(), offsetVector.end(), offset);
        patch = memoryPatches[std::distance(offsetVector.begin(), itr)]; //Get index of memoryPatches vector
    } else {
        patch = MemoryPatch::createWithHex(offset, std::move(hexBytes));
        memoryPatches.push_back(patch);
        offsetVector.push_back(offset);
    }

    if (!patch.isValid()) {
        return;
    }
    if (isOn && patch.get_CurrBytes() == patch.get_OrigBytes()) {
        patch.Modify();
    } else if (!isOn && patch.get_CurrBytes() != patch.get_OrigBytes()) {
        patch.Restore();
    }
}

uintptr_t string2Offset(const char *c) {
    int base = 16;
    // See if this function catches all possibilities.
    // If it doesn't, the function would have to be amended
    // whenever you add a combination of architecture and
    // compiler that is not yet addressed.
    static_assert(sizeof(uintptr_t) == sizeof(unsigned long)
                  || sizeof(uintptr_t) == sizeof(unsigned long long),
                  "Please add string to handle conversion for this architecture.");

    // Now choose the correct function ...
    if (sizeof(uintptr_t) == sizeof(unsigned long)) {
        return strtoul(c, nullptr, base);
    }

    // All other options exhausted, sizeof(uintptr_t) == sizeof(unsigned long long))
    return strtoull(c, nullptr, base);
}

// Easy way to get a field from an instance
template<typename T> inline T &GetField(void *instance, uint64_t offset) {
    return (T &) (*(T *) ((uint64_t) instance + offset));
}

// Some useful macros to avoid typing too much
#define HOOK(offset, ptr, orig) DobbyHook((void *)(g_il2cppBaseMap.startAddress + string2Offset(offset)), (void *)ptr, (void **)&orig)
#define HOOKD(offset, func) DobbyHook((void *)(g_il2cppBaseMap.startAddress + string2Offset(offset)), (void *)func, (void **)&old_##func)
#define HOOKD_ABS(offset, func) DobbyHook((void *)(offset), (void *)func, (void **)&old_##func)
#define HOOKD_NORIG(offset, func) DobbyHook((void *)(g_il2cppBaseMap.startAddress + string2Offset(offset)), (void *)func, NULL)
#define PATCH(offset, hex) patchOffset(g_il2cppBaseMap.startAddress + string2Offset(offset), hex, true)
#define PATCH_SWITCH(offset, hex, boolean) patchOffset(g_il2cppBaseMap.startAddress + string2Offset(offset), hex, boolean)
#define RESTORE(offset) patchOffset(string2Offset(offset), "", false)
#define OFFSET(offset) (g_il2cppBaseMap.startAddress + string2Offset(offset))