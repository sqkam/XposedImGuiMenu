package org.modfs.xposedmenu;

import android.content.pm.ApplicationInfo;
import android.os.Build;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.Objects;

import de.robv.android.xposed.IXposedHookZygoteInit;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class Inject implements IXposedHookZygoteInit  {
    boolean loaded = false;
    String app_name = "com.app.here"; // EDIT THIS BEFORE COMPILING
    String target_abi = Build.SUPPORTED_ABIS[0]; // SET THIS TO THE ABI YOU'RE TARGETING
    public static InputStream resourceStream(String name) {
        return Objects.requireNonNull(Inject.class.getClassLoader()).getResourceAsStream(name);
    }

    @Override
    public void initZygote(StartupParam startupParam) throws Throwable {
        XposedHelpers.findAndHookMethod(Class.forName("android.app.LoadedApk"), "createAppFactory", ApplicationInfo.class,
                ClassLoader.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                ApplicationInfo ai = (ApplicationInfo)param.args[0];
                ClassLoader cl = (ClassLoader)param.args[1];
                if (ai.toString().contains(app_name) && cl != null) {
                    if (!loaded) {
                        // load our menu before anything else, this is EXTREMELY fast so it might break lol
                        String pathname = "/data/user/0/" + app_name + "/cache/libxposedmenu.so";
                        File soFile = new File(pathname);

                        // NOTE: The ABI set has to be the one that the app uses
                        InputStream soFileStream = resourceStream("lib/" + target_abi + "/libxposedmenu.so");

                        byte[] soFileContent = new byte[soFileStream.available()];
                        soFileStream.read(soFileContent);
                        soFile.createNewFile();
                        FileOutputStream out = new FileOutputStream(soFile);
                        out.write(soFileContent);
                        XposedHelpers.callStaticMethod(XposedHelpers.findClass("java.lang.Runtime", cl), "nativeLoad", pathname, cl);
                        loaded = true;
                    }
                }
                super.beforeHookedMethod(param);
            }
        });
    }
}
