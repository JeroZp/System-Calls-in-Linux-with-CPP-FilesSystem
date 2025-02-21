#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

//ChatGPT para determinar si el archivo es un archivo binario si encuentra un byte fuera del rango ASCII
bool is_binary(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    char ch;
    // Leer los primeros 512 bytes del archivo
    for (int i = 0; i < 512 && file.get(ch); ++i) {
        if (ch < 0 || ch > 127) {
            return true; // Si encontramos un byte no ASCII, es binario
        }
    }
    return false; // Si no encontramos caracteres no ASCII, es texto
}

void compress(const char* filename) {

     if (is_binary(filename)) {
        std::cout << "El archivo " << filename << " es binario." << std::endl;
    } else {
        std::cout << "El archivo " << filename << " es de texto." << std::endl;
    }

    int fd_in = open(filename, O_RDONLY);  // Abre el archivo de entrada
    if (fd_in == -1) {
        std::cerr << "Error abriendo el archivo de entrada." << std::endl;
        return;
    }

    // Determina el nombre del archivo de salida (el que va a comprimirse)
    std::string output_file = std::string(filename) + ".rle";
    int fd_out = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);  // Abre el archivo de salida
    if (fd_out == -1) {
        std::cerr << "Error abriendo el archivo de salida." << std::endl;
        close(fd_in);
        return;
    }

    char prev_char = 0;
    char count = 1;  // Usa un byte para contar las repeticiones
    char current_char;

    // Lee el archivo byte por byte
    while (read(fd_in, &current_char, 1) > 0) {
        if (current_char == prev_char) {
            count++;
            //Con ayuda de ChatGPT
            if (count == 255) {  // Limitar el contador a 255 (el valor máximo de un char)
                write(fd_out, &count, 1);  // Escribir el número de repeticiones
                write(fd_out, &prev_char, 1);  // Escribir el carácter
                count = 1;  // Reiniciar el contador
            }
        } else {
            // Si el carácter cambia, escribe el contador y el carácter anterior
            if (prev_char != 0) {
                write(fd_out, &count, 1);  // Escribe el número de repeticiones
                write(fd_out, &prev_char, 1);  // Escribe el carácter
            }
            // Reinicia el contador y actualiza el carácter anterior
            count = 1;
            prev_char = current_char;
        }
    }

    // Escribe el último grupo de caracteres
    if (prev_char != 0) {
        write(fd_out, &count, 1);  
        write(fd_out, &prev_char, 1);  
    }

    close(fd_in);  // Cierra archivo de entrada
    close(fd_out); // Cierra archivo de salida
}


void encrypt(const char* filename, int shift) {

    if (is_binary(filename)) {
        std::cout << "El archivo " << filename << " es binario." << std::endl;
    } else {
        std::cout << "El archivo " << filename << " es de texto." << std::endl;
    }

    int fd_in = open(filename, O_RDONLY);  // Abre archivo de entrada
    if (fd_in == -1) {
        std::cerr << "Error abriendo el archivo para encriptar.\n";
        return;
    }

    // Determina el nombre del archivo de salida (el que va a encriptar)
    std::string output_file = std::string(filename) + ".enc";
    int fd_out = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);  // Abre archivo de salida
    if (fd_out == -1) {
        std::cerr << "Error abriendo el archivo de salida.\n";
        close(fd_in);
        return;
    }

    char current_char;

    // Lee el archivo byte por byte
    while (read(fd_in, &current_char, 1) > 0) {
        // Encripta mediante el metodo Cesar, desplaza el byte por el valor de shift
        char encrypted_char = current_char + shift;
        write(fd_out, &encrypted_char, 1); 
    }

    close(fd_in);  // Cierra el archivo de entrada
    close(fd_out); // Cierra el archivo de salida
}

void decrypt(const char* filename, int shift) {
   
    int fd_in = open(filename, O_RDONLY);
    if (fd_in == -1) {
        std::cerr << "Error abriendo el archivo para desencriptar.\n";
        return;
    }

    // Determina el nombre del archivo de salida (el que va a desencriptar)
    std::string output_file = std::string(filename) + ".dec";
    int fd_out = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_out == -1) {
        std::cerr << "Error abriendo el archivo de salida.\n";
        close(fd_in);
        return;
    }

    char current_char;

    // Lee el archivo byte por byte y lo desencripta
    while (read(fd_in, &current_char, 1) > 0) {
        // Desencripta restando el desplazamiento
        char decrypted_char = current_char;
        write(fd_out, &decrypted_char, 1);
    }

    close(fd_in);  // Cierra archivo de entrada
    close(fd_out); // Cierra archivo de salida

    std::cout << "Archivo desencriptado correctamente: " << output_file << std::endl;
}

void decompress(const char* filename) {
    // Abre el archivo comprimido
    int fd_in = open(filename, O_RDONLY);
    if (fd_in == -1) {
        std::cerr << "Error abriendo el archivo para descomprimir.\n";
        return;
    }

    // Determina el nombre del archivo de salida (el archivo a descromprimir)
    std::string output_file = std::string(filename) + ".dec";
    int fd_out = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_out == -1) {
        std::cerr << "Error abriendo el archivo de salida.\n";
        close(fd_in);
        return;
    }

    char count_char, data_char;

    // Lee y descomprime el archivo
    while (read(fd_in, &count_char, 1) > 0) {
        // El primer byte es el número de repeticiones
        int count = static_cast<unsigned char>(count_char);  // Convierte el byte a entero

        // Lee el siguiente byte que es el carácter a repetir
        if (read(fd_in, &data_char, 1) <= 0) {
            std::cerr << "Error leyendo el archivo comprimido.\n";
            break;
        }

        // Escribe el carácter repetido 'count' veces
        for (int i = 0; i < count; ++i) {
            write(fd_out, &data_char, 1);
        }
    }

    close(fd_in); // Cierra el archivo de entrada
    close(fd_out); // Cierra el archivo de salida

    std::cout << "Archivo descomprimido exitosamente: " << output_file << std::endl;
}

void show_help() {
    std::cout << "Uso: \n";
    std::cout << "-h o --help: Muestra este mensaje de ayuda.\n";
    std::cout << "-v o --version: Muestra la versión del programa.\n";
    std::cout << "-c <archivo> o --compress <archivo>: Comprime el archivo.\n";
    std::cout << "-x <archivo> o --decompress <archivo>: Descomprime el archivo.\n";
    std::cout << "-e <archivo> o --encrypt <archivo>: Encripta el archivo.\n";
    std::cout << "-d <archivo> o --decrypt <archivo>: Desencripta el archivo.\n";
}

void show_version() {
    std::cout << "Versión 1.0\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        show_help();
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        show_help();
    } else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        show_version();
    } else if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--compress") == 0) {
        if (argc < 3) {
            std::cerr << "Se debe especificar el archivo a comprimir.\n";
            return 1;
        }
        compress(argv[2]);
    } else if (strcmp(argv[1], "-x") == 0 || strcmp(argv[1], "--decompress") == 0) {
        if (argc < 3) {
            std::cerr << "Se debe especificar el archivo a descomprimir.\n";
            return 1;
        }
        decompress(argv[2]);
    } else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--encrypt") == 0) {
        if (argc < 4) {
            std::cerr << "Se debe especificar el archivo a encriptar y el desplazamiento (shift).\n";
            return 1;
        }

        int shift = std::stoi(argv[3]);  // Convertir el valor de shift desde el argumento - ChatGPT
        encrypt(argv[2], shift);  // Pasa el archivo y el desplazamiento a la función
    } else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--decrypt") == 0) {
        if (argc < 4) {
            std::cerr << "Se debe especificar el archivo a desencriptar.\n";
            return 1;
        }

        int shift = std::stoi(argv[3]);  // Convertir el valor de shift desde el argumento - ChatGPT
        decrypt(argv[2], shift);
    } else {
        show_help();
    }

    return 0;
}