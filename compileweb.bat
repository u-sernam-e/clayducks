setlocal EnableDelayedExpansion

set input=

for %%a in (source/*.cpp) do set input=!%input%! %%~a

echo %input%

for %%a in (C:/dev/projects/superpooplib/src/*.cpp) do echo %%a

call C:/dev/emsdk/emsdk_env.bat
call em++ -o index.html %input% -Os -Wall C:/dev/raylib/srcweb/libraylib.a -IC:/dev/raylib/srcweb -Iheaders -IC:/dev/projects/superpooplib/headers -L C:/dev/raylib/srcweb/libraylib.a -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall -s EXPORTED_FUNCTIONS="['_malloc', '_main']" --shell-file C:/dev/raylib/srcweb/shell.html -DPLATFORM_WEB --preload-file res -Wno-narrowing -std=c++20

AAAAAAAHHHHHHHHHHHHHHHHH