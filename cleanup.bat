echo off
del Makefile.win
del *.depend
del *.layout
del bin\*raydium*.*
del bin\*.exe
del bin\*.o
del bin\*.txt
del bin\*.tri
del bin\*.blend
del bin\*.tga
del bin\*.lf
del bin\*.pro
del bin\*.beg
del bin\*.mni
del bin\*.cam
del bin\*.cam1
del bin\*.kmz
del bin\*.dae
del bin\*.xml
del raydium\raydium\*.o
del tests\*.layout
del tests\*.depend
del tests\*.save
del tests\*.bmarks
del /s tests\*.o
del /s *.save
rd /q /s tests\obj
rd /q /s tests\.objs

