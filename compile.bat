@echo Building Example %1
@set back_path=%PATH%
@PATH CodeBlocks\MinGW\bin;
@if exist bin\%1.exe (
	@del bin\%1.exe
)

@mingw32-gcc.exe -w -DRAYDLL -DFORCE_LIBRAYDIUM -fexpensive-optimizations -O3  -Iinclude\php -Iinclude\php\main -Iinclude\php\regex -Iinclude\php\TSRM -Iinclude\php\Zend -Iinclude\ode\include -Iinclude -Iraydium -c raydium\%1.c -o bin\%1.o
@if errorlevel 1 goto err
@mingw32-g++.exe -Lbin -Llib -o bin\%1.exe bin\%1.o   -s  -lraydium -lphp5ts -lode -lopengl32 -lgdi32 -lglu32 -lws2_32 -ljpeg -lmingwex -lstdc++ -lglew32 -lalut -lopenal32 -lvorbisfile -lvorbis -logg -lintl -lwinmm -lvfw32 -lAR
@if errorlevel 1 goto err
@if exist bin\%1.o (
	@del bin\%1.o
)
@echo Building Example %1 Ok
@goto fin
:err
@echo Building Example %1 Fail
@pause
:fin
@set PATH=%back_path%