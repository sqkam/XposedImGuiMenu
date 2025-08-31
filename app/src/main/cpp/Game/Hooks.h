#pragma once

// Simple hook example
float (*old_some_method)(...);
float some_method(void* instance) {
    if (!instance)
        return old_some_method(instance);

    if (some_feature)
        some_pointer(7.0f); // declared on Pointers.h

    // get/set field examples
    GetField<int>(instance, 0x10) = 99; // Set int field 0x10 to 99
    float fieldValue = GetField<float>(instance, 0x48); // Get float field 0x48

    return fieldValue;
}

void Hooks() {
    HOOKD("0x123456", some_method);
}