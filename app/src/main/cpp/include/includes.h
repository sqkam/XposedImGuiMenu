#pragma once

// C++ headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <limits>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <unordered_set>
#include <utility>

// C headers
#include <unistd.h>
#include <fcntl.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <jni.h>
#include <android/log.h>

// Custom external headers
#include <dobby.h>
#include "Il2CppHelper.h"
#include "Tools.h"
#include <KittyInclude.hpp>
#include <KittyMemory.hpp>
#include <KittyScanner.hpp>
#include <MemoryPatch.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_android.h>

// Obfuscation macro (simple implementation)
#define OBFUSCATE(str) str

// IL2Cpp namespace declaration (similar to the reference project)
namespace Il2Cpp {
    void *GetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, int argsCount = 0);
}

// Using alias for easier access (like the reference project)
using namespace Il2Cpp;

// 移除BNM相关声明，直接使用Dobby

// Internal headers
#include "input.h"
#include "log.h"
#include "Misc.h"
#include "JNIStuff.h"
#include "ptr.h"
#include "GameValues.h"
#include "Pointers.h"
#include "Patches.h"
#include "Hooks.h"
#include "../Menu.h"