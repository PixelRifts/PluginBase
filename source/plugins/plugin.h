#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include "defines.h"

#include "core/core.h"
#include "core/core.c"
#include "base/mem.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

dll_export void GlobalInit() {
    M_ScratchInit();
    
    gladLoadGL();
}

dll_export void GlobalFree() {
    M_ScratchFree();
}

#endif // _PLUGIN_H_