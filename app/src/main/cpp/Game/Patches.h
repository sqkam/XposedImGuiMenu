#pragma once

void InitPatches() {
    // Patches that are called once go here
    PATCH("0x123456", "C0035FD6");
}

void Patches() {
    // Patches that are called every frame go here (those that require user input)
    PATCH_SWITCH("0x654321", "C0035FD6", some_feature);
}