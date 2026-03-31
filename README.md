# DetectiveAI

Para Ejecutar Cualquier Version del detective, primero se debe ejecutar el gendeps.bat, despues el genproj.bat 
Luego en el proyecto de Visual, ir a los ajustes de MainDetective y en el apartado de Depuración asegurarse de que en el directorio de trabajo se $(TargetDir) y NO $(ProjectDir)

(solo para la version 3 del detective):
Este proyecto usa IA externa para generar la historia y un audio basado en dicha historia, si no tiene Piper o Ollama, simplemente ejecute los script OllamaInstall y Instal_Piper que se proporciona en la carpeta tools.
Todo el contenido que se genera durante la ejecución del programa se guardará en la siguiente ruta:
"build\MainDetective\Debug\data"
Si todo funcionó como debe, seran los archivos "Story.txt" y "narration.wav"

Gracias

---------------------------------------------------------------------------------------------------------------------------------------------------------------------

To run any version of the detective, first run gendeps.bat, then genproj.bat. 
Then, in your Visual Studio project, go to the MainDetective settings and in the Debugging section, make sure that the working directory is set to $(TargetDir) and NOT $(ProjectDir).

(Only for version 3 of the detective):
This proyect uses external AI to generate the history and an audio file that bases itself from the previous story, if you do not have Piper or Ollama, simply execute the scripts OllamaInstall and Instal_Piper that are granted in the tools folder.
All the content that is generated during the execution of the program will be saved in the next route:
"build\MainDetective\Debug\data"
If everything worked as planed, the files are "Story.txt" and "narration.wav"

Thanks
