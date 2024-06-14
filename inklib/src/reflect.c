
#include <inkling/reflect.h>

ink_readonly ink_typeInfo u8TypeInfo = {
    .size = sizeof(u8),
    .nFields = 0
};

ink_readonly ink_typeInfo i32TypeInfo = {
    .size = sizeof(i32),
    .nFields = 0
};

ink_readonly ink_typeInfo u32TypeInfo = {
    .size = sizeof(u32),
    .nFields = 0
};

ink_readonly ink_typeInfo f32TypeInfo = {
    .size = sizeof(f32),
    .nFields = 0
};

ink_readonly ink_typeInfo f64TypeInfo = {
    .size = sizeof(f64),
    .nFields = 0
};

ink_readonly ink_typeInfo usizeTypeInfo = {
    .size = sizeof(usize),
    .nFields = 0
};

void ink_transferData(void* from, ink_typeInfo* fromType, void* to, ink_typeInfo* toType) {
    memcpy(to, from, fromType->size);
    for(usize i = 0; i < toType->nFields; i++) {
        for(usize j = 0; j < fromType->nFields; j++) {
            if(strcmp(toType->fields[i].name, fromType->fields[j].name) == 0) {
                ink_transferData(from + fromType->fields[j].offset, fromType->fields[j].type, to + toType->fields[i].offset, toType->fields[i].type);
            }
        }
    }
}
