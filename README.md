# Actividad 2.6: Problema práctico en OpenMP.

# Integrantes:
* Cancelada de la O, Gerardo Alexander
* Hernández Miranda, David Alejandro

# Descripción de la Solución:
Se implementó un algoritmo de búsqueda exhaustiva en C++ utilizando orientación a objetos mediante la clase `BruteForceSolver`. 
El programa evalúa un espacio de búsqueda alfanumérico en base 36 ($A-Z$ y $0-9$) para localizar una clave de prueba introducida por el usuario. 

La solución está optimizada mediante el uso estricto de memoria dinámica y mapeo matemático biyectivo, lo que permite recorrer combinaciones de $36^N$ mediante índices de tipo `unsigned long long` sin saturar la memoria RAM.

Se implementaron dos versiones de búsqueda:
1. **Secuencial:** Recorrido lineal iterativo desde la primera combinación hasta la última.
2. **Paralela (OpenMP):** División equitativa del espacio de búsqueda en subrangos continuos entre los hilos de hardware disponibles, con un mecanismo de parada temprana mediante lectura/escritura sincronizada de banderas compartidas.

# Alfabeto Utilizado:
* **Caracteres:** `A-Z` y `0-9` (36 caracteres en total).
* **Espacio de búsqueda:** $36^N$ combinaciones posibles (calculado dinámicamente según la longitud $N$ de la clave).

# Directivas y Funciones de OpenMP Utilizadas:
* `#pragma omp parallel`: Creación de la región paralela y distribución explícita del trabajo por subrangos según el ID del hilo (`tid`).
* `#pragma omp critical`: Protección de la sección de registro del hilo ganador y copia de la clave encontrada para evitar condiciones de carrera.
* `#pragma omp flush`: Sincronización inmediata de la variable compartida `found_flag` en memoria para notificar la detención a los demás hilos.
* `omp_get_wtime()`: Toma de tiempos de alta precisión antes y después de cada ejecución.
* `omp_get_max_threads()` y `omp_get_thread_num()`: Identificación del número de hilos del procesador e identificador único de cada hilo.

# Instrucciones de Compilación y Ejecución desde Terminal:

# Paso 1: Abrir la Terminal en la Carpeta del Proyecto:
1. Abre la carpeta de Windows donde tienes guardado el archivo `main.cpp`.
2. Haz clic en la barra de direcciones superior (donde se muestra la ruta de la carpeta).
3. Escribe `cmd` o `powershell` y presiona Enter.

# Paso 2: Compilar el Código:
En la ventana de la consola que se acaba de abrir, ejecuta el siguiente comando:

`g++ -O2 -fopenmp main.cpp -o fuerza_bruta.exe`

* `-fopenmp`: Habilita el soporte para las directivas y funciones de OpenMP.
* `-O2`: Aplica optimizaciones del compilador para mediciones de tiempo más precisas.
* `-o fuerza_bruta.exe`: Define el nombre del archivo ejecutable generado.

# Paso 3: Ejecutar el Programa:

* **En CMD (Símbolo del sistema):**
  `fuerza_bruta.exe`

* **En PowerShell:**
  `.\fuerza_bruta.exe`

## Notas de Uso
1. El programa solicitará ingresar una clave de prueba formada únicamente por letras (A-Z) y números (0-9).
2. Valida automáticamente la entrada; en caso de contener caracteres especiales o espacios, solicitará el ingreso nuevamente.
3. Se ejecutan automáticamente las versiones secuencial y paralela mostrando la distribución de rangos, la finalización de los hilos, el hilo ganador y la tabla comparativa de tiempos con el *Speedup* obtenido.
