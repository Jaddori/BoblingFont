@echo off

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

del *.pdb > NUL 2> NUL
cl.exe -Zi -EHsc -W2 -WX -nologo -Febobling_font.exe ..\code\boblingfont.cpp /link /INCREMENTAL:NO user32.lib gdi32.lib
popd
