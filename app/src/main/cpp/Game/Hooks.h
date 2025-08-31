#pragma once

// 放弃BNM，使用项目原生的GetMethodOffset函数
bool (*_UnlockFPS)() = nullptr;

bool UnlockFPS() {
    return true; // 直接返回true，解锁FPS
}

void Hooks() {
    LOGI("========== 使用原生GetMethodOffset的FPS解锁Hook ==========");
    
    try {
        // 60 FPS Hook
        LOGI("正在尝试60 FPS Hook...");
        void* offset60 = GetMethodOffset(OBFUSCATE("Project_d.dll"), OBFUSCATE("Assets.Scripts.Framework"), OBFUSCATE("GameSettings"), OBFUSCATE("get_Supported60FPSMode"), 0);
        if (offset60 != nullptr) {
            LOGI("60 FPS method offset: %p", offset60);
            DobbyHook(offset60, (void*)UnlockFPS, (void**)&_UnlockFPS);
            LOGI("60 FPS hook应用成功");
        } else {
            LOGE("60 FPS method未找到");
        }
        
        // 90 FPS Hook  
        LOGI("正在尝试90 FPS Hook...");
        void* offset90 = GetMethodOffset(OBFUSCATE("Project_d.dll"), OBFUSCATE("Assets.Scripts.Framework"), OBFUSCATE("GameSettings"), OBFUSCATE("get_Supported90FPSMode"), 0);
        if (offset90 != nullptr) {
            LOGI("90 FPS method offset: %p", offset90);
            DobbyHook(offset90, (void*)UnlockFPS, (void**)&_UnlockFPS);
            LOGI("90 FPS hook应用成功");
        } else {
            LOGE("90 FPS method未找到");
        }
        
        // 120 FPS Hook
        LOGI("正在尝试120 FPS Hook...");
        void* offset120 = GetMethodOffset(OBFUSCATE("Project_d.dll"), OBFUSCATE("Assets.Scripts.Framework"), OBFUSCATE("GameSettings"), OBFUSCATE("get_Supported120FPSMode"), 0);
        if (offset120 != nullptr) {
            LOGI("120 FPS method offset: %p", offset120);
            DobbyHook(offset120, (void*)UnlockFPS, (void**)&_UnlockFPS);
            LOGI("120 FPS hook应用成功");
        } else {
            LOGE("120 FPS method未找到");
        }
        
        LOGI("所有FPS Hook尝试完成");
    } catch (...) {
        LOGE("FPS Hook过程中发生异常");
    }
    
    LOGI("========== 原生GetMethodOffset FPS Hook完成 ==========");
}