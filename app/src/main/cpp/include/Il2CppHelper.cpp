#include "Il2CppHelper.h"
#include "../libraries/xdl/include/xdl.h"

namespace Il2CppHelper {
    // IL2CPP函数指针定义
    const void *(*il2cpp_assembly_get_image)(const void *assembly) = nullptr;
    void *(*il2cpp_domain_get)() = nullptr;
    void **(*il2cpp_domain_get_assemblies)(const void *domain, size_t *size) = nullptr;
    const char *(*il2cpp_image_get_name)(void *image) = nullptr;
    void *(*il2cpp_class_from_name)(const void *image, const char *namespaze, const char *name) = nullptr;
    void *(*il2cpp_class_get_method_from_name)(void *klass, const char *name, int argsCount) = nullptr;

    // 缓存
    map<string, void *> m_cacheMethods;
    map<string, void *> m_cacheClasses;

    void *get_export_function(const char *lib, const char *name) {
        void *handle = xdl_open(lib, 4);
        if (handle) {
            void *fn = xdl_sym(handle, name, 0);
            if (fn) {
                IL2CPP_LOGI("Found IL2CPP function: %s", name);
                return fn;
            }
        }
        IL2CPP_LOGE("Failed to find IL2CPP function: %s", name);
        return nullptr;
    }

    int Attach(const char *libname) {
        if (!libname) return -1;

        IL2CPP_LOGI("Attaching to IL2CPP library: %s", libname);

        il2cpp_assembly_get_image = (const void *(*)(const void *)) get_export_function(libname, "il2cpp_assembly_get_image");
        il2cpp_domain_get = (void *(*)()) get_export_function(libname, "il2cpp_domain_get");
        il2cpp_domain_get_assemblies = (void **(*)(const void *, size_t *)) get_export_function(libname, "il2cpp_domain_get_assemblies");
        il2cpp_image_get_name = (const char *(*)(void *)) get_export_function(libname, "il2cpp_image_get_name");
        il2cpp_class_from_name = (void *(*)(const void *, const char *, const char *)) get_export_function(libname, "il2cpp_class_from_name");
        il2cpp_class_get_method_from_name = (void *(*)(void *, const char *, int)) get_export_function(libname, "il2cpp_class_get_method_from_name");

        // 检查关键函数是否加载成功
        if (!il2cpp_domain_get || !il2cpp_domain_get_assemblies || 
            !il2cpp_image_get_name || !il2cpp_class_from_name || 
            !il2cpp_class_get_method_from_name) {
            IL2CPP_LOGE("Failed to load essential IL2CPP functions");
            return -1;
        }

        IL2CPP_LOGI("IL2CPP Attach completed successfully");
        return 0;
    }

    void *GetImage(const char *image) {
        if (!il2cpp_domain_get || !il2cpp_domain_get_assemblies || !il2cpp_assembly_get_image || !il2cpp_image_get_name) {
            IL2CPP_LOGE("IL2CPP not attached properly");
            return nullptr;
        }

        void *domain = il2cpp_domain_get();
        if (!domain) {
            IL2CPP_LOGE("Could not get IL2CPP domain");
            return nullptr;
        }

        size_t assemblyCount;
        void **assemblies = il2cpp_domain_get_assemblies(domain, &assemblyCount);
        
        for (size_t i = 0; i < assemblyCount; i++) {
            const void *img = il2cpp_assembly_get_image(assemblies[i]);
            if (img) {
                const char *imgName = il2cpp_image_get_name((void*)img);
                if (imgName && strstr(imgName, image)) {
                    IL2CPP_LOGI("Found image: %s", imgName);
                    return (void*)img;
                }
            }
        }

        IL2CPP_LOGE("Could not find image: %s", image);
        return nullptr;
    }

    void *GetClass(const char *image, const char *namespaze, const char *clazz) {
        string _sig = string(image) + namespaze + clazz;
        
        if (m_cacheClasses.count(_sig) > 0) {
            return m_cacheClasses[_sig];
        }

        void *img = GetImage(image);
        if (!img) {
            IL2CPP_LOGE("Can't find image %s!", image);
            return nullptr;
        }

        void *klass = il2cpp_class_from_name(img, namespaze, clazz);
        if (!klass) {
            IL2CPP_LOGE("Can't find class %s in namespace %s!", clazz, namespaze);
            return nullptr;
        }

        m_cacheClasses[_sig] = klass;
        IL2CPP_LOGI("Found class: %s.%s", namespaze, clazz);
        return klass;
    }

    void *GetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, int argsCount) {
        string _sig = string(image) + namespaze + clazz + name + to_string(argsCount);
        
        if (m_cacheMethods.count(_sig) > 0) {
            return m_cacheMethods[_sig];
        }

        void *klass = GetClass(image, namespaze, clazz);
        if (!klass) {
            IL2CPP_LOGE("Can't find class %s for method %s!", clazz, name);
            return nullptr;
        }

        void **method = (void**)il2cpp_class_get_method_from_name(klass, name, argsCount);
        if (!method) {
            IL2CPP_LOGE("Can't find method %s in class %s!", name, clazz);
            return nullptr;
        }

        m_cacheMethods[_sig] = *method;
        IL2CPP_LOGI("Found method: %s.%s.%s with %d args", namespaze, clazz, name, argsCount);
        return *method;
    }
}
