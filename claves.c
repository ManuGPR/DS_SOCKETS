#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "claves.h"
#include "comm.h"


int init() {
	//Función init que manda el mensaje de init al servidor
	int sd, res;

    //Se recive las varibles de entorno IP_TUPLAS y PORT_TUPLAS
	char *ip_tuplas = getenv("IP_TUPLAS");
	if (ip_tuplas == NULL) {
		printf("Error: getenv IP de init\n");
		return -1;
	}
	
	char *port_tuplas = getenv("PORT_TUPLAS");
	if (port_tuplas == NULL) {
		printf("Error: getenv PORT de init\n");
		return -1;
	}
	
	int port = atoi(port_tuplas);
	if (port == 0) {
		return -1;
	}    

	sd = create_client_socket(ip_tuplas, port);
	if (sd < 0) {
		printf("Error: creación del socket del cliente de init\n");
		return -1;
	}

	char *op = "1";
	char r[4];
	
	res = write_line(sd, op);
	if (res == -1) {
		printf("Error: envio de la operación de init\n");
		close(sd);
		return -1;
	}
	
	res = read_line(sd, r, 4);
	if (res == -1) {
		printf("Error: recepción de la respuesta de init\n");
		close(sd);
		return -1;
	}
	
	close(sd);
	return atoi(r);
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    //Función set_value que manda el mensaje de set_value al servidor
    if (N_value2 <= 32 && N_value2 >= 1 && strlen(value1) < 256) {
        int sd, res;
        char *ip_tuplas = getenv("IP_TUPLAS");
        if (ip_tuplas == NULL) {
            printf("Error: getenv IP de set_value\n");
            return -1;
        }

        char *port_tuplas = getenv("PORT_TUPLAS");
        if (port_tuplas == NULL) {
            printf("Error: getenv PORT de set_value\n");
            return -1;
        }

        int port = atoi(port_tuplas);
        if (port == 0) {
            return -1;
        }

        sd = create_client_socket(ip_tuplas, port);
        
        if (sd < 0) {
            printf("Error: creación del socket del cliente de set_value\n");
            return -1;
        }

        char *op = "2";
        char r[4];
        char buffer[1024];

        res = write_line(sd, op);
        if (res == -1) {
            printf("Error: envío la operación de set_value\n");
            close(sd);
            return -1;
        }


        sprintf(buffer, "%i", key);
        res = write_line(sd, buffer);
        if (res == -1) {
            printf("Error: envio de la key de set_value\n");
            close(sd);
            return -1;
        }

		
		// Mensaje de continuación
		res = read_line(sd, r, 4);
		if (res == -1) {
		    printf("Error: recepción del mensaje de continuación de set_value\n");
		    close(sd);
		    return -1;
		}
		
		// Si se ha encontrado la key
		if (strcmp(r, "0")== 0){
			
			// Se envía value1
		    res = write_line(sd, value1);
		    if (res == -1) {
		        printf("Error: envío de value1 de set_value\n");
		        close(sd);
		        return -1;
		    }		
		    
		    // Se envía N_value2
			sprintf(buffer, "%i", N_value2);
		    res = write_line(sd, buffer);
		    if (res == -1) {
		        printf("Error: envío de N_value2 de set_value\n");
		        close(sd);
		        return -1;
		    }
			
			// Se envía V_value2
		    for (int i = 0; i < N_value2; i++) {
		        sprintf(buffer, "%lf", V_value2[i]);
		        res = write_line(sd, buffer);
		        if (res == -1) {
		            printf("Error: envío de V_value2[%i] de set_value\n", i);
		            close(sd);
		            return -1;
		        }
		    }
			
			// Se recibe la respuesta
		    res = read_line(sd, r, 4);
		    if (res == -1) {
		        printf("Error: recepción de la respuesta de set_value\n");
		        close(sd);
		        return -1;
		    }

		    close(sd);
		    return atoi(r);
		}
	}
	return -1;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2){
    //Función set_value que manda el mensaje de get_value al servidor
    int sd, res;
    char *ip_tuplas = getenv("IP_TUPLAS");
    
    if (ip_tuplas == NULL) {
        printf("Error: getenv IP de get_value\n");
        return -1;
    }

    char *port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL) {
        printf("Error: getenv PORT de get_value\n");
        return -1;
    }

    int port = atoi(port_tuplas);
    if (port == 0) {
        return -1;
    }

    sd = create_client_socket(ip_tuplas, port);
    if (sd < 0) {
        printf("Error: creación del socket del cliente de get_value\n");
        return -1;
    }

    char *op = "3";
    char r[4];
    char buffer[1024];

    res = write_line(sd, op);
    if (res == -1) {
        printf("Error: envío de la operación de get_value\n");
        close(sd);
        return -1;
    }

    sprintf(buffer, "%d", key);
    res = write_line(sd, buffer);
    if (res == -1) {
        printf("Error: envío de la key de get_value\n");
        close(sd);
        return -1;
    }
    
    // Mensaje de continuación
    res = read_line(sd, r, 4);
    if (res == -1) {
        printf("Error: recepción del mensaje de continuación de get_value\n");
        close(sd);
        return -1;
    }
    
    // Si se encuentra la key
    if (strcmp(r, "0")== 0){
    
        //Recibe value 1
        res = read_line(sd, value1, 1024);
        if (res == -1) {
            printf("Error: recepción de value1 de get_value\n");
            close(sd);
            return -1;
        }
        if (strcmp(value1, "-1") == 0) {
        	close(sd);
        	return -1;
        }

        //Recibe N_value2
        res = read_line(sd, buffer, 1024);
        if (res == -1) {
            printf("Error: recepción de N_value2 de get_value\n");
            close(sd);
            return -1;
        }
        
        *N_value2 = atoi(buffer);
        if (*N_value2 == 0){
            printf("Error: atoi de get_value\n");
            close(sd);
            return -1;
        }
        else if (*N_value2 == -1){
            printf("Error: N_value2 fuera de rango de get_value\n");
            close(sd);
            return -1;
        }
        else {
        
		    // Recibe V_value2
		    for(int i = 0; i < *N_value2; i++){
		        res = read_line(sd, buffer, 1024);
		        if (res == -1) {
		            printf("Error: recepción de V_value2[%d]\n de get_value", i);
		            close(sd);
		            return -1;
		        }
		        V_value2[i] = strtod(buffer, NULL);
		    }
		    
		    // Recibe el mensaje de respuesta
		    res = read_line(sd, r, 4);
		    if (res == -1) {
		        printf("Error: recepción de la respuesta de get_value\n");
		        close(sd);
		        return -1;
		    }

		    close(sd);
		    return atoi(r);
		}
    }
    close(sd);
    return -1;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2){
    //Función modify_value que manda el mensaje de modify_value al servidor
    if (N_value2 <= 32 && N_value2 >= 1 && strlen(value1) < 256) {
        int sd, res;
        char *ip_tuplas = getenv("IP_TUPLAS");
        if (ip_tuplas == NULL) {
            printf("Error: getenv IP de modify_value\n");
            return -1;
        }

        char *port_tuplas = getenv("PORT_TUPLAS");
        if (port_tuplas == NULL) {
            printf("Error: getenv PORT de modify_value\n");
            return -1;
        }

        int port = atoi(port_tuplas);
        if (port == 0) {
            return -1;
        }

        sd = create_client_socket(ip_tuplas, port);
        if (sd < 0) {
            printf("Error: creación del socket del cliente de modify_value\n");
            return -1;
        }

        char *op = "4";
        char r[4];
        char buffer[1024];


        res = write_line(sd, op);
        if (res == -1) {
            printf("Error: envío de la operación de modify_value\n");
            close(sd);
            return -1;
        }

        sprintf(buffer, "%i", key);
        res = write_line(sd, buffer);
        if (res == -1) {
            printf("Error al enviar la key\n");
            close(sd);
            return -1;
        }

		// Mensaje de continuación
		res = read_line(sd, r, 4);
		if (res == -1) {
		    printf("Error: recepción del mensaje de continuación de modify_value\n");
		    close(sd);
		    return -1;
		}
		
		// Si se encuentra la key 
		if (strcmp(r, "0")== 0){
			
			// Se envía value1
		    res = write_line(sd, value1);
		    if (res == -1) {
		        printf("Error: envío value1 de modify_value\n");
		        close(sd);
		        return -1;
		    }
			
			// Se envía N_value2
		    sprintf(buffer, "%i", N_value2);
		    res = write_line(sd, buffer);
		    if (res == -1) {
		        printf("Error: envío de N_value2 de modify_value\n");
		        close(sd);
		        return -1;
		    }
			
			// Se envía V_value2
		    for (int i = 0; i < N_value2; i++) {
		        sprintf(buffer, "%lf", V_value2[i]);
		        res = write_line(sd, buffer);
		        if (res == -1) {
		            printf("Error: envío de V_value2[%i] de modify_value\n", i);
		            close(sd);
		            return -1;
		        }
		    }

		    res = read_line(sd, r, 4);
		    if (res == -1) {
		        printf("Error: recepción de la respuesta de modify_value\n");
		        close(sd);
		        return -1;
		    }

		    close(sd);
		    return atoi(r);
    	}
    }
    return -1;
}

int delete_key(int key){
    //Función delete_key que manda el mensaje de delete_key al servidor
    int sd, res;
    char *ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL) {
        printf("Error: getenv IP de delete_key\n");
        return -1;
    }

    char *port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL) {
        printf("Error: getenv PORT de delete_key\n");
        return -1;
    }

    int port = atoi(port_tuplas);
    if (port == 0) {
        return -1;
    }

    sd = create_client_socket(ip_tuplas, port);
    if (sd < 0) {
        printf("Error: creación del socket del cliente de delete_key\n");
        return -1;
    }

    char *op = "5";
    char r[4];
    char buffer[1024];

    res = write_line(sd, op);
    if (res == -1) {
        printf("Error: envío de la operación de delete_key\n");
        close(sd);
        return -1;
    }

    sprintf(buffer, "%i", key);
    res = write_line(sd, buffer);
    if (res == -1) {
        printf("Error: envío de la key de delete_key\n");
        close(sd);
        return -1;
    }

    res = read_line(sd, r, 4);
    if (res == -1) {
        printf("Error: recepción de la respuesta de delete_key\n");
        close(sd);
        return -1;
    }

    close(sd);
    return atoi(r);
}

int exist(int key){
    //Función exist que manda el mensaje de exist al servidor
    int sd, res;
    char *ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL) {
        printf("Error: getenv IP de exist\n");
        return -1;
    }

    char *port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL) {
        printf("Error: getenv PORT de exist\n");
        return -1;
    }

    int port = atoi(port_tuplas);
    if (port == 0) {
        return -1;
    }

    sd = create_client_socket(ip_tuplas, port);
    if (sd < 0) {
        printf("Error: creación del socket del cliente de exist\n");
        return -1;
    }

    char *op = "6";
    char r[4];
    char buffer[1024];

    res = write_line(sd, op);
    if (res == -1) {
        printf("Error: envío de la operación de exist\n");
        close(sd);
        return -1;
    }

    sprintf(buffer, "%d", key);
    res = write_line(sd, buffer);
    if (res == -1) {
        printf("Error: envío la key de exist\n");
        close(sd);
        return -1;
    }

    res = read_line(sd, r, 4);
    if (res == -1) {
        printf("Error: recepción de la respuesta de exist\n");
        close(sd);
        return -1;
    }

    close(sd);
    return atoi(r);
}
