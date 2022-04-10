@ECHO off

SET compiler_flags=-Wall -Wvarargs -Werror -Wno-unused-function -g -Wno-incompatible-pointer-types-discards-qualifiers
SET include_flags=-Isource -I. -Ithird_party/include
SET defines=-DPLUGIN -D_DEBUG -D_CRT_SECURE_NO_WARNINGS
SET custom_libs=-lmsvcrt -lshell32 -lwinmm -luser32 -luserenv -lgdi32 -Lthird_party/lib -lopengl32 -lglfw3 -Lbin -lengine

ECHO Building Plugin: fexp
clang third_party/source/glad.c source/plugins/fexp/fexp.c %include_flags% %compiler_flags% %defines% -shared %custom_libs% -o plugins/fexp.dll
