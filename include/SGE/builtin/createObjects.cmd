@REM gcc -c builtin/meshes/meshes.c -lSL

gcc -c builtin/extData/daylightCycle.c -lSL
gcc -c builtin/extData/freeCam.c -lSL
gcc -c builtin/extData/mouseCam.c -lSL

gcc -c builtin/postEffects/blur.c -lSL
gcc -c builtin/postEffects/bloom.c -lSL
gcc -c builtin/postEffects/SSAO.c -lSL

gcc -c builtin/objects/trail.c -lSL

gcc -c builtin/backgrounds/simpleSky.c -lSL
exit