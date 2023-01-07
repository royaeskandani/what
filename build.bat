@echo on
cd /d "C:\Program Files\Webots\msys64\mingw64\bin\"
gcc.exe -Wall -O0 "%~dp0code\*.c" -o "%~dp0project\_project.exe" -D PC
cd "%~dp0project"
_project.exe
