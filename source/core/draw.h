/* date = October 2nd 2021 11:17 am */

#ifndef DRAW_H
#define DRAW_H

#define D_MAX_BATCHES 16

void D_Init();
void D_Shutdown();

void D_DrawQuadC(rect quad, vec4 color, f32 rounding);
void D_DrawQuadT(rect quad, R_Texture texture, vec4 tint);
void D_DrawQuadST(rect quad, R_Texture texture, rect uvs, vec4 tint);

void D_DrawString(vec2 pos, string str);
void D_DrawStringC(vec2 pos, string str, vec4 color);

void D_SetFont(string filename, f32 size);

void D_BeginDraw();
void D_EndDraw();

#endif //DRAW_H
