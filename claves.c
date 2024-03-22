#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include "mensajes.h"
#include "claves.h"
#include "comm.h"


int init() {
	//Función init que manda el mensaje de init al servidor
	int sd, res;
	char *ip_tuplas = getenv("IP_TUPLAS");
	char *port_tuplas = getenv("PORT_TUPLAS");
	int port = itoa(port_tuplas);
	if (port == 0) {
		return -1;
	}    
	
	sd = create_client_socket(ip_tuplas, port);
	if (sd < 0) {
		printf("Error en la creación del socket del cliente\n");
		return -1;
	}

	char op = 0;
	char r[4];
	
	res = send_message(sd, (char *)&op, sizeof(char));
	if (res == -1) {
		printf("Error al enviar la operación\n");
		return -1;
	}
	
	res = receive_message(sd, (char*)&r, sizeof(r));
	if (res == -1) {
		printf("Error al recibir la respuesta\n");
		return -1;
	}
	
	res = atoi(r);
	if (res == 0) {return -1;}
	
	close(sd);
	return res;
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    //Función set_value que manda el mensaje de set_value al servidor
    return 2;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2){
    //Función set_value que manda el mensaje de get_value al servidor
    
    // Copia lo recibido
    /*strcpy(value1, r.value1);
    *N_value2 = r.N_or_exists;
    memcpy(V_value2, r.V_value2, r.N_or_exists * sizeof(double));
	*/
	
    return 3;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2){
    //Función modify_value que manda el mensaje de modify_value al servidor
    
    
    /*if (N_value2 <= 32 && N_value2 >= 1) {
    	// envío de mensaje
    }
    else {
        //r.res = -1;
    }*/

    return 4;
}

int delete_key(int key){
    //Función delete_key que manda el mensaje de delete_key al servidor
    return 5;
}
int exist(int key){
    //Función exist que manda el mensaje de exist al servidor
    return 6;
}
