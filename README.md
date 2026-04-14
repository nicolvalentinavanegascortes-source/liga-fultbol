# Sistema de Gestión de Liga de Fútbol en C++

## Estudiante

Nicol Valentina Vanegas Cortés

## Descripción del programa

Este programa en C++ simula la gestión de una liga de fútbol desde consola. Permite organizar equipos, registrar partidos, calcular automáticamente la tabla de posiciones y mostrar estadísticas generales. Además, incluye funciones para generar una gráfica en consola, crear un ranking de equipos, simular partidos automáticamente y exportar resultados a un archivo CSV. Todo esto se realiza utilizando estructuras de datos y manejo de archivos.

## Instrucciones para compilar

Para compilar el programa es necesario contar con un compilador g++, como MinGW. El comando de compilación estándar es: g++ main.cpp -o liga.exe -mconsole. En caso de usar PowerShell y presentar errores, se puede utilizar el comando alternativo: g++ main.cpp -o liga.exe "-Wl,--subsystem,console".

## Instrucciones para ejecutar

Después de compilar, se debe ejecutar el programa con el comando: .\liga.exe en la terminal. Al ejecutarse, se mostrará un menú interactivo que permite acceder a todas las funcionalidades disponibles.

## Formato de config.txt

El archivo config.txt define la configuración de la liga. Debe incluir el nombre de la liga, los puntos por victoria, empate y derrota, y los equipos participantes. Cada línea sigue el formato clave=valor. Un ejemplo sería una liga llamada Liga Colombiana con 3 puntos por victoria, 1 por empate y 0 por derrota, seguida de equipos como Millonarios, Nacional, Santa Fe y Cali.

## Decisiones de diseño

El programa utiliza estructuras (struct) para organizar los datos en tres tipos principales: ConfigLiga, Partido y Equipo, lo que permite mantener la información ordenada y clara. También emplea vectores (std::vector) para manejar listas dinámicas de equipos y partidos.

El código está modularizado en funciones específicas para tareas como lectura de archivos, validación de datos, construcción de la tabla y visualización de información, lo que mejora la organización y el mantenimiento del programa.

Se implementa manejo de archivos mediante ifstream y ofstream, verificando siempre que los archivos se abran correctamente con is_open(). Además, se utilizan punteros en la función de actualización de equipos para modificar directamente los datos sin generar copias innecesarias.

El ordenamiento de la tabla se realiza con la función sort de la biblioteca algorithm, usando una función lambda que organiza los equipos según puntos, diferencia de gol y goles a favor.

Finalmente, el programa incluye validaciones como la verificación de existencia de equipos y la detección de partidos duplicados, así como una función de simulación que permite generar partidos automáticamente para pruebas.
