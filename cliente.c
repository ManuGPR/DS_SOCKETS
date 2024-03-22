#include <stdio.h>
#include "claves.h"

int main() {

    // Llamada a la función init
    int res = init();
    printf("Init devuelve %d\n", res);

    // Llamada a la función set_value
    int N_value2 = 32;
    double V_value2[N_value2];
    for (int i = 0; i < N_value2; i++) {V_value2[i] = (double)i;}
    res = set_value(10, "prueba de cadena", N_value2, V_value2);
    printf("Set_value devuelve %d\n", res);

    // Llamada a la función get_value
    int N_value2_res;
    double V_value2_res[32];
    char Value1_res[256];

    res = get_value(10, Value1_res, &N_value2_res , V_value2_res);
    printf("Get_value devuelve %d\n", res);
    printf("Value1 = %s\n", Value1_res);
    printf("N_value2 = %i\n", N_value2_res);
    printf("V_value2:\n");
    for(int i = 0; i < N_value2_res; i++){
        printf("%lf",V_value2_res[i]);
        if (i < N_value2_res -1) printf(", ");
        else printf("\n");
    }

    // Llamada a la función modify value
    int new_n = 16;
    double vector_modify[new_n];
    for (int i = 0; i < new_n; i++) {vector_modify[i] = i+i;}
    res = modify_value(10, "prueba de nueva cadena", new_n, vector_modify);
    printf("Modify value devuelve %d\n", res);

    // Llamada a la función exist 1
    int key = 10;
    res = exist(key);
    printf("Exist 1 devuelve %d\n", res);

    // Llamada a la función delete_key
    res = delete_key(key);
    printf("Delete_key devuelve %d\n", res);

    // Llamada a la función exist 2
    res = exist(key);
    printf("Exist 2 devuelve %d\n", res);
    return 0;

	return 0;
}
