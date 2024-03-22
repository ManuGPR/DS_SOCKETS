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

int leer_fichero(char *abs_path, int k){

    // Escribe los datos
    FILE *archivo;
    //char caracter[10];
    char number[20];
    sprintf(number, "%i",k);

    char nombre_archivo[100];
    memset(&nombre_archivo, 0, sizeof(nombre_archivo));
    strcat(nombre_archivo, abs_path);
    strcat(nombre_archivo, "/");
    strcat(nombre_archivo, number);


    // Abre el archivo en modo lectura
    archivo = fopen(nombre_archivo, "r");
    if(archivo == NULL){
        printf("Error al abrir el fichro");
        return -1;
    }
    // Lee y muestra cada caracter del archivo
    char linea[100];
    fgets(linea, sizeof(linea), archivo);
    printf("K: %s", linea);
    fgets(linea, sizeof(linea), archivo);
    printf("Value1: %s", linea);
    fgets(linea, sizeof(linea), archivo);
    printf("N: %s", linea);
    printf("Value2: ");
    while(fgets(linea, sizeof(linea), archivo) !=NULL){
        printf("%s\n", linea);
    }

    // Cierra el archivo
    fclose(archivo);
    printf("\n");
    return 0;
}


int main(){
    //Se obtine la path del diretorio tuplas donde estan almacanadas las key y se abre el directorio
    const char *rel_path = "./tuplas";
    char *abs_path;
    abs_path = realpath(rel_path, NULL);

    //Se borran todos lor archivos y se crea un archivo
    init();
    int  modify;
    int n = 1;
    int k = 1;
    double *vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set_value(k, "archivo", n, vector);

    printf("\n=========Test de modify_value=========\n");
    /*Test 1: funcionamiento corecto*/
    printf("Test 1: todo correcto\n");

    print_files(abs_path);
    printf("Los datos que ha en el archivo creado: \n");
    leer_fichero(abs_path, k);

    //Se modifica el archivo
    n = 3;
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    modify = modify_value(k, "archivo_cambiado", n , vector);
    printf("Resultado prueba 1: %d\n", modify);

    print_files(abs_path);
    printf("Los datos que ha en el archivo modificado: \n");
    leer_fichero(abs_path, k);
	free(vector);

    /*Test 2: no hay ningun archivo*/
    printf("\nTest 2: no existe ninguna clave\n");
    //Se borran los archivos
    init();
    print_files(abs_path);
    vector = calloc(n, sizeof(double));
    modify = modify_value(k, "archivo_cambiado", n , vector);
    printf("Resultado prueba 2: %d\n", modify);
	free(vector);

    /*Test 3: se modifica el archivo y N = 0 */
    printf("\nTest 3: N = 0 \n");
    //Se borran todos lor archivos y se crea un archivo
    init();
    n = 1;
    vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set_value(k, "archivo", n, vector);
    printf("Los datos que ha en el archivo creado: \n");
    leer_fichero(abs_path, k);
	free(vector);
    //Se modifica el archivo
    n = 0;
    vector = calloc(n, sizeof(double));
    modify = modify_value(k, "archivo_cambiado", n , vector);
    printf("Resultado prueba 3: %d\n", modify);
	free(vector);
    printf("Los datos del archivo: \n");
    leer_fichero(abs_path, k);

    /*Test4: Modifica el archivo*/
    printf("\nTest 4: N = 32\n");
    //Se modifica el archivo
    n = 32;
    vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) {vector[i] = (double) i; }
    modify = modify_value(k, "archivo_cambiado", n , vector);
    printf("Resultado prueba 4: %d\n", modify);
	free(vector);

    /*Test 5: Modifica el archivo*/
    printf("\nTest 5: N = 33\n");
    //Se modifica el archivo
    n = 33;
    vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) {vector[i] = (double) i; }
    modify = modify_value(k, "archivo_cambiado", n , vector);
    printf("Resultado prueba 5: %d\n", modify);
    free(vector);

}
