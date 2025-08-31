#pragma once

#include <stdio.h>
#include <android/log.h>
#include <dlfcn.h>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <inttypes.h>

using namespace std;

#define IL2CPP_TAG "XposedMenu-Il2Cpp"
#define IL2CPP_LOGI(...) __android_log_print(ANDROID_LOG_INFO, IL2CPP_TAG, __VA_ARGS__)
#define IL2CPP_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, IL2CPP_TAG, __VA_ARGS__)

namespace Il2CppHelper {
    // IL2CPP函数指针声明
    extern const void *(*il2cpp_assembly_get_image)(const void *assembly);
    extern void *(*il2cpp_domain_get)();
    extern void **(*il2cpp_domain_get_assemblies)(const void *domain, size_t *size);
    extern const char *(*il2cpp_image_get_name)(void *image);
    extern void *(*il2cpp_class_from_name)(const void *image, const char *namespaze, const char *name);
    extern void *(*il2cpp_class_get_method_from_name)(void *klass, const char *name, int argsCount);

    // 缓存
    extern map<string, void *> m_cacheMethods;
    extern map<string, void *> m_cacheClasses;

    // 核心函数
    int Attach(const char *libname = "libil2cpp.so");
    void *GetImage(const char *image);
    void *GetClass(const char *image, const char *namespaze, const char *clazz);
    void *GetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, int argsCount = 0);
    
    // xdl相关函数
    void *get_export_function(const char *lib, const char *name);
}
