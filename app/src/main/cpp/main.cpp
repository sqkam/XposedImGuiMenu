#include "includes.h"

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
    do {
        sleep(1);
        g_il2cppBaseMap = KittyMemory::getElfBaseMap("libil2cpp.so");
    } while (!g_il2cppBaseMap.isValid());

    do {
        sleep(1);
        unityMaps = ElfScanner::createWithPath("libunity.so");
    } while (!unityMaps.isValid());

    // input native function
    RegisterNativeFn nativeInjectEventFn = KittyScanner::findRegisterNativeFn(unityMaps, "nativeInjectEvent");

    if (nativeInjectEventFn.isValid()) {
        HOOKD_ABS(nativeInjectEventFn.fnPtr, nativeInjectEvent);
    } else {
        LOGE("InjectEventPtr is dead, menu unable to initialize.");
    }

    Pointers();
    InitPatches();
    Hooks();

    DobbyHookSM("libEGL.so", "eglSwapBuffers", (void *) hook_eglSwapBuffers, (void **) &old_eglSwapBuffers);

    LOGI("Menu finished loading");

    return nullptr;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jvm = vm;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    BNM::Loading::TryLoadByJNI(env); // BNM loading, comment this out if you don't use BNM

    UnityPlayer_cls = env->FindClass("com/unity3d/player/UnityPlayer");
    UnityPlayer_CurrentActivity_fid = env->GetStaticFieldID(UnityPlayer_cls, "currentActivity", "Landroid/app/Activity;");

    int ret;
    pthread_t ntid;
    if ((ret = pthread_create(&ntid, nullptr, hack_thread, nullptr)))
        LOGE("can't create thread: %s\n", strerror(ret));
    return JNI_VERSION_1_6;
}