@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "root=%~dp0"
set "outroot=%root%compressed"

if not exist "%outroot%" mkdir "%outroot%"

echo Recherche des fichiers .wav dans %root%
for /r "%root%" %%F in (*.wav) do (
  set "name=%%~nF"
  if /I not "!name:~-11!"=="_compressed" (
    echo Compression de : %%~fF
    ffmpeg -y -i "%%~fF" -ac 1 -ar 44100 -c:a pcm_u8 "%outroot%\%%~nF_compressed.wav"
  ) else (
    echo Skip : %%~fF
  )
)
echo Conversion terminee.
pause