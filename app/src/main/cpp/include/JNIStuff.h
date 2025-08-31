#pragma once

JavaVM *jvm;
jclass UnityPlayer_cls;
jfieldID UnityPlayer_CurrentActivity_fid;
JNIEnv* getEnv() {
    JNIEnv *env;
    int status = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if(status < 0) {
        status = jvm->AttachCurrentThread(&env, NULL);
        if(status < 0) {
            LOGE("Error Getting JNI");
            return nullptr;
        }
    }
    return env;
}

void displayKeyboard() {
    JNIEnv *env = getEnv();
    jclass ctx = env->FindClass("android/content/Context");
    jfieldID fid = env->GetStaticFieldID(ctx, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
    jmethodID mid = env->GetMethodID(ctx, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject context = env->GetStaticObjectField(UnityPlayer_cls, UnityPlayer_CurrentActivity_fid);
    jobject InputManObj = env->CallObjectMethod(context, mid, (jstring) env->GetStaticObjectField(ctx, fid));
    jclass ClassInputMethodManager = env->FindClass("android/view/inputmethod/InputMethodManager");
    jmethodID toggleSoftInputId = env->GetMethodID(ClassInputMethodManager, "toggleSoftInput", "(II)V");
    env->CallVoidMethod(InputManObj, toggleSoftInputId, 0, 2); // if it doesn't work try different values for the last 2 ints
}

jmethodID MotionEvent_getX;
jmethodID MotionEvent_getY;
jmethodID MotionEvent_getAction;
jmethodID KeyEvent_getUnicodeChar;
jmethodID KeyEvent_getMetaState;
jmethodID KeyEvent_getAction;
jmethodID KeyEvent_getKeyCode;

jboolean (*old_nativeInjectEvent)(...);
jboolean nativeInjectEvent(JNIEnv *env, jobject thiz, jobject inputEvent)
{
    if (init)
    {
        jclass MotionEventCls = env->FindClass("android/view/MotionEvent");
        jclass KeyEventCls = env->FindClass("android/view/KeyEvent");
        if (env->IsInstanceOf(inputEvent, MotionEventCls))
        {
            if (!MotionEvent_getX)
                MotionEvent_getX = env->GetMethodID(MotionEventCls, "getX",
                                                    "()F");
            if (!MotionEvent_getY)
                MotionEvent_getY = env->GetMethodID(MotionEventCls, "getY","()F");
            if (!MotionEvent_getAction)
                MotionEvent_getAction = env->GetMethodID(MotionEventCls, "getAction", "()I");
            ImGuiIO &io = ImGui::GetIO();
            auto x = env->CallFloatMethod(inputEvent, MotionEvent_getX);
            auto y = env->CallFloatMethod(inputEvent, MotionEvent_getY);
            auto event_action = env->CallIntMethod(inputEvent,
                                             MotionEvent_getAction);
            ImGui_ImplAndroid_HandleInputEvent(x, y, event_action);

            if (io.WantCaptureMouse)
                return true;
        } else if (env->IsInstanceOf(inputEvent, KeyEventCls))
        {
            ImGuiIO &io = ImGui::GetIO();
            if (!KeyEvent_getAction)
                KeyEvent_getAction = env->GetMethodID(KeyEventCls, "getAction","()I");
            if (env->CallIntMethod(inputEvent, KeyEvent_getAction) == 0)
            {
                if (!KeyEvent_getKeyCode)
                    KeyEvent_getKeyCode = env->GetMethodID(KeyEventCls, "getKeyCode","()I");
                if (!KeyEvent_getUnicodeChar)
                    KeyEvent_getUnicodeChar = env->GetMethodID(KeyEventCls,"getUnicodeChar","(I)I");
                if (!KeyEvent_getMetaState)
                    KeyEvent_getMetaState = env->GetMethodID(KeyEventCls, "getMetaState", "()I");
                int KeyCode = env->CallIntMethod(inputEvent, KeyEvent_getKeyCode);
                io.AddKeyEvent(ImGui_ImplAndroid_KeyCodeToImGuiKey(KeyCode), true);
                io.AddInputCharacter(env->CallIntMethod(inputEvent, KeyEvent_getUnicodeChar,env->CallIntMethod(inputEvent,KeyEvent_getMetaState)));
            }
            if (env->CallIntMethod(inputEvent, KeyEvent_getAction) == 1)
            {
                if (!KeyEvent_getKeyCode)
                    KeyEvent_getKeyCode = env->GetMethodID(KeyEventCls, "getKeyCode","()I");
                int KeyCode = env->CallIntMethod(inputEvent, KeyEvent_getKeyCode);
                io.AddKeyEvent(ImGui_ImplAndroid_KeyCodeToImGuiKey(KeyCode), false);
            }
        }
    }
    return old_nativeInjectEvent(env, thiz, inputEvent);
}