#include <stdio.h>
#include "../claves.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

int print_files(char *abs_path) {
    //Imprime por pantalla los archivos que hay en el directorio tuplas
    DIR *dir = opendir(abs_path);
    struct dirent *tuplas;
    printf("En el directorio tuplas hay los siguientes archivos:  ");
    while ((tuplas = readdir(dir)) != NULL) {
        printf("%s  ", tuplas->d_name);
    }
    printf("\n");
    return 0;
}



int main(){
    printf("\n=========Test de exist=========\n");

    //Se obtine la path del diretorio tuplas donde estan almacanadas las key y se abre el directorio
    const char *rel_path = "./tuplas";
    char *abs_path;
    abs_path = realpath(rel_path, NULL);

    //Declaracion de variables
    int exits;
    int n = 1;
    double vector[32];
    int k = 1;

    //Se borran todos lor archivos y se crea un archivo
    init();
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set_value(k, "archivo", n, vector);

    /*Test 1: funcionamiento corecto*/
    printf("Test 1: todo correcto\n");
    print_files(abs_path);
    //Se borra el archvo
    exits = exist(k);
    printf("Resultado prueba 1: %d\n", exits);
    print_files(abs_path);

    /*Test 2 se exite ninguna clave*/
    printf("\nTest 2: no existe ninguna clave\n");
    //Se borran los archivos
    init();
    print_files(abs_path);

    exits = exist(k);
    printf("Resultado prueba 2: %d\n", exits);
}
