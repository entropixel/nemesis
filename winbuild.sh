#!/bin/sh

SDL2=/home/kyle/win32-sdl2
zipname=nem-win32-$(date +"%m%d%y-%H%M").zip

make CC=i686-w64-mingw32-gcc SDL2_PATH=$SDL2 OUT=nemesis.exe
i686-w64-mingw32-strip nemesis.exe

zip /tmp/$zipname nemesis.exe img/*
cd $SDL2/bin
zip /tmp/$zipname *dll
