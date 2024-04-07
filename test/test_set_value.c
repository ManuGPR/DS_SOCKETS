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

int leer_fichero(char *abs_path, int k){

    // Escribe los datos
    FILE *archivo;
    
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
        printf("Error al abrir el fichero\n");
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

void test_1() {
	/*Test 1: Crea un archivo */
    printf("======Test 1: funcionamiento para N = 1 ======\n");
    print_files(abs_path);
    int n = 1;
    int k = 1;
    double *vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    int set = set_value(k, "prueba_1", n, vector);
    printf("Resultado prueba 1 (todo bien): %d\n", set);
    print_files(abs_path);
    printf("\n");
    if (set == 0){leer_fichero(abs_path, k);}
	free(vector);
}

void test_2() {
	/*Test 2: ya existe la key*/
    printf("\n======Test 2: key ya existe======\n");
    print_files(abs_path);
    int n = 1;
    int k = 1;
    double *vector = calloc(n, sizeof(double));

    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    int set = set_value(k, "prueba_2", n, vector);
    printf("Resultado prueba 2 (key existe): %d\n", set);
    print_files(abs_path);
    if (set == 0){ leer_fichero(abs_path, k);}
    free(vector);

}

void test_3() {
	/*Test 3: N = 0*/
    printf("\n======Test 3: N=0 ======\n");
    int n = 0;
    int k = 3;
    print_files(abs_path);
    double * vector = calloc(n, sizeof(double));

    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    int set = set_value(k, "prueba_3", n, vector);
    printf("Resultado prueba 3 (N=0): %d\n", set);
    print_files(abs_path);
    if (set == 0){leer_fichero(abs_path, k);}
	free(vector);
	
}

void test_4() {
	/*Test 4: N = 2 */
    printf("\n======Test 4: N=2 ======\n");
    int n = 2;
    print_files(abs_path);
    int k = 4;
    double *vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    int set = set_value(k, "prueba_4", n, vector);
    printf("Resultado prueba 4 (N=2): %d\n", set);
    print_files(abs_path);
    printf("\n");
    if (set == 0){leer_fichero(abs_path, k);}
    free(vector);
	
}

void test_5() {
	/*Test 5: N = 31 */
    printf("\n======Test 5: N=31 ======\n");
    print_files(abs_path);
    int n = 31;
    int k = 5;
    double *vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) {vector[i] = (double) i;}
    int set = set_value(k, "prueba_5", n, vector);
    printf("Resultado prueba 5 (N=31): %d\n", set);
    print_files(abs_path);
    printf("\n");
    if (set == 0){ leer_fichero(abs_path, k);}
	free(vector);

}

void test_6() {
	/*Test 6: N = 32 */
    printf("\n======Test 6: N=32 ======\n");
    print_files(abs_path);
    int n = 32;
    int k = 6;
    double *vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    int set = set_value(k, "prueba_6", n, vector);
    printf("Resultado prueba 6 (N=32): %d\n", set);
    print_files(abs_path);
    printf("\n");
    if (set == 0){leer_fichero(abs_path, k);}
	free(vector);

}

void test_7() {
	/*Test 7: N = 33 */
    printf("\n======Test 7: N=33 ======\n");
    print_files(abs_path);
    int n = 33;
    int k = 7;
    double *vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    int set = set_value(k, "prueba_7", n, vector);
    printf("Resultado prueba 7 (N=33): %d\n", set);
    print_files(abs_path);
    if (set == 0){leer_fichero(abs_path, k);}
	free(vector);

}

void test_8() {
	/*Test 8: len(cadena) > 255 */
    printf("\n======Test 8: len(cadena) > 255 ======\n");
    int n = 1;
    int k = 8;
    double *vector = calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    char cadena[300];
    for (int i = 0; i < 50; i++){ strcat(cadena, "prueba");}

    int set = set_value(k, cadena, n, vector);
    printf("Resultado prueba 8 (len(cadena) >255): %d\n", set);
    print_files(abs_path);
    if (set == 0){ leer_fichero(abs_path, k);}
	free(vector);

}

int main() {
    const char *rel_path = "./tuplas";
    abs_path = realpath(rel_path, NULL);
    init();
    
    // Llamada a los test
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();
    test_7();
    test_8();
    printf("\n");
    return 0;
}
