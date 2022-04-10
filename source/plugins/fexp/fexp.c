#include "defines.h"
#include "base/mem.h"
#include "core/core.h"
#include "core/core.c"
#include <stdio.h>
#include <stdlib.h>

typedef struct fexp_context {
    i32 a;
    i32 b;
    i32 c;
} fexp_context;

dll_export void* Init() {
    fexp_context* ctx = malloc(sizeof(fexp_context));
    ctx->a = 10;
    return ctx;
}

dll_export void Update(void* context) {
    
}

dll_export void Render(void* context, D_Drawer* drawer) {
    D_DrawQuadC(drawer, (rect) { 10, 10, 200, 200 }, (vec4) { 0.2f, 0.3f, 0.8f, 1.0f }, 3);
}

dll_export void Free(void* context) {
    fexp_context* ctx = (fexp_context*) context;
    printf("%d\n", ctx->a);
    flush;
    free(context);
}
