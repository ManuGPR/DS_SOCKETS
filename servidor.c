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

// Paths
const char *rel_path="./tuplas";
char *abs_path;

// Mutex para la copia del socket
pthread_mutex_t mutex;
pthread_cond_t copiado;
int copia = 0;

int sd_copy(int sd){
	// Función auxiliar que copia el socket y lo devuelve  
    pthread_mutex_lock(&mutex);
    copia = 1;
    pthread_cond_signal(&copiado);
    pthread_mutex_unlock(&mutex);
    return sd;
}

void get_tuple_abs_path(char * tuple_name, int key) {
	//Función auxiliar que obtiene el path absoluto de una tupla
	strcpy(tuple_name, abs_path);
    strcat(tuple_name, "/");
    char key_str[32];
    sprintf(key_str, "%d", key);
    strcat(tuple_name, key_str);
}

int receive_key(int sd) {
	// Función auxiliar que recibe la key
    char buffer[1024];
    if (read_line(sd, buffer, 1024) == -1) {
        printf("Error: read_line incorrecto\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }
    return atoi(buffer);
}

int init_server(int * nsd) {
	// Variables para la conexión	
	int sd = sd_copy(*nsd);
    
	// Declaración de variables necesarias para el init
	DIR *dir = opendir(abs_path);
	struct dirent* tuplas;
	char* file_name;
	char res[4] = "0";
	
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
	
	int r = write_line(sd, res);
	if (r == -1) {
		printf("Error: envio del resultado de init\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
	}

	close(sd);
	pthread_exit(NULL);
}

int set_value_server(int * nsd) {
	// Variables para la conexión
    int sd = sd_copy(*nsd);
	int res = 0;
    char buffer[1024];

	// Se obtiene la key
	int key = receive_key(sd);

	// Se obtiene el nombre absoluto del fichero
	char *tuple_name = calloc(PATH_MAX, sizeof(char));
	get_tuple_abs_path(tuple_name, key);
	
	// Se mira si existe
	if (access(tuple_name, F_OK) == 0) {	
		printf("Error: Archivo existe\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
	}
	
	// Mensaje de confirmación
    write_line(sd, "0");
	
	// Crea el fichero
	FILE * tuple;
	tuple = fopen(tuple_name, "w+");
	if (tuple == NULL) {
		perror("");
		close(sd);
		pthread_exit((void*)-1);
	}

    // Recibe value1
    char value1[256];
    if (read_line(sd, value1, 256) == -1) {
        printf("Error: read_line de set_value (value1) incorrecto\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }
    
    // Recibe N_value2
    int N;
    if (read_line(sd, buffer, 1024) == -1) {
        printf("Error: read_line de set_value (N_value2) incorrecto\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }

    N = atoi(buffer);
    if (N == 0){
        printf("Error: atoi en set_value\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }

    // Recibe V_value2
    double value2[N];
    for(int i = 0; i<N; i ++){
        if (read_line(sd, buffer, 1024) == -1) {
            printf("Error: read_line de set_value (V_value2) incorrecto\n");
            write_line(sd, "-1");
            close(sd);
            pthread_exit((void*)-1);
        }
        value2[i] = strtod(buffer, NULL);
    }

	if (fprintf(tuple, "%d\n", key) < 0) {res = -1;}
	if (fprintf(tuple, "%s\n", value1) < 0) {res = -1;}
	if (fprintf(tuple, "%d\n", N) < 0) {res = -1;}
	for (int i = 0; i < N; i++) {
		if (fprintf(tuple, "%lf", value2[i]) < 0) {res = -1;}
		if (i < N -1) {fprintf(tuple, ", ");}
    }
	
    // Cierra la tupla
    fclose(tuple);

    //Enviar menaje de repuesta
    sprintf(buffer, "%i", res);
    int r = write_line(sd, buffer);
    if (r == -1) {
        printf("Error: envio del resultado de set_value\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }
    
    close(sd);
    pthread_exit(NULL);
}

int get_value_server(int * nsd) {
	// Variables para la conexión   
    int sd = sd_copy(*nsd);
	int res = 0;
    char buffer[1024];

    // Se obtiene la key
    int key = receive_key(sd);

	// Se consigue el path de la tupla
    char *tuple_name = calloc(PATH_MAX, sizeof(char));
    get_tuple_abs_path(tuple_name, key);
    
    // Se mira si existe
    if (access(tuple_name, F_OK) == -1) {
        printf("Error: Archivo no existe\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }
    
    // Mensaje de confirmación
    write_line(sd, "0");

    // Abre el archivo
    FILE * tuple;
    tuple = fopen(tuple_name, "r");
    if (tuple == NULL) {
        perror("");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }

    // Lee los datos
    char value1[256];
    int N;
    double value2[32];
    
    // Ley la key
    if (fscanf(tuple, "%d\n", &key) < 1) {res = -1;}

    // Lee value1 y lo manda por el socket
    if (fscanf(tuple, "%[^\n]s\n", value1) < 1) {
    	printf("Error: lectura de value1 de get_value\n");	
    	write_line(sd, "-1");
		close(sd);
		pthread_exit((void *)-1);
    }
    write_line(sd, value1);

    // Lee N_value2 y lo manda por el socket
    if (fscanf(tuple, "%d\n", &N) < 1) {
    	printf("Error: lectura de N_value2 de get_value\n");
    	write_line(sd, "-1");
    	close(sd);
    	pthread_exit((void *)-1);
    }
    
    else if (N > 32 || N<= 0){
    	printf("Error: N_value2 fuera de rango\n");
        write_line(sd, "-1");
    	close(sd);
    	pthread_exit((void *)-1);
    }
    else{
        sprintf(buffer, "%i", N);
		write_line(sd, buffer);
    }

    // Lee V_value2 y lo manda por el socket
    for (int i = 0; i < N; i++) {
        if (fscanf(tuple, "%lf", &value2[i]) < 1) {res = -1;}
        sprintf(buffer, "%lf", value2[i]);
        write_line(sd, buffer);
        if (i < N -1) { fscanf(tuple, ", ");}
    }
  	
    // Cierra la tupla
    fclose(tuple);

    // Devuelve el mensaje de respuesta
    sprintf(buffer, "%i", res);
    int r = write_line(sd, buffer);
    if (r == -1) {
        printf("Error: envio del resultado de get_value\n");
        close(sd);
        pthread_exit((void*)-1);
    }
    close(sd);
    pthread_exit(NULL);
}

int modify_value_server(int * nsd) {
	// Variables para la conexión    
    int sd = sd_copy(*nsd);
    int res = 0;
    char buffer[1024];

    // Se consigue la key
    int key = receive_key(sd);

    // Se obtiene el nombre absoluto del fichero
    char *tuple_name = calloc(PATH_MAX, sizeof(char));
    get_tuple_abs_path(tuple_name, key);

    // Se mira si existe
    if (access(tuple_name, F_OK) == -1) {
        printf("Error: Archivo no existe\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }
	
	// Mensaje de confirmación
    write_line(sd,"0");

    // Crea el fichero
    FILE * tuple;
    tuple = fopen(tuple_name, "w+");
    if (tuple == NULL) {
        perror("");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }

    // Recibe value1
    char value1[256];
    if (read_line(sd, value1, 256) == -1) {
        printf("Error: read_line de modify_value (value1) incorrecto\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }

    // Recibe N_value2
    int N;
    if (read_line(sd, buffer, 1024) == -1) {
        printf("Error: read_line de modify_value (N_value2) incorrecto\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }

    N = atoi(buffer);
    if (N == 0){
        printf("Error: atoi de modify_value\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }

    // Recibe V_value2
    double value2[N];
    for(int i = 0; i<N; i ++){
        if (read_line(sd, buffer, 1024) == -1) {
            printf("Error: read_line de modify_value (V_value2) incorrecto\n");
            write_line(sd, "-1");
            close(sd);
            pthread_exit((void*)-1);
        }
        value2[i] = strtod(buffer, NULL);
    }

    if (fprintf(tuple, "%d\n", key) < 0) {res = -1;}
    if (fprintf(tuple, "%s\n", value1) < 0) {res = -1;}
    if (fprintf(tuple, "%d\n", N) < 0) {res = -1;}
    for (int i = 0; i < N; i++) {
        if (fprintf(tuple, "%lf", value2[i]) < 0) {res = -1;}
        if (i < N -1) {fprintf(tuple, ", ");}
    }

    // Cierra la tupla
    fclose(tuple);

    //Enviar menaje de repuesta
    sprintf(buffer, "%i", res);
    int r = write_line(sd, buffer);
    if (r == -1) {
        printf("Error: envio del resultado de modify_value\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }
    close(sd);

    pthread_exit(NULL);
}

int delete_key_server(int * nsd) {
	// Variables para la conexión
    int sd = sd_copy(*nsd);
    int res = 0;
    char buffer[1024];

    // Se obtiene la key
    int key = receive_key(sd);
	
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
   		printf("Error: el fichero no existe\n");
   		res = -1;
   	}

    //Enviar menaje de repuesta
    sprintf(buffer, "%i", res);
    int r = write_line(sd, buffer);
    if (r == -1) {
        printf("Error: envio del resultado de remove_key\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }
    close(sd);

    pthread_exit(NULL);
}

int exist_server(int * nsd) {
	// Variables para la conexión
    int sd = sd_copy(*nsd);
    int res = 0;
    char buffer[1024];

    // Se obtiene la key
    int key = receive_key(sd);
	
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
    
    // Enviar menaje de repuesta
    sprintf(buffer, "%i", res);
    int r = write_line(sd, buffer);
    if (r == -1) {
        printf("Error: envio del resultado de exist\n");
        write_line(sd, "-1");
        close(sd);
        pthread_exit((void*)-1);
    }
    close(sd);

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
	// Se comprueba el número de args
	if (argc != 2) {
		printf("Error: Puerto del servidor sin especificar\n");
		return -1;
	}
	
	// Se checkea el puerto
	int puerto = atoi(argv[1]);
	if (puerto <= 0) {
		printf("Error: mal puerto\n");
		return -1;
	}

	// Se crean los attr de los threads
	pthread_attr_t attr_thr;
	pthread_attr_init(&attr_thr);
	pthread_attr_setdetachstate(&attr_thr, PTHREAD_CREATE_DETACHED);
	
	// Se le da valor al path de las tuplas
	abs_path = realpath(rel_path, NULL);

	// Creación del socket del servidor
	int socket_server = create_server_socket(puerto, SOCK_STREAM);
	if (socket_server < 0) {
		return -1;
	}
	char buffer[4];
	int new_sd;
	
	// Bucle de espera a las peticiones
	char op;
	while(1) {
		new_sd = accept_server(socket_server);
		
		// Si se acepta correctamente la petición
		if (new_sd >= 0) {
			
			if (read_line(new_sd, buffer, 4) == -1) {
				printf("Error: no se lee la operación\n");
				close(new_sd);
			}
			
			// Si se lee bien la operación
			else {
			
				op = buffer[0];
				printf("Conexión aceptada: procesando petición con id %c\n", op);
				
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
				pthread_mutex_lock(&mutex);
				while (copia == 0){
					pthread_cond_wait(&copiado, &mutex);
				}
				copia = 0;
				pthread_mutex_unlock(&mutex);
			}
		}
	}
	return 0;
}
