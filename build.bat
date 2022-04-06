@ECHO off
REM build script for engine

SET compiler_flags=-Wall -Wvarargs -Werror -Wno-unused-function -g -Wno-incompatible-pointer-types-discards-qualifiers
SET include_flags=-Isource -I. -Ithird_party/include
SET defines=-D_DEBUG -D_CRT_SECURE_NO_WARNINGS

IF NOT EXIST bin/stb.o (
	ECHO Couldn't find stb.o - Building stb.o...
	clang source/impl/stb_impl.c %include_flags% -c %compiler_flags% %defines% -o bin/stb.o
)

ECHO Building base.o...
clang source/base/base.c %include_flags% -c %compiler_flags% %defines% -o bin/base.o
ECHO Building os.o...
clang source/os/os.c %include_flags% -c %compiler_flags% %defines% -o bin/os.o
ECHO Building core.o...
clang source/core/core.c %include_flags% -c %compiler_flags% %defines% -o bin/core.o

ECHO Creating engine.lib...
llvm-ar rc bin/engine.lib bin/stb.o bin/base.o bin/os.o bin/core.o

SET linker_flags=-lmsvcrt -lshell32 -lwinmm -luser32 -luserenv -lgdi32 -Lthird_party/lib -lopengl32 -lglfw3
SET customlibs=-Lbin -lengine
SET output=-o bin/test.exe

ECHO Building test.exe...
clang source/test.c third_party/source/glad.c %include_flags% %compiler_flags% %defines% %linker_flags% %customlibs% %output%
