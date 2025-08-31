#include "includes.h"
#include <unistd.h>

EGLBoolean (*old_eglSwapBuffers)(...);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    if (!init) {
        SetupImGui();
        init = true;
    }

    ImGuiIO &io = GetIO();
    if (io.WantTextInput && !iskeyboardopen) {
        displayKeyboard();
        iskeyboardopen = true;
    } else if (!io.WantTextInput && iskeyboardopen) {
        displayKeyboard();
        iskeyboardopen = false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    NewFrame();

    DrawMenu();

    EndFrame();
    Render();
    glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());

    Patches();

    return old_eglSwapBuffers(dpy, surface);
}

void *hack_thread(void *) {
    LOGI("hack_thread started - step by step testing");
    
    // 第一步：测试 KittyMemory 的基本功能
    LOGI("Step 1: Testing basic KittyMemory functionality");
    
    try {
        // 测试获取所有内存映射
        LOGI("Testing KittyMemory::getAllMaps()...");
        auto allMaps = KittyMemory::getAllMaps();
        LOGI("getAllMaps() returned %zu maps", allMaps.size());
        
        // 查找游戏相关的库
        LOGI("Searching for game-related libraries in maps...");
        bool foundIL2CPP = false;
        bool foundUnity = false;
        std::vector<std::string> gameLibs;
        
        for (const auto& map : allMaps) {
            std::string path = map.pathname;
            if (path.find("libil2cpp.so") != std::string::npos) {
                LOGI("Found libil2cpp.so at: %s", path.c_str());
                foundIL2CPP = true;
                gameLibs.push_back(path);
            } else if (path.find("libunity.so") != std::string::npos) {
                LOGI("Found libunity.so at: %s", path.c_str());
                foundUnity = true;
                gameLibs.push_back(path);
            } else if (path.find("libmain.so") != std::string::npos) {
                LOGI("Found libmain.so at: %s", path.c_str());
                gameLibs.push_back(path);
            } else if (path.find("libgame") != std::string::npos) {
                LOGI("Found game library: %s", path.c_str());
                gameLibs.push_back(path);
            } else if (path.find("com.tencent.ngame.chty") != std::string::npos && path.find(".so") != std::string::npos) {
                LOGI("Found app-specific library: %s", path.c_str());
                gameLibs.push_back(path);
            }
        }
        
        LOGI("Game library scan results: IL2CPP=%s, Unity=%s, Total=%zu", 
             foundIL2CPP ? "YES" : "NO", foundUnity ? "YES" : "NO", gameLibs.size());
        
        if (!foundIL2CPP && !foundUnity && gameLibs.empty()) {
            LOGI("No game libraries found yet");
        }
        
        LOGI("Step 1 completed successfully");
    } catch (...) {
        LOGE("Exception in Step 1 (KittyMemory basic test)");
        return nullptr;
    }
    
    // 第二步：测试 getElfBaseMap 功能
    LOGI("Step 2: Testing KittyMemory::getElfBaseMap functionality");
    
    try {
        // 测试查找已知存在的库
        LOGI("Testing getElfBaseMap with libc.so...");
        auto libcMap = KittyMemory::getElfBaseMap("libc.so");
        if (libcMap.isValid()) {
            LOGI("Found libc.so at base: 0x%lx, size: 0x%lx", libcMap.startAddress, libcMap.length);
        } else {
            LOGI("libc.so not found with getElfBaseMap");
        }
        
        // 测试查找 libil2cpp.so
        LOGI("Testing getElfBaseMap with libil2cpp.so...");
        auto il2cppMap = KittyMemory::getElfBaseMap("libil2cpp.so");
        if (il2cppMap.isValid()) {
            LOGI("Found libil2cpp.so at base: 0x%lx, size: 0x%lx", il2cppMap.startAddress, il2cppMap.length);
        } else {
            LOGI("libil2cpp.so not found with getElfBaseMap (expected at this stage)");
        }
        
        LOGI("Step 2 completed successfully");
    } catch (...) {
        LOGE("Exception in Step 2 (getElfBaseMap test)");
        return nullptr;
    }
    
    // 第三步：跳过BNM初始化，因为libil2cpp.so还未加载
    LOGI("Step 3: Skipping BNM initialization - libil2cpp.so not yet loaded");
    LOGI("BNM requires IL2CPP to be loaded first, we'll do this later");
    LOGI("Step 3 completed successfully - BNM skipped for now");
    
    // 第四步：使用aov-hook的方法等待libil2cpp.so
    LOGI("Step 4: Waiting for libil2cpp.so using aov-hook method...");
    
    ProcMap il2cppMap;
    bool il2cppFound = false;
    while (!il2cppMap.isValid()) {
        LOGI("Attempting to find libil2cpp.so...");
        il2cppMap = KittyMemory::getElfBaseMap("libil2cpp.so");
        if (il2cppMap.isValid()) {
            LOGI("libil2cpp.so found at base: 0x%llx, size: 0x%llx", 
                 (unsigned long long)il2cppMap.startAddress, 
                 (unsigned long long)il2cppMap.length);
            il2cppFound = true;
            break;
        }
        LOGI("libil2cpp.so not found, sleeping 1 second...");
        sleep(1);
    }
    
    if (il2cppFound) {
        LOGI("Step 4 completed successfully - libil2cpp.so loaded!");
        
        // 第五步：进行FPS Hook
        LOGI("Step 5: Initializing IL2CPP and applying FPS hooks...");
        
        try {
            // 5a: 尝试Attach到IL2CPP
            LOGI("Step 5a: Attempting IL2CPP Attach...");
            int attachResult = Il2CppHelper::Attach("libil2cpp.so");
            if (attachResult == 0) {
                LOGI("Step 5a: IL2CPP Attach successful!");
                
                // 5b: 应用FPS Hook  
                LOGI("Step 5b: Starting real IL2CPP FPS hook application...");
                Pointers();  // 现在是空操作，但保持结构
                Hooks();     // 使用真正的IL2CPP GetMethodOffset
                LOGI("Step 5b: Real IL2CPP FPS hook application completed");
                
                LOGI("Step 5 completed successfully - Real IL2CPP FPS hooks applied!");
            } else {
                LOGE("Step 5a: IL2CPP Attach failed with code %d", attachResult);
                LOGE("Step 5: Cannot proceed without IL2CPP - this might not be an IL2CPP game");
            }
        } catch (...) {
            LOGE("Exception in Step 5 (IL2CPP hook application)");
        }
    } else {
        LOGI("libil2cpp.so not found - FPS hooks not applied");
    }
    
    LOGI("hack_thread completed - all steps finished!");
    return nullptr;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jvm = vm;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    LOGI("JNI_OnLoad called");

    // 延迟BNM初始化到游戏进程真正启动后
    // BNM::Loading::TryLoadByJNI(env); // 暂时注释掉

    UnityPlayer_cls = env->FindClass("com/unity3d/player/UnityPlayer");
    UnityPlayer_CurrentActivity_fid = env->GetStaticFieldID(UnityPlayer_cls, "currentActivity", "Landroid/app/Activity;");

    int ret;
    pthread_t ntid;
    if ((ret = pthread_create(&ntid, nullptr, hack_thread, nullptr)))
        LOGE("can't create thread: %s\n", strerror(ret));
    else
        LOGI("hack_thread created successfully");
        
    return JNI_VERSION_1_6;
}