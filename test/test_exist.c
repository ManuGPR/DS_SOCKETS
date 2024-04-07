#include <stdio.h>
#include "../claves.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

char *abs_path;

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

void test_1(int k){
    /*Test 1: funcionamiento corecto*/
    printf("======Test 1: todo correcto======\n");
    print_files(abs_path);
    
    // Se borra el archvo
    int exists = exist(k);
    printf("Resultado prueba 1: %d\n", exists);
    print_files(abs_path);
}

void test_2(int k) {
	/* Test 2: no existe ninguna clave*/
    printf("\n======Test 2: no existe ninguna clave======\n");
    
    //Se borran los archivos
    init();
    print_files(abs_path);
    int exists = exist(k);
    printf("Resultado prueba 2: %d\n", exists);

}

int main() {
    //Se obtine la path del diretorio tuplas donde estan almacanadas las key y se abre el directorio
    const char *rel_path = "./tuplas";
    abs_path = realpath(rel_path, NULL);

    //Declaracion de variables
    int n = 1;
    double vector[32];
    int k = 1;

    //Se borran todos lor archivos y se crea un archivo
    init();
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set_value(k, "archivo", n, vector);
	
	// Llamada a los test
	test_1(k);
	test_2(k);
	printf("\n");
	return 0;
}
