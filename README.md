# Xposed ImGui Menu

Uses Xposed's ZygoteInit function to inject the menu at runtime **on Unity IL2CPP games**.

While it's supposed to be used with just Xposed and rooted devices, the module works with **[LSPatch](https://github.com/JingMatrix/LSPatch)**, this way you can have stealthy no root injection.

## Cloning:
```
git clone --recursive https://github.com/fedes1to/XposedImGuiMenu.git --depth 1
```

TODO: Implement launcher.

## Important info
This menu implements ImGui, libcURL, openSSL, KittyMemory, Dobby and Keystone, **it does NOT support Vulkan**.

Remember to modify accordingly the [Inject.java](https://github.com/fedes1to/XposedImGuiMenu/blob/master/app/src/main/java/org/modfs/xposedmenu/Inject.java), [AndroidManifest.xml](https://github.com/fedes1to/XposedImGuiMenu/blob/master/app/src/main/AndroidManifest.xml), [build.gradle:app](https://github.com/fedes1to/XposedImGuiMenu/blob/master/app/build.gradle), GlobalSettings.hpp (BNM) and [log.h](https://github.com/fedes1to/XposedImGuiMenu/blob/master/app/src/main/cpp/include/log.h) files.

If you want to keep your menu safe from cracking we recommend [Hikari](https://github.com/61bcdefg/Hikari-LLVM15) and header string encryption ([obfusheader](https://github.com/ac3ss0r/obfusheader.h) or [skCrypter](https://github.com/skadro-official/skCrypter) work well).

To keep the final APK file as small and compatible as possible, compile for your specific target CPU architecture using [abiFilters](https://developer.android.com/ndk/guides/abis).
