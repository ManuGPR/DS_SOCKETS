#include <stdio.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include "mensajes.h"

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

int init_server(struct Peticion * peticion) {
	// Copia de la peticion
	pthread_mutex_lock(&mutex);
	struct Peticion p;
	memcpy(&p, peticion, sizeof(struct Peticion));
	pthread_cond_signal(&copiado);
	copia = 1;
	pthread_mutex_unlock(&mutex);
	
	// Se crea la estructura de respuesta
	struct Respuesta r;
	memset(&r, cero, cero);
	
	// Se crea la cola de respuesta al cliente
	mqd_t q_client = mq_open(p.q_clientname, O_WRONLY);
	
	// Declaración de variables necesarias para el init
	DIR *dir = opendir(abs_path);
	struct dirent* tuplas;
	char* file_name;
	
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
				r.res = -1;
			}
			
			// Se libera el espacio dinámico
			free(file_name);
		}
	}
	
	// Se envía el mensaje 
	mq_send(q_client, (char*)&r, sizeof(r), 0);
	
	// Se cierra la cola
	mq_close(q_client);
	pthread_exit(NULL);
}

int set_value_server(struct Peticion * peticion) {
	// Copia de la peticion
	pthread_mutex_lock(&mutex);
	struct Peticion p;
	memcpy(&p, peticion, sizeof(struct Peticion));
	copia = 1;
	pthread_cond_signal(&copiado);
	pthread_mutex_unlock(&mutex);

    // Se crea la estructura de respuesta
    struct Respuesta r;
    memset(&r, cero, sizeof(r));
	
	// Se crea la cola de respuesta al cliente
    mqd_t q_client = mq_open(p.q_clientname, O_WRONLY);
	
    // Se obtiene el nombre absoluto del fichero
	char *tuple_name = calloc(PATH_MAX, sizeof(char));
	get_tuple_abs_path(tuple_name, p.key);
	
	// Se mira si existe
	if (access(tuple_name, F_OK) == 0) {	
		printf("Archivo existe\n");
		r.res = -1;
		mq_send(q_client, (char*)&r, sizeof(r), 0);
		mq_close(q_client);
		pthread_exit(NULL);
	}
	
	// Crea el fichero
	FILE * tuple;
	tuple = fopen(tuple_name, "w+");
	if (tuple == NULL) {
		perror("");
		r.res = -1;
		mq_send(q_client, (char*)&r, sizeof(r), 0);
		mq_close(q_client);
		pthread_exit(NULL);
	}
	
	// Escribe los datos
	if (fprintf(tuple, "%d\n", p.key) < 0) {r.res = -1;}
	if (fprintf(tuple, "%s\n", p.value1) < 0) {r.res = -1;}
	if (fprintf(tuple, "%d\n", p.N_value2) < 0) {r.res = -1;}
	for (int i = 0; i < p.N_value2; i++) {
		if (fprintf(tuple, "%lf", p.V_value2[i]) < 0) {r.res = -1;}
		if (i < p.N_value2 -1) {fprintf(tuple, ", ");}
    }
     
    // Cierra la tupla
    fclose(tuple);
    
    // Se envía el mensaje
    mq_send(q_client, (char*)&r, sizeof(r), 0);

    // Se cierra la cola
    mq_close(q_client);
    pthread_exit(NULL);
}

int get_value_server(struct Peticion * peticion) {
	// Copia de la peticion
	pthread_mutex_lock(&mutex);
	struct Peticion p;
	memcpy(&p, peticion, sizeof(struct Peticion));
	copia = 1;
	pthread_cond_signal(&copiado);
	pthread_mutex_unlock(&mutex);
	
    // Se crea la estructura de respuesta
    struct Respuesta r;
    memset(&r, cero, sizeof(r));
    
    // Se crea la cola de respuesta al cliente
    mqd_t q_client = mq_open(p.q_clientname, O_WRONLY);
    
    // Se consigue el path de la tupla
    char *tuple_name = calloc(PATH_MAX, sizeof(char));
    get_tuple_abs_path(tuple_name, p.key);
    
    // Se mira si existe
    if (access(tuple_name, F_OK) == -1) {
        printf("Archivo no existe\n");
        r.res = -1;
        mq_send(q_client, (char*)&r, sizeof(r), 0);
        mq_close(q_client);
        pthread_exit(NULL);
    }

    // Abre el archivo
    FILE * tuple;
    tuple = fopen(tuple_name, "r");
    if (tuple == NULL) {
        perror("");
        r.res = -1;
        mq_send(q_client, (char*)&r, sizeof(r), 0);
        mq_close(q_client);
        pthread_exit(NULL);
    }

    // Lee los datos
    int key;
    if (fscanf(tuple, "%d\n", &key) < 1) { printf("1\n"); r.res = -1;}
    if (fscanf(tuple, "%[^\n]s\n", r.value1) < 1) {printf("2\n");r.res = -1;}
    if (fscanf(tuple, "%d\n", &r.N_or_exists) < 1) {printf("3\n");r.res = -1;}
    for (int i = 0; i < r.N_or_exists; i++) {
        if (fscanf(tuple, "%lf", &r.V_value2[i]) < 1) {printf("4\n");r.res = -1;}
        if (i < r.N_or_exists -1) { fscanf(tuple, ", ");}
    }
	if (r.N_or_exists < 1 || r.N_or_exists > 32){
		r.res = -1;
	}
  	
    // Cierra la tupla
    fclose(tuple);

    // Se envía el mensaje
    mq_send(q_client, (char*)&r, sizeof(r), 0);

    // Se cierra la cola
    mq_close(q_client);
    pthread_exit(NULL);
}

int modify_value_server(struct Peticion * peticion) {
	// Copia de la peticion
	pthread_mutex_lock(&mutex);
	struct Peticion p;
	memcpy(&p, peticion, sizeof(struct Peticion));
	copia = 1;
	pthread_cond_signal(&copiado);
	pthread_mutex_unlock(&mutex);
	
    // Se crea la estructura de respuesta
    struct Respuesta r;
    memset(&r, cero, sizeof(r));

    // Se crea la cola de respuesta al cliente
    mqd_t q_client = mq_open(p.q_clientname, O_WRONLY);

    // Se obtiene el nombre absoluto del fichero
    char *tuple_name = calloc(PATH_MAX, sizeof(char));
    get_tuple_abs_path(tuple_name, p.key);

    // Se mira si existe
    if (access(tuple_name, F_OK) == -1) {
        printf("Archivo no existe\n");
        r.res = -1;
        mq_send(q_client, (char*)&r, sizeof(r), 0);
        mq_close(q_client);
        pthread_exit(NULL);
    }

    // Crea el fichero
    FILE * tuple;
    tuple = fopen(tuple_name, "w+");
    if (tuple == NULL) {
        perror("");
        r.res = -1;
        mq_send(q_client, (char*)&r, sizeof(r), 0);
        mq_close(q_client);
        pthread_exit(NULL);
    }

    // Escribe los datos
    if (fprintf(tuple, "%d\n", p.key) < 0) {r.res = -1;}
    if (fprintf(tuple, "%s\n", p.value1) < 0) {r.res = -1;}
    if (fprintf(tuple, "%d\n", p.N_value2) < 0) {r.res = -1;}
    for (int i = 0; i < p.N_value2; i++) {
        if (fprintf(tuple, "%lf", p.V_value2[i]) < 0) {r.res = -1;}
        if (i < p.N_value2 -1) {fprintf(tuple, ", ");}
    }

    // Cierra la tupla
    fclose(tuple);

    // Se envía el mensaje
    mq_send(q_client, (char*)&r, sizeof(r), 0);

    // Se cierra la cola
    mq_close(q_client);
    pthread_exit(NULL);
}

int delete_key_server(struct Peticion * peticion) {
	// Copia de la peticion
	pthread_mutex_lock(&mutex);
	struct Peticion p;
	memcpy(&p, peticion, sizeof(struct Peticion));
	copia = 1;
	pthread_cond_signal(&copiado);
	pthread_mutex_unlock(&mutex);

    // Se crea la estructura de respuesta
    struct Respuesta r;
    memset(&r, cero, sizeof(r));

    // Se crea la cola de respuesta al cliente
    mqd_t q_client = mq_open(p.q_clientname, O_WRONLY);
    
    // Declaración de variables necesarias para el delete key
    DIR *dir = opendir(abs_path);
    struct dirent* tuplas;
    char* file_name;
    char key_str[32];
    sprintf(key_str, "%d", p.key);
    char *tuple_name = (char *)calloc(PATH_MAX, sizeof(char));
	get_tuple_abs_path(tuple_name, p.key);
		
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
			        r.res = -1;
			    }

			    // Se libera el espacio dinámico
			    free(file_name);
			}
		}
   	}
   	else {
   		printf("El fichero no existe\n");
   		r.res = -1;
   	}

    // Se envía el mensaje
    mq_send(q_client, (char*)&r, sizeof(r), 0);

    // Se cierra la cola
    mq_close(q_client);
    pthread_exit(NULL);
}

int exist_server(struct Peticion * peticion) {
	// Copia de la peticion
	pthread_mutex_lock(&mutex);
	struct Peticion p;
	memcpy(&p, peticion, sizeof(struct Peticion));
	copia = 1;
	pthread_cond_signal(&copiado);
	pthread_mutex_unlock(&mutex);
	
    // Se crea la estructura de respuesta
    struct Respuesta r;
    memset(&r, cero, sizeof(r));

    // Se crea la cola de respuesta al cliente
    mqd_t q_client = mq_open(p.q_clientname, O_WRONLY);

	// Datos para el fichero
    char *tuple_name = calloc(PATH_MAX, sizeof(char));
    get_tuple_abs_path(tuple_name, p.key);

    // Se mira si existe
    r.res = access(tuple_name, F_OK);
    if (access(tuple_name, F_OK ) == 0) {
        r.res = 1;
    } else {
        if (errno == ENOENT) {
            r.res = 0;
        } else {
            r.res = -1;
        }
    }

    // Se envía el mensaje
    mq_send(q_client, (char*)&r, sizeof(r), 0);

    // Se cierra la cola
    mq_close(q_client);
    pthread_exit(NULL);
}

int main() {
	// Se crean los attr de los threads
	pthread_attr_t attr_thr;
	pthread_attr_init(&attr_thr);
	pthread_attr_setdetachstate(&attr_thr, PTHREAD_CREATE_DETACHED);
	
	// Se crean los atributos y la estructura de petición
	struct mq_attr attr;
	struct Peticion p;
	
	// Se le da valor al path de las tuplas
	abs_path = realpath(rel_path, NULL);

	// Se inicializan los valores de attr y e p
	attr.mq_msgsize = sizeof(struct Peticion);
	attr.mq_maxmsg = 10;
	memset(&p, cero, sizeof(p));
	
	// Se abre la cola del servidor y se comprueba que se abra bien
	mqd_t q_server = mq_open("/SERVIDOR", O_CREAT | O_RDONLY, 0700, &attr);
	if (q_server == -1) {
		perror("");
		return -1;
	}
	// Bucle de espera a las peticiones
	ssize_t b_read;
	while(1) {
		b_read = mq_receive(q_server, (char *)&p, sizeof(p), NULL);
		if (b_read == -1) {
			perror("");
			return -1;
		}
		pthread_t thread;
		
		// Llamada a las funciones
		switch(p.op) {
			case 0: pthread_create(&thread, &attr_thr, (void*)init_server, (void*)&p);
				break;
			case 1: pthread_create(&thread, &attr_thr, (void*)set_value_server, (void*)&p);
				break;
			case 2: pthread_create(&thread, &attr_thr, (void*)get_value_server, (void*)&p);
				break;
			case 3: pthread_create(&thread, &attr_thr, (void*)modify_value_server, (void*)&p);
				break;
			case 4: pthread_create(&thread, &attr_thr, (void*)delete_key_server,(void*)&p);
				break;
			case 5: pthread_create(&thread, &attr_thr, (void*)exist_server, (void*)&p);
				break;
		}
		// Mutex 
		pthread_mutex_lock(&mutex);
		while(copia == 0) {
			pthread_cond_wait(&copiado, &mutex);
		}
		copia = 0;
		pthread_mutex_unlock(&mutex);
		
	}
	return 0;
}
