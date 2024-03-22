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

    printf("El contenido del fichero es:\n");
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
    return 0;
}

int main() {
    int set;
    const char *rel_path = "./tuplas";
    char *abs_path = calloc(4096, sizeof(char));
    abs_path = realpath(rel_path, NULL);
    printf("\n=========Test de set value=========\n");	    
    init();
    
    /*Test 1: Crea un archivo */
    printf("Test 1: funcionamiento para N = 2\n");
    print_files(abs_path);
    int n = 1;
    int k = 1;
    double *vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set = set_value(k, "prueba_1", n, vector);
    printf("Resultado prueba 1: %d\n", set);
    print_files(abs_path);
    if (set == 0){leer_fichero(abs_path, k);}
	free(vector);
	
    /*Test 2: ya exite la key*/
    printf("\nTest 2: key ya existe\n");
    print_files(abs_path);
    n = 1;
    k = 1;
    vector = calloc(n, sizeof(double));

    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set = set_value(k, "prueba_2", n, vector);
    printf("Resultado prueba 2: %d\n", set);
    print_files(abs_path);
    if (set == 0){ leer_fichero(abs_path, k);}
    free(vector);

    /*Test 3: N = 0*/
    printf("\nTest 3: N=0\n");
    n = 0;
    k = 3;
    print_files(abs_path);
    vector = calloc(n, sizeof(double));

    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set = set_value(k, "prueba_3", n, vector);
    printf("Resultado prueba 3 (N=0): %d\n", set);
    print_files(abs_path);
    if (set == 0){leer_fichero(abs_path, k);}
	free(vector);
	
    /*Test 4: N = 2 */
    printf("\nTest 4: N=2\n");
    n = 2;
    print_files(abs_path);
    k = 4;
    vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set = set_value(k, "prueba_4", n, vector);
    printf("Resultado prueba 4 (N=2): %d\n", set);
    print_files(abs_path);
    if (set == 0){leer_fichero(abs_path, k);}
    free(vector);
	
    /*Test 5: N = 31 */
    printf("\nTest 5: N=31\n");
    print_files(abs_path);
    n = 31;
    k = 5;
    vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) {vector[i] = (double) i;}
    set = set_value(k, "prueba_5", n, vector);
    printf("Resultado prueba 5 (N=31): %d\n", set);
    print_files(abs_path);
    if (set == 0){ leer_fichero(abs_path, k);}
	free(vector);

    /*Test 6: N = 32 */
    printf("\nTest 6: N=32\n");
    print_files(abs_path);
    n = 32;
    k = 6;
    vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set = set_value(k, "prueba_6", n, vector);
    printf("Resultado prueba 6 (N=32): %d\n", set);
    print_files(abs_path);
    if (set == 0){leer_fichero(abs_path, k);}
	free(vector);

    /*Test 7: N = 33 */
    printf("\nTest 7: N=33\n");
    print_files(abs_path);
    n = 33;
    k = 7;
    vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    set = set_value(k, "prueba_7", n, vector);
    printf("Resultado prueba 7 (N=33): %d\n", set);
    print_files(abs_path);
    if (set == 0){leer_fichero(abs_path, k);}
	free(vector);

    /*Test 8: len(cadena) > 255 */
    printf("\nTest 8: len(cadena) > 255 \n");
    n = 1;
    k = 8;
    vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    char cadena[300];
    for (int i = 0; i < 50; i++){ strcat(cadena, "prueba");}

    set = set_value(k, cadena, n, vector);
    printf("Resultado prueba 8 (len(cadena) >255): %d\n", set);
    print_files(abs_path);
    if (set == 0){ leer_fichero(abs_path, k);}
	free(vector);

    return 0;
}
