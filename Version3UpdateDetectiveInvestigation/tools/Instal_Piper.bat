@echo off
echo Installing Piper TTS with multiple voices...

:: Create folders
mkdir C:\piper
mkdir C:\piper\models

:: Download Piper executable
powershell -Command "Invoke-WebRequest -Uri 'https://github.com/rhasspy/piper/releases/download/2023.11.14-2/piper_windows_amd64.zip' -OutFile 'C:\piper\piper.zip'"

:: Extract
powershell -Command "Expand-Archive -Path 'C:\piper\piper.zip' -DestinationPath 'C:\piper' -Force"

:: Cleanup zip
del C:\piper\piper.zip

echo Downloading voice models...

:: ---- FEMALE VOICES ----

:: Female - US English (medium quality)
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx' -OutFile 'C:\piper\models\en_US-lessac-medium.onnx'"
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json' -OutFile 'C:\piper\models\en_US-lessac-medium.onnx.json'"

:: Female - US English (high quality)
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/amy/medium/en_US-amy-medium.onnx' -OutFile 'C:\piper\models\en_US-amy-medium.onnx'"
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/amy/medium/en_US-amy-medium.onnx.json' -OutFile 'C:\piper\models\en_US-amy-medium.onnx.json'"

:: Female - UK English
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_GB/alba/medium/en_GB-alba-medium.onnx' -OutFile 'C:\piper\models\en_GB-alba-medium.onnx'"
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_GB/alba/medium/en_GB-alba-medium.onnx.json' -OutFile 'C:\piper\models\en_GB-alba-medium.onnx.json'"

:: ---- MALE VOICES ----

:: Male - US English
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/ryan/medium/en_US-ryan-medium.onnx' -OutFile 'C:\piper\models\en_US-ryan-medium.onnx'"
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/ryan/medium/en_US-ryan-medium.onnx.json' -OutFile 'C:\piper\models\en_US-ryan-medium.onnx.json'"

:: Male - UK English
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_GB/alan/medium/en_GB-alan-medium.onnx' -OutFile 'C:\piper\models\en_GB-alan-medium.onnx'"
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_GB/alan/medium/en_GB-alan-medium.onnx.json' -OutFile 'C:\piper\models\en_GB-alan-medium.onnx.json'"

:: Male - US English (deep voice)
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/danny/low/en_US-danny-low.onnx' -OutFile 'C:\piper\models\en_US-danny-low.onnx'"
powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/danny/low/en_US-danny-low.onnx.json' -OutFile 'C:\piper\models\en_US-danny-low.onnx.json'"

:: ---- TEST ALL VOICES ----

echo Testing all voices...

echo Testing Female US (lessac)...
echo Hello, I am Lessac, a female US English voice. | C:\piper\piper\piper.exe --model C:\piper\models\en_US-lessac-medium.onnx --output_file C:\piper\test_lessac.wav
echo Hello, I am Amy, a female US English voice. | C:\piper\piper\piper.exe --model C:\piper\models\en_US-amy-medium.onnx --output_file C:\piper\test_amy.wav
echo Hello, I am Alba, a female UK English voice. | C:\piper\piper\piper.exe --model C:\piper\models\en_GB-alba-medium.onnx --output_file C:\piper\test_alba.wav
echo Hello, I am Ryan, a male US English voice. | C:\piper\piper\piper.exe --model C:\piper\models\en_US-ryan-medium.onnx --output_file C:\piper\test_ryan.wav
echo Hello, I am Alan, a male UK English voice. | C:\piper\piper\piper.exe --model C:\piper\models\en_GB-alan-medium.onnx --output_file C:\piper\test_alan.wav
echo Hello, I am Danny, a deep male US English voice. | C:\piper\piper\piper.exe --model C:\piper\models\en_US-danny-low.onnx --output_file C:\piper\test_danny.wav

echo.
echo ================================
echo All voices installed and tested!
echo Test files saved to C:\piper\
echo ================================
echo Female US:  test_lessac.wav, test_amy.wav
echo Female UK:  test_alba.wav
echo Male US:    test_ryan.wav, test_danny.wav (deep)
echo Male UK:    test_alan.wav
echo ================================
pause