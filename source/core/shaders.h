/* date = September 30th 2021 0:54 pm */

const char* v_src = "#version 330 core\n"
"layout (location = 0) in vec2  a_pos;\n"
"layout (location = 1) in vec2  a_texcoord;\n"
"layout (location = 2) in float a_texindex;\n"
"layout (location = 3) in vec4  a_color;\n"
"layout (location = 4) in float a_rounding;\n"
"layout (location = 5) in vec2  a_uidims;\n"
"layout (location = 6) in vec2  a_uicorner;\n"
"\n"
"layout (location = 0) out float v_texindex;\n"
"layout (location = 1) out vec2  v_texcoord;\n"
"layout (location = 2) out vec4  v_color;\n"
"layout (location = 3) out float v_rounding;\n"
"layout (location = 4) out vec2  v_uidims;\n"
"layout (location = 5) out vec2  v_uicorner;\n"
"\n"
"uniform mat4 u_projection;"
"\n"
"void main() {\n"
"    gl_Position = u_projection * vec4(a_pos, 0.0, 1.0);\n"
"    v_texindex = a_texindex;\n"
"    v_texcoord = a_texcoord;\n"
"    v_color = a_color;\n"
"    v_rounding = a_rounding;\n"
"    v_uidims = a_uidims;\n"
"    v_uicorner = a_uicorner;\n"
"}\n";

const char* f_src = "#version 330 core\n"
"layout (location = 0) in float v_texindex;\n"
"layout (location = 1) in vec2  v_texcoord;\n"
"layout (location = 2) in vec4  v_color;\n"
"layout (location = 3) in float v_rounding;\n"
"layout (location = 4) in vec2  v_uidims;\n"
"layout (location = 5) in vec2  v_uicorner;\n"
"\n"
"layout (location = 0) out vec4 f_color;\n"
"\n"
"uniform sampler2D u_tex[8];\n"
"\n"
"const float smoothness = 0.5;\n"
"\n"
"float round_corners() {\n"
"    vec2 pixelPos = v_uicorner * vec2(v_uidims.x, v_uidims.y);\n"
"    vec2 minCorner = vec2(v_rounding, v_rounding);\n"
"    vec2 maxCorner = vec2(v_uidims.x - v_rounding, v_uidims.y - v_rounding);\n"
"    \n"
"    vec2 cornerPoint = clamp(pixelPos, minCorner, maxCorner);\n"
"    float lowerBound = (v_rounding - smoothness);\n"
"    float upperBound = (v_rounding + smoothness);\n"
"    \n"
"    float ppxmin = 1.0 - step(minCorner.x, pixelPos.x);\n"
"    float ppxmax = 1.0 - step(pixelPos.x, maxCorner.x);\n"
"    float ppymin = 1.0 - step(minCorner.y, pixelPos.y);\n"
"    float ppymax = 1.0 - step(pixelPos.y, maxCorner.y);\n"
"    \n"
"    float boolean = step(1, (ppxmin + ppxmax) * (ppymin + ppymax));\n"
"    float cornerAlpha = 1.0 - smoothstep(lowerBound, upperBound, distance(pixelPos, cornerPoint));\n"
"    return boolean * cornerAlpha + (1. - boolean);\n"
"}\n"
"\n"
"void main() {\n"
"    switch (int(v_texindex)) {\n"
"        case 0: f_color = v_color * texture(u_tex[0], v_texcoord); break;\n"
"        case 1: f_color = v_color * texture(u_tex[1], v_texcoord); break;\n"
"        case 2: f_color = v_color * texture(u_tex[2], v_texcoord); break;\n"
"        case 3: f_color = v_color * texture(u_tex[3], v_texcoord); break;\n"
"        case 4: f_color = v_color * texture(u_tex[4], v_texcoord); break;\n"
"        case 5: f_color = v_color * texture(u_tex[5], v_texcoord); break;\n"
"        case 6: f_color = v_color * texture(u_tex[6], v_texcoord); break;\n"
"        case 7: f_color = v_color * texture(u_tex[7], v_texcoord); break;\n"
"        default: discard;\n"
"    }\n"
"    f_color.a *= round_corners();\n"
"}\n";
