@echo off
echo Installing Piper TTS...

:: Create folders
mkdir C:\piper
mkdir C:\piper\models

:: Download Piper executable (uses PowerShell for download)
powershell -Command "Invoke-WebRequest -Uri 'https://github.com/rhasspy/piper/releases/download/2023.11.14-2/piper_windows_amd64.zip' -OutFile 'C:\piper\piper.zip'"

:: Extract
powershell -Command "Expand-Archive -Path 'C:\piper\piper.zip' -DestinationPath 'C:\piper' -Force"

:: Download voice model
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx' -OutFile 'C:\piper\models\en_US-lessac-medium.onnx'"

powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json' -OutFile 'C:\piper\models\en_US-lessac-medium.onnx.json'"

:: Cleanup zip
del C:\piper\piper.zip

:: Test it works
echo Testing Piper...
echo Hello, Piper is working correctly. | C:\piper\piper\piper.exe --model C:\piper\models\en_US-lessac-medium.onnx --output_file C:\piper\test.wav

echo.
echo Done! Test audio saved to C:\piper\test.wav
echo Play it to verify everything works.
pause