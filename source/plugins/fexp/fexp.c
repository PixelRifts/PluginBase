#include "defines.h"
#include <stdio.h>

dll_export void Init() {
    printf("Init\n");
}

dll_export void Update() {
    
}

dll_export void Render() {
    
}

dll_export void Free() {
    printf("Free\n");
}
