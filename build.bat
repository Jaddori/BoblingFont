@echo off

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

del *.pdb > NUL 2> NUL
cl.exe -DWIN32 -MD -Zi -EHsc -W2 -WX -nologo -Febobling_font.exe ..\code\bfont_win32.cpp /ID:\dev\git\boblingpng\code\ /link /IGNORE:4098 /INCREMENTAL:NO /LIBPATH:D:\dev\git\boblingpng\build\ /LIBPATH:C:\libpng\bin\ /LIBPATH:C:\zlib\bin user32.lib gdi32.lib bpng_d.lib libpng_d.lib zlib_d.lib
popd
