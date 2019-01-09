@echo Building Raydium Dll
@set back_path=%PATH%
@PATH .\CodeBlocks\MinGW\bin;
@if exist bin\raydium*.* (
	del bin\raydium*.*
)
@if exist bin\libraydium*.* (
	del bin\libraydium*.*
)
@mingw32-gcc.exe -w -DRAYDLL -fexpensive-optimizations -O3  -Iraydium -Iinclude\php -Iinclude\php\main -Iinclude\php\regex -Iinclude\php\Zend -Iinclude\ode\include -Iinclude -Iinclude\php\TSRM -c raydium\raydium\index.c -o bin\index.o
@if errorlevel 1 goto err
@mingw32-g++.exe -shared -Wl,--output-def=bin\raydium.def -Wl,--out-implib=bin\raydium.dll -Wl,--dll -Llib bin\index.o   -o bin\raydium.dll  -lphp5ts -lode -lopengl32 -lgdi32 -lglu32 -lws2_32 -ljpeg -lmingwex -lstdc++ -lglew32 -lalut -lopenal32 -lvorbisfile -lvorbis -logg -lwinmm -lintl -lvfw32
@if errorlevel 1 goto err
@if exist bin\index.o (
	del bin\index.o
)
@echo Building Raydium Dll Ok
@goto fin
:err
@echo Building Raydium Dll Fail
@pause
:fin
@set PATH=%back_path%


