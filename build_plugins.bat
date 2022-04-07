@ECHO off

SET compiler_flags=-Wall -Wvarargs -Werror -Wno-unused-function -g -shared -Wno-incompatible-pointer-types-discards-qualifiers
SET include_flags=-Isource -I. -Ithird_party/include
SET defines=-D_DEBUG -D_CRT_SECURE_NO_WARNINGS

ECHO Building Plugin: fexp
clang source/plugins/fexp/fexp.c %include_flags% %compiler_flags% %defines% -o plugins/fexp.dll
