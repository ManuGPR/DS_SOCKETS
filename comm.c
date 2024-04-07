#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "comm.h"

int create_server_socket(int port, int type) {
	// Función que crea el socket del servidor
	struct sockaddr_in addr_server;
	int sd, res;
	
	// Se crea el socket
	sd = socket(AF_INET, type, 0);
	if (sd < 0) {
		printf("Error en la creación del socket del servidor\n");
		return -1;
	}

	// Se inicializa la dirección	
	bzero((char *)&addr_server, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = INADDR_ANY;
	addr_server.sin_port = htons(port);
	
	// Se bindea
	res = bind(sd, (const struct sockaddr *)&addr_server, sizeof(addr_server));
	if (res == -1) {
		perror("");
		printf("Error en el bind del socket del servidor\n");
		return -1;
	}
	
	// Se hace el listen
	res = listen(sd, SOMAXCONN);
	if (res == -1) {
		printf("Error en el listen del socket del servidor\n");
		return -1;
	}
	
	printf("Servidor conectado, esperando conexiones\n");
	return sd;
}

int create_client_socket(char * remote, int port) {
	// Función que crea el socket del cliente
	struct sockaddr_in addr_server;
	struct hostent * h;
	int sd, res;
	
	// Se crea el socket
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		return -1;
	}
	
	// Se consigue el host
	h = gethostbyname(remote);
	if (h == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}
	
	// Se inicialliza la dirección
	bzero((char *)&addr_server, sizeof(addr_server));
	memcpy(&(addr_server.sin_addr), h->h_addr, h->h_length);
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(port);
	
	// Se manda un connect al socket
	res = connect(sd, (struct sockaddr *)&addr_server, sizeof(addr_server));
	if (res < 0) {
		printf("Error en el connect del cliente\n");
		return -1;
	}
	
	return sd;
}

int accept_server(int sd) {
	// Función que acepta una solicitud de conexión del servidor
	int sc;
	struct sockaddr_in addr_client;
	socklen_t size = sizeof(addr_client);

	// Se hace el accept y se comprueba
	sc = accept(sd, (struct sockaddr *)&addr_client, (socklen_t *)&size); 
	if (sc < 0) {
		printf("Error en el accept del servidor\n");
		return -1;
	}
	
	return sc;
}

int send_message(int sd, char *buffer, int len) {
	// Función que escribe en un socket
	int r, l=len;
	
	do {
		// Se escribe en el socket un mensaje de longitud l
		r = write(sd, buffer, l);
		if (r < 0) {return -1;}
		
		// Se ajustan los punteros
		l = l-r;
		buffer = buffer + r;
	
	} while((l>0) && (r >= 0));
	
	return 0;
}

int receive_message(int sd, char *buffer, int len) {
	// Función que lee de un socket
	int r, l=len;
	
	do {
		// Se lee un mensaje de longitud l
		r = read(sd, buffer, l);
		if (r < 0) {return -1;}
		
		l = l - r;
		buffer = buffer + r;
	} while((l>0) && (r >= 0));
	
	return 0;
}

ssize_t write_line(int fd, char*buffer) {
	// Función que escribe el buffer en un socket
	return send_message(fd, buffer, strlen(buffer)+1);
}

ssize_t read_line(int fd, char *buffer, size_t n) {
	// Función que lee una línea de un file_descriptor / socket
	ssize_t n_read;
	size_t t_read;
	char *buf;
	char ch;
	
	// Si no son adecuados
	if (n <= 0 || buffer == NULL) {
		errno = EINVAL;
		return -1;
	}
	
	buf = buffer;
	t_read = 0;
	
	while(1) {
	
		// Se lee un char y se comprueba si se ha leido
		n_read = read(fd, &ch, 1);
		if (n_read == -1) {
			if (errno == EINTR) {continue;}
			else {return -1;}
		}
		else if(n_read == 0) {
			// Si se ha leído todo lo que se había que leer, se devuelve 0
			if (t_read == 0) {return 0;}
			else break;
		}
		else {
			// si el char es un salto de línea o un final de cadena, se para
			if (ch == '\n') break;
			if (ch == '\0') break;
			if (t_read < n-1) {
				// Se actualizan los contadores y se escribe en buffer
				t_read++;
				*buf++ = ch;
			}
		}
	}
	*buf = '\0';	
	return t_read;
}
