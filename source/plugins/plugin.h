#ifndef _PLUGIN_H_
#define _PLUGIN_H_

dll_export void GlobalInit() {
    M_ScratchInit();
}

dll_export void GlobalFree() {
    M_ScratchFree();
}

#endif // _PLUGIN_H_