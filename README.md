# Proyecto de Compresión y Encriptación

Este proyecto implementa dos algoritmos: **Run-Length Encoding (RLE)** para compresión y **Cifrado César** para encriptación y desencriptación. El programa permite comprimir y descomprimir archivos utilizando RLE y encriptar o desencriptar archivos utilizando Cifrado César.

## Algoritmos Utilizados

### **Run-Length Encoding (RLE)**

**Run-Length Encoding (RLE)** es un algoritmo de compresión de datos sin pérdida. RLE reemplaza las secuencias de datos repetidos con una única instancia de ese dato y la cantidad de repeticiones.

#### Funcionamiento:
1. Se examina el archivo y, por cada secuencia de caracteres repetidos, se reemplaza por el número de repeticiones seguido del carácter.
2. Ejemplo:
``` AAAABBBCCCCD -> 4A3B4C1D ```
3. La descompresión es simplemente el proceso inverso: leer la cantidad de repeticiones y expandir el carácter correspondiente.

### **Cifrado César**

El **Cifrado César** es un algoritmo de encriptación que reemplaza cada letra del texto por otra que esté un número fijo de posiciones más adelante en el alfabeto. Es un cifrado de sustitución simple.

#### Funcionamiento:
1. Cada letra del texto original se desplaza por un número fijo de posiciones en el alfabeto.
2. Ejemplo:
- Texto original: `HELLO`
- Desplazamiento de `3`: `KHOOR`
3. Para desencriptar, se invierte el desplazamiento. Si se usó un desplazamiento de `3` para cifrar, se usa un desplazamiento de `-3` para descifrar.

---

## Instrucciones de Uso

### Requisitos
- El proyecto fue desarrollado en **C++**.
- Debes tener un compilador de C++ instalado, como **g++**.

### Compilación

1. Abre una terminal y navega hasta el directorio donde se encuentra el código fuente.
2. Compila el programa utilizando el siguiente comando:
```bash
g++ parcial1.cpp -o parcial1
```
### Ejecución
El programa proporciona las siguientes opciones para comprimir, descomprimir, encriptar y desencriptar archivos:

Comprimir con RLE:
``` ./parcial1 -c archivo.txt ```
Para acceder a las demas funciones del código ejecuta:
``` ./parcial1 -h ```
