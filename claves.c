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
		printf("Error: getenv IP\n");
		return -1;
	}
	
	char *port_tuplas = getenv("PORT_TUPLAS");
	if (port_tuplas == NULL) {
		printf("Error: getenv PORT\n");
		return -1;
	}
	
	int port = atoi(port_tuplas);
	if (port == 0) {
		return -1;
	}    

	sd = create_client_socket(ip_tuplas, port);
	if (sd < 0) {
		printf("Error en la creación del socket del cliente\n");
		return -1;
	}

	char *op = "1";
	char r[4];
	
	res = write_line(sd, op);
	if (res == -1) {
		printf("Error al enviar la operación\n");
		close(sd);
		return -1;
	}
	
	res = read_line(sd, r, 4);
	if (res == -1) {
		printf("Error al recibir la respuesta\n");
		close(sd);
		return -1;
	}
	
	if (strcmp(r, "0") != 0) {return -1;}
	
	close(sd);
	return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    //Función set_value que manda el mensaje de set_value al servidor
    if (N_value2 <= 32 && N_value2 >= 1) {
        int sd, res;
        char *ip_tuplas = getenv("IP_TUPLAS");
        if (ip_tuplas == NULL) {
            printf("Error: getenv IP\n");
            return -1;
        }

        char *port_tuplas = getenv("PORT_TUPLAS");
        if (port_tuplas == NULL) {
            printf("Error: getenv PORT\n");
            return -1;
        }

        int port = atoi(port_tuplas);
        if (port == 0) {
            return -1;
        }

        sd = create_client_socket(ip_tuplas, port);
        
        if (sd < 0) {
            printf("Error en la creación del socket del cliente\n");
            return -1;
        }

        char *op = "2";
        char r[4];
        char buffer[1024];

        res = write_line(sd, op);
        if (res == -1) {
            printf("Error al enviar la operación\n");
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

        res = write_line(sd, value1);
        if (res == -1) {
            printf("Error al enviar la key\n");
            close(sd);
            return -1;
        }
		
		// Mensaje de continuación
		res = read_line(sd, r, 4);
		if (res == -1) {
		    printf("Error al recibir la respuesta\n");
		    close(sd);
		    return -1;
		}
		if (strcmp(r, "0")== 0){
		
			sprintf(buffer, "%i", N_value2);
		    res = write_line(sd, buffer);
		    if (res == -1) {
		        printf("Error al enviar N_value2\n");
		        close(sd);
		        return -1;
		    }

		    for (int i = 0; i < N_value2; i++) {
		        sprintf(buffer, "%lf", V_value2[i]);
		        res = write_line(sd, buffer);
		        if (res == -1) {
		            printf("Error al enviar V_value2[%i]\n", i);
		            close(sd);
		            return -1;
		        }
		    }

		    res = read_line(sd, r, 4);
		    if (res == -1) {
		        printf("Error al recibir la respuesta\n");
		        close(sd);
		        return -1;
		    }

		    if (strcmp(r, "0") != 0) { return -1; }

		    close(sd);
		    return 0;
		}
	}
	return -1;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2){
    //Función set_value que manda el mensaje de get_value al servidor

    int sd, res;
    char *ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL) {
        printf("Error: getenv IP\n");
        return -1;
    }

    char *port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL) {
        printf("Error: getenv PORT\n");
        return -1;
    }

    int port = atoi(port_tuplas);
    if (port == 0) {
        return -1;
    }

    sd = create_client_socket(ip_tuplas, port);
    if (sd < 0) {
        printf("Error en la creación del socket del cliente\n");
        return -1;
    }

    char *op = "3";
    char r[4];
    char buffer[1024];

    res = write_line(sd, op);
    if (res == -1) {
        printf("Error al enviar la operación\n");
        close(sd);
        return -1;
    }

    sprintf(buffer, "%d", key);
    res = write_line(sd, buffer);
    if (res == -1) {
        printf("Error al enviar la key\n");
        close(sd);
        return -1;
    }
    
    // Mensaje de continuación
    res = read_line(sd, r, 4);
    if (res == -1) {
        printf("Error al recibir la respuesta\n");
        close(sd);
        return -1;
    }
    if (strcmp(r, "0")== 0){
        //Recibe value 1
        res = read_line(sd, value1, 1024);
        if (res == -1) {
            printf("Error al recibir value1\n");
            close(sd);
            return -1;
        }

        //Recibe N_value2
        res = read_line(sd, buffer, 1024);
        if (res == -1) {
            printf("Error al recibir N_value2\n");
            close(sd);
            return -1;
        }
        *N_value2 = atoi(buffer);
        if(*N_value2 == 0){
            printf("Error al hacer atoi\n");
            close(sd);
            return -1;
        }
        else if (*N_value2 == -1){
            printf("N fuera de rango\n");
            close(sd);
            return -1;
        }
        
        // Recibe V_value2
        for(int i = 0; i < *N_value2; i++){
            res = read_line(sd, buffer, 1024);
            if (res == -1) {
                printf("Error al recibir V_value2\n");
                close(sd);
                return -1;
            }
            V_value2[i] = strtod(buffer, NULL);
        }
    } else{
        return -1;
    }

    return 0;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2){
    //Función modify_value que manda el mensaje de modify_value al servidor
    if (N_value2 <= 32 && N_value2 >= 1) {
        int sd, res;
        char *ip_tuplas = getenv("IP_TUPLAS");
        if (ip_tuplas == NULL) {
            printf("Error: getenv IP\n");
            return -1;
        }

        char *port_tuplas = getenv("PORT_TUPLAS");
        if (port_tuplas == NULL) {
            printf("Error: getenv PORT\n");
            return -1;
        }

        int port = atoi(port_tuplas);
        if (port == 0) {
            return -1;
        }

        sd = create_client_socket(ip_tuplas, port);
        if (sd < 0) {
            printf("Error en la creación del socket del cliente\n");
            return -1;
        }

        char *op = "4";
        char r[4];
        char buffer[1024];


        res = write_line(sd, op);
        if (res == -1) {
            printf("Error al enviar la operación\n");
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

        res = write_line(sd, value1);
        if (res == -1) {
            printf("Error al enviar la key\n");
            close(sd);
            return -1;
        }

		// Mensaje de continuación
		res = read_line(sd, r, 4);
		if (res == -1) {
		    printf("Error al recibir la respuesta\n");
		    close(sd);
		    return -1;
		}
		if (strcmp(r, "0")== 0){
		
		    sprintf(buffer, "%i", N_value2);
		    res = write_line(sd, buffer);
		    if (res == -1) {
		        printf("Error al enviar N_value2\n");
		        close(sd);
		        return -1;
		    }

		    for (int i = 0; i < N_value2; i++) {
		        sprintf(buffer, "%lf", V_value2[i]);
		        res = write_line(sd, buffer);
		        if (res == -1) {
		            printf("Error al enviar V_value2[%i]\n", i);
		            close(sd);
		            return -1;
		        }
		    }

		    res = read_line(sd, r, 4);
		    if (res == -1) {
		        printf("Error al recibir la respuesta\n");
		        close(sd);
		        return -1;
		    }

		    if (strcmp(r, "0") != 0) { return -1; }

		    close(sd);
		    return 0;
    	}
    }
    return -1;
}

int delete_key(int key){
    //Función delete_key que manda el mensaje de delete_key al servidor
    int sd, res;
    char *ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL) {
        printf("Error: getenv IP\n");
        return -1;
    }

    char *port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL) {
        printf("Error: getenv PORT\n");
        return -1;
    }

    int port = atoi(port_tuplas);
    if (port == 0) {
        return -1;
    }

    sd = create_client_socket(ip_tuplas, port);
    if (sd < 0) {
        printf("Error en la creación del socket del cliente\n");
        return -1;
    }

    char *op = "5";
    char r[4];
    char buffer[1024];

    res = write_line(sd, op);
    if (res == -1) {
        printf("Error al enviar la operación\n");
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

    res = read_line(sd, r, 4);
    if (res == -1) {
        printf("Error al recibir la respuesta\n");
        close(sd);
        return -1;
    }

    if (strcmp(r, "0") != 0) {return -1;}

    close(sd);
    return 0;
}
int exist(int key){
    //Función exist que manda el mensaje de exist al servidor
    int sd, res;
    char *ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL) {
        printf("Error: getenv IP\n");
        return -1;
    }

    char *port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL) {
        printf("Error: getenv PORT\n");
        return -1;
    }

    int port = atoi(port_tuplas);
    if (port == 0) {
        return -1;
    }

    sd = create_client_socket(ip_tuplas, port);
    if (sd < 0) {
        printf("Error en la creación del socket del cliente\n");
        return -1;
    }

    char *op = "6";
    char r[4];
    char buffer[1024];

    res = write_line(sd, op);
    if (res == -1) {
        printf("Error al enviar la operación\n");
        close(sd);
        return -1;
    }

    sprintf(buffer, "%d", key);
    res = write_line(sd, buffer);
    if (res == -1) {
        printf("Error al enviar la key\n");
        close(sd);
        return -1;
    }

    res = read_line(sd, r, 4);
    if (res == -1) {
        printf("Error al recibir la respuesta\n");
        close(sd);
        return -1;
    }

    close(sd);
    return atoi(r);
}
