#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "../claves.h"

int main() {

    printf("Cliente %d ejecutando\n", getpid());
    // Llamada a la función set_value
    int N_value2 = 32;
    double V_value2[N_value2];
    for (int i = 0; i < N_value2; i++) { V_value2[i] = (double) i; }
    set_value(10, "prueba de cadena", N_value2, V_value2);

    sleep(2);

    N_value2 = 32;
    for (int i = 0; i < N_value2; i++) { V_value2[i] = (double) i; }
    set_value(10, "prueba de cadena", N_value2, V_value2);

    printf("Cliente %d acaba\n", getpid());
}