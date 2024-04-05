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

    // Lee y muestra cada caracter del archivo
    char linea[500];
    while(fgets(linea, sizeof(linea), archivo) !=NULL){
        printf("%s\n", linea);
    }

    // Cierra el archivo
    fclose(archivo);
    printf("\n");
    return 0;
}

int escribir_fichero(char *abs_path, int k, int N, char *value1, double *value2){
    
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
    archivo = fopen(nombre_archivo, "w");
    if(archivo == NULL){
        printf("Error al abrir el fichero\n");
        return -1;
    }

    if (fprintf(archivo, "%d\n", k) < 0) {return -1;}
    if (fprintf(archivo, "%s\n", value1) < 0) {return -1;}
    if (fprintf(archivo, "%d\n", N) < 0) {return -1;}
    for (int i = 0; i < N; i++) {
        if (fprintf(archivo, "%lf", value2[i]) < 0) {return  -1;}
        if (i < N -1) {fprintf(archivo, ", ");}
    }

    // Cierra el archivo
    fclose(archivo);
    printf("\n");
    return 0;

}

void test_1() {
	/*Test 1: funcionamiento corecto*/
    printf("Test 1: todo correcto\n");

    //Se borran todos lor archivos y se crea un archivo
    init();
    
    int n = 2;
    int k = 1;
    double vector[64];
    int N;
    double v2[32];
    char v1[256];
    
    for (int i = 0; i < n; i++) { vector[i] = (double) i;}

    set_value(k, "archivo", n, vector);

    print_files(abs_path);
    printf("Los datos que hay en el archivo creado: \n");
    leer_fichero(abs_path, k);

    //Se obtine el resultado
    int get = get_value(1, v1, &N , v2);
    printf("Resultado prueba 1: %d\n", get);

    printf("Los datos que se han obtenido son:\n");
    printf("K: %i\n", k);
    printf("V1: %s\n", v1);
    printf("N: %i\n", N);
    printf("V2: ");
    for(int i = 0; i < N; i++){
        printf("%lf\n", v2[i]);
    }

}

void test_2() {
	/*Test 2: no existe ninguna clave*/
    printf("\nTest 2: no existe ninguna clave\n");

    //Se borran los archivos
    init();
    
    int k = 1;
    int N;
    double v2[32];
    char v1[256];
    
    int get = get_value(k, v1, &N , v2);
    printf("Resultado prueba 2: %d\n", get);
}

void test_3() {
	/*Test 3: N = 33*/
    printf("\nTest 3: N > 32 \n");
    
    int k = 3;
    int n = 33;
    double vector[64];
    int N;
    double v2[32];
    char v1[256];
    
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    //Crear un fichero a mano de N = 33
    escribir_fichero(abs_path, k, n, "prueba 3", vector);
    leer_fichero(abs_path,k);
    int get = get_value(k, v1, &N , v2);
    printf("Resultado prueba 3: %d\n", get);
}

void test_4() {
	/*Test 4: len(value1) > 255*/
    printf("\nTest 4: len(value1) > 255\n");
    init();

    char cadena[300];
    int n = 1;
    int k = 2;
    double vector[64];
    int N;
    double v2[32];
    char v1[256];
    
    for (int i = 0; i < 50; i++){ strcat(cadena, "prueba");}
    for (int i = 0; i < n; i++) { vector[i] = (double) i; }
    escribir_fichero(abs_path, k, n, cadena, vector);
    leer_fichero(abs_path,k);
    int get = get_value(k, v1, &N , v2);
    printf("Resultado prueba 4: %d\n", get);
    printf("Los datos que se han obtenido son:\n");
    printf("K: %i\n", k);
    printf("V1: %s\n", v1);
    printf("N: %i\n", N);
    printf("V2: ");
    for(int i = 0; i < N; i++){
        printf("%lf\n", v2[i]);
    }
}

int main(){
    //Se obtiene la path del diretorio tuplas donde estan almacanadas las key y se abre el directorio
    const char *rel_path = "./tuplas";
    abs_path = realpath(rel_path, NULL);

	// Llamada a las funciones
	test_1();
	test_2();    
    test_3();
    test_4();
    return 0;	
}
