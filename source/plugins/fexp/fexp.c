#include "defines.h"
#include "base/mem.h"
#include "core/core.h"
#include "core/core.c"
#include <stdio.h>
#include <stdlib.h>

typedef struct fexp_context {
    f32 x;
    f32 y;
} fexp_context;

dll_export void* Init() {
    fexp_context* ctx = malloc(sizeof(fexp_context));
    memset(ctx, 0, sizeof(fexp_context));
    ctx->x = -50;
    ctx->y = -50;
    return ctx;
}

dll_export void Update(void* context, I_InputState* input) {
    fexp_context* ctx = (fexp_context*) context;
    
    if (I_Key(input, GLFW_KEY_RIGHT)) {
        ctx->x += 0.2f;
    }
    if (I_Key(input, GLFW_KEY_LEFT)) {
        ctx->x -= 0.2f;
    }
    if (I_Key(input, GLFW_KEY_UP)) {
        ctx->y -= 0.2f;
    }
    if (I_Key(input, GLFW_KEY_DOWN)) {
        ctx->y += 0.2f;
    }
}

dll_export void Render(void* context, D_Drawer* drawer) {
    fexp_context* ctx = (fexp_context*) context;
    D_DrawQuadC(drawer, (rect) { ctx->x, ctx->y, 100, 100 }, (vec4) { 0.2f, 0.3f, 0.8f, 1.0f }, 3);
}

dll_export void Free(void* context) {
    free(context);
}
