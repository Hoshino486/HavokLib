@echo off
REM HavokLib build script (MSVC x64)
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
set PATH=C:\Users\34184\AppData\Local\Programs\Python\Python313\Scripts;%PATH%
set PYBIN=C:\Users\34184\AppData\Local\Programs\Python\Python313\python.exe
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DPython3_EXECUTABLE="%PYBIN%" %*
if errorlevel 1 exit /b 1
cmake --build build --parallel
