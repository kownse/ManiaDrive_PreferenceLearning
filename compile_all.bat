@echo Full Compile Test
@call compile_raydium_dll.bat
@for %%i in (raydium\*.c) do @compile.bat %%~ni
@echo Fin.
@pause
