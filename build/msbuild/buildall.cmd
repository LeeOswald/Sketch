@echo off
cd %~dp0

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64

msbuild Sketch.sln /p:Configuration=DebugNT /p:Platform=x64 /m
msbuild Sketch.sln /p:Configuration=DebugWIN32 /p:Platform=x64 /m
msbuild Sketch.sln /p:Configuration=ReleaseNT /p:Platform=x64 /m
msbuild Sketch.sln /p:Configuration=ReleaseWIN32 /p:Platform=x64 /m

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86

msbuild Sketch.sln /p:Configuration=DebugNT /p:Platform=x86 /m
msbuild Sketch.sln /p:Configuration=DebugWIN32 /p:Platform=x86 /m
msbuild Sketch.sln /p:Configuration=ReleaseNT /p:Platform=x86 /m
msbuild Sketch.sln /p:Configuration=ReleaseWIN32 /p:Platform=x86 /m
