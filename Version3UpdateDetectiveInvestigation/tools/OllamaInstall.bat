@echo off
echo Descargando Ollama...
winget install Ollama.Ollama

echo Descargando Llama3...
ollama pull llama3

echo Listo! Ejecuta ollama serve antes de correr el programa.
pause