# Packet_Siffer_pro
Packet sniffer multiplataforma usando la libreria pcap con interfaz grafica similar a WireShark, Proyecto Final Redes I


## Puesta a punto
1. Instalar Herramientas Basicas

- Este proyecto usa CMake para ser compilado en diferentes plataformas, para ello es fundamental tener instalado [CMake](https://cmake.org/download/). Cuando lo instalas, asegurate de marcar la opcion de **Agregar al PATH**.
- Ademas, debes de asegurarte que cuentes con el compilador g++ de 32 bits, en windows, es necesario que sea el que descargas con MinGW
- Otra herramienta necesaria es Makefile, si estas en windows normalmente se instala con las herramientas de MinGW, si no la tienes instalala

Para Asegurarte que tienes todas estas herramientas bien configuradas, puedes verificar en la terminal con los siguientes comandos
```Bash
    # CMake
    cmake --version
    
    # g++ 
    g++ --version

    # Makefile en windows
    mingw32-make --version

    # Makefile en Linux
    make --version
```
2. Modificar el CMakeLists.txt

Para que el CMakeList y en general la compilacion funcione bien, debes de incluir las rutas de los include y las libs de las librerias externas que usamos (por el momento, solo Npcap) de la siguiente manera:
- Busca las lineas include_directories y link_directories, encontraras dos de cada una, las primeras son las que toma en windows y las segundas las de linux
- Modifica las que pertenecen a Windows poniendo tu ruta a la carpeta Include y Libs respectivamente del Npcap, en la parte de Linux no debes de modificar nada.

Ejemplo de como te debe de quedar

```bash
    if (WIN32)
        ...configuraciones extra de windows
        include_directories("C:\\Tu\\ruta\\al\\pcap\\Include")
        link_directories("Tu\\ruta\\al\\pcap\\Lib")
        ...
    elseif(UNIX)
        ...configuraciones de Linux 
```

## Compilacion del proyecto

Suponiendo que ya tienes hechos los pasos de la puesta a punto (Tambien debes verificar el paso 2 cada que hagas pull)

1. Ejecutar CMake

En la terminal de comandos, dirigete hacia el directorio del proyecto, despues entra en el directorio build (Creala si no esta hecha), y dentro de dicho directorio ejecuta el comando
```bash
    cmake .. -G "MinGW Makefiles"
```
Esto creara diferentes archivos en la carpeta build, entre ellas debera aparecer un archivo Makefile

2. Ejecutar el Makefile

Ahora, en la misma terminal donde hiciste el cmake, ejecuta el siguiente comando:

```bash
    # En windows
    mingw32-make

    # En linux 
    make
```

Esto compilara el proyecto y generara un ejecutable listo para ser llamado con el comando

```bash
    # En windows
    ./Packet_sniffer_pro.exe

    # En linux
    sudo ./Packet_sniffer_pro
```

