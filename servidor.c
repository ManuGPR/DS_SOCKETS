#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include "comm.h"

#define PATH_MAX 4096
#define cero 0

const char *rel_path="./tuplas";
char *abs_path;

pthread_mutex_t mutex;
pthread_cond_t copiado;
int copia = 0;

void get_tuple_abs_path(char * tuple_name, int key) {
	//Función auxiliar que obtiene el path absoluto de una tupla
	strcpy(tuple_name, abs_path);
    strcat(tuple_name, "/");
    char key_str[32];
    sprintf(key_str, "%d", key);
    strcat(tuple_name, key_str);
}

int init_server(int * sd) {
	// Declaración de variables necesarias para el init
	DIR *dir = opendir(abs_path);
	struct dirent* tuplas;
	char* file_name;
	char res[4] = "0";
	
	// Recibir la key
	/*char buffer[128]
	if (receive_message(*sd, buffer, strlen(buffer)) == -1) {
		printf("Error: init receive\n");
		return -1;
	}
	
	int key = atoi(buffer);*/
	
	// Mientras haya tuplas en el fichero de tuplas
	while ((tuplas = readdir(dir)) != NULL) {
		
		// Si el objeto no es un directorio
		if (strcmp(tuplas->d_name, ".") != 0 && strcmp(tuplas->d_name, "..") != 0) {
			
			// Se reserva espacio para el nombre del fichero y se obtiene su path absoluto
			file_name = calloc(PATH_MAX, sizeof(char));
			strcpy(file_name, abs_path);
			strcat(file_name, "/");
			strcat(file_name, tuplas->d_name);

			// Se borra el fichero, si hay algún error, se escribe y la respuesta devolverá -1
			if (remove(file_name) == -1) {	
				perror("");
				strcpy(res, "-1");
			}
			
			// Se libera el espacio dinámico
			free(file_name);
		}
	}
	
	int r = write_line(*sd, res);
	if (r == -1) {
		printf("Error al enviar la operación\n");
		return -1;
	}
	close(*sd);
	pthread_exit(NULL);
}

int set_value_server(int * sd) {
	int res;
	int key = 0;
	// Se obtiene el nombre absoluto del fichero
	char *tuple_name = calloc(PATH_MAX, sizeof(char));
	get_tuple_abs_path(tuple_name, key);
	
	// Se mira si existe
	if (access(tuple_name, F_OK) == 0) {	
		printf("Archivo existe\n");
		pthread_exit(NULL);
	}
	
	// Crea el fichero
	FILE * tuple;
	tuple = fopen(tuple_name, "w+");
	if (tuple == NULL) {
		perror("");
		pthread_exit(NULL);
	}
	
	// Escribe los datos
	char value1[256] = {"hola"};
    int N = 1;
    double value2[1] = {1};
	if (fprintf(tuple, "%d\n", key) < 0) {res = -1;}
	if (fprintf(tuple, "%s\n", value1) < 0) {res = -1;}
	if (fprintf(tuple, "%d\n", N) < 0) {res = -1;}
	for (int i = 0; i < N; i++) {
		if (fprintf(tuple, "%lf", value2[i]) < 0) {res = -1;}
		if (i < N -1) {fprintf(tuple, ", ");}
    }
     
    // Cierra la tupla
    fclose(tuple);
    
    pthread_exit(NULL);
}

int get_value_server(int * sd) {
	int res;
	int key = 0;
	// Se consigue el path de la tupla
    char *tuple_name = calloc(PATH_MAX, sizeof(char));
    get_tuple_abs_path(tuple_name, key);
    
    // Se mira si existe
    if (access(tuple_name, F_OK) == -1) {
        printf("Archivo no existe\n");
        pthread_exit(NULL);
    }

    // Abre el archivo
    FILE * tuple;
    tuple = fopen(tuple_name, "r");
    if (tuple == NULL) {
        perror("");
        pthread_exit(NULL);
    }

    // Lee los datos
    char value1[256];
    int N;
    double value2[32];
    
    if (fscanf(tuple, "%d\n", &key) < 1) {res = -1;}
    if (fscanf(tuple, "%[^\n]s\n", value1) < 1) {res = -1;}
    if (fscanf(tuple, "%d\n", &N) < 1) {res = -1;}
    for (int i = 0; i < N; i++) {
        if (fscanf(tuple, "%lf", &value2[i]) < 1) {res = -1;}
        if (i < N -1) { fscanf(tuple, ", ");}
    }
	if (N < 1 || N > 32){
		res = -1;
	}
  	
    // Cierra la tupla
    fclose(tuple);

    pthread_exit(NULL);
}

int modify_value_server(int * sd) {
	int res;
	int key = 0;
	
	// Se obtiene el nombre absoluto del fichero
    char *tuple_name = calloc(PATH_MAX, sizeof(char));
    get_tuple_abs_path(tuple_name, key);

    // Se mira si existe
    if (access(tuple_name, F_OK) == -1) {
        printf("Archivo no existe\n");
        res = -1;
        pthread_exit(NULL);
    }

    // Crea el fichero
    FILE * tuple;
    tuple = fopen(tuple_name, "w+");
    if (tuple == NULL) {
        perror("");
        res = -1;
        pthread_exit(NULL);
    }

    // Escribe los datos
    char value1[256] = {"hola"};
    int N = 1;
    double value2[1] = {1};
    
    if (fprintf(tuple, "%d\n", key) < 0) {res = -1;}
    if (fprintf(tuple, "%s\n", value1) < 0) {res = -1;}
    if (fprintf(tuple, "%d\n", N) < 0) {res = -1;}
    for (int i = 0; i < N; i++) {
        if (fprintf(tuple, "%lf", value2[i]) < 0) {res = -1;}
        if (i < N -1) {fprintf(tuple, ", ");}
    }

    // Cierra la tupla
    fclose(tuple);

    pthread_exit(NULL);
}

int delete_key_server(int * sd) {
	int res;
	int key = 0;
	
	// Declaración de variables necesarias para el delete key
    DIR *dir = opendir(abs_path);
    struct dirent* tuplas;
    char* file_name;
    char key_str[32];
    sprintf(key_str, "%d", key);
    char *tuple_name = (char *)calloc(PATH_MAX, sizeof(char));
	get_tuple_abs_path(tuple_name, key);
		
	// Si el fichero existe se busca
	if (access(tuple_name, F_OK) == 0) {
		// Mientras haya tuplas en el fichero de tuplas
		while ((tuplas = readdir(dir)) != NULL) {
			// Si el objeto no es un directorio
			if (strcmp(tuplas->d_name, key_str) == 0) {
			    // Se reserva espacio para el nombre del fichero y se obtiene su path absoluto
			    file_name = calloc(PATH_MAX, sizeof(char));
			    strcpy(file_name, abs_path);
			    strcat(file_name, "/");
			    strcat(file_name, tuplas->d_name);

			    // Se borra el fichero, si hay algún error, se escribe y la respuesta devolverá -1
			    if (remove(file_name) == -1) {
			        perror("");
			        res = -1;
			    }

			    // Se libera el espacio dinámico
			    free(file_name);
			}
		}
   	}
   	else {
   		printf("El fichero no existe\n");
   		res = -1;
   	}

    pthread_exit(NULL);
}

int exist_server(int * sd) {
	int res;
	int key = 0;
	
	// Datos para el fichero
    char *tuple_name = calloc(PATH_MAX, sizeof(char));
    get_tuple_abs_path(tuple_name, key);

    // Se mira si existe
    res = access(tuple_name, F_OK);
    if (access(tuple_name, F_OK ) == 0) {
        res = 1;
    } else {
        if (errno == ENOENT) {
            res = 0;
        } else {
            res = -1;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
	// Se comprueba el número de args
	if (argc != 2) {
		printf("Error: Puerto sin especificar\n");
		return -1;
	}
	
	// Se checkea el puerto
	int puerto = atoi(argv[1]);
	if (puerto <= 0) {
		printf("Error: mal puerto\n");
		return -1;
	}
	
	/*// Se guarda el puerto en una env
	if (setenv("PORT_TUPLAS", argv[1], 1) == -1) {
		printf("Error: setenv puerto\n");
		return -1;
	}
	
	// Se guarda la IP en una env
	if (setenv("IP_TUPLAS", "a", 1) == -1) {
		printf("Error: setenv puerto\n");
		return -1;
	}*/
	

	// Se crean los attr de los threads
	pthread_attr_t attr_thr;
	pthread_attr_init(&attr_thr);
	pthread_attr_setdetachstate(&attr_thr, PTHREAD_CREATE_DETACHED);
	
	// Se le da valor al path de las tuplas
	abs_path = realpath(rel_path, NULL);

	// Bucle de espera a las peticiones
	char op;
	
	// Creación del socket del servidor
	int socket_server = create_server_socket(puerto, SOCK_STREAM);
	char buffer[4];
	int new_sd;
	while(1) {
		new_sd = accept_server(socket_server);
		if (read_line(new_sd, buffer, 4) == -1) {
			printf("Error: receive message\n");
			return -1;
		}
		
		op = buffer[0];
		
		pthread_t thread;
		// Llamada a las funciones
		switch(op) {
			case '1': pthread_create(&thread, &attr_thr, (void*)init_server, (void*)&new_sd);
				break;
			case '2': pthread_create(&thread, &attr_thr, (void*)set_value_server, (void*)&new_sd);
				break;
			case '3': pthread_create(&thread, &attr_thr, (void*)get_value_server, (void*)&new_sd);
				break;
			case '4': pthread_create(&thread, &attr_thr, (void*)modify_value_server, (void*)&new_sd);
				break;
			case '5': pthread_create(&thread, &attr_thr, (void*)delete_key_server, (void*)&new_sd);
				break;
			case '6': pthread_create(&thread, &attr_thr, (void*)exist_server, (void*)&new_sd);
				break;
		}
	}
	return 0;
}
