#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include "mensajes.h"
#include "claves.h"

int init() {
	//Función init que manda el mensaje de init al servidor
	// Se inicializan las colas
	mqd_t q_server, q_client;
	size_t cero = 0;
	
	// Se inicializan los atributos de las colas
	struct mq_attr attr_r;
	attr_r.mq_msgsize = sizeof(struct Respuesta);
	attr_r.mq_maxmsg = 1;

    //Se escoge el nombre de la cola cliente
	char q_clientname[16];
	sprintf(q_clientname, "/CLIENTE-%d", getpid());

    //Se abren las colas
	q_server = mq_open("/SERVIDOR", O_WRONLY);
	q_client = mq_open(q_clientname, O_CREAT | O_RDONLY, 0700, &attr_r);

    //Se crean los mensajes de peticion y respuesta
	struct Peticion p;
	struct Respuesta r;
	memset(&p, cero, sizeof(p));
	strcpy(p.q_clientname, q_clientname);
	p.op = 0;

    //Se manda la peticion y se recibe la respuesta
	mq_send(q_server, (char *)&p, sizeof(p), 0);
	mq_receive(q_client, (char *)&r, sizeof(r), 0);

    //Se cierran las colas
	mq_close(q_client);
	mq_close(q_server);
	mq_unlink(q_clientname);
	return r.res;
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    //Función set_value que manda el mensaje de set_value al servidor
    // Se inicializan las colas
    mqd_t q_server, q_client;
    size_t cero = 0;

    // Se inicializan los atributos de las colas
    struct mq_attr attr_r;
    attr_r.mq_msgsize = sizeof(struct Respuesta);
    attr_r.mq_maxmsg = 1;

    //Se escoge el nombre de la cola cliente
    char q_clientname[16];
    sprintf(q_clientname, "/CLIENTE-%d", getpid());

    //Se abren las colas
    q_server = mq_open("/SERVIDOR", O_WRONLY);
    q_client = mq_open(q_clientname, O_CREAT | O_RDONLY, 0700, &attr_r);

    //Se crean los mensajes de peticion y respuesta
    struct Peticion p;
    struct Respuesta r;
    memset(&p, cero, sizeof(p));
    
    strcpy(p.q_clientname, q_clientname);
    p.op = 1;
    strcpy(p.value1, value1);
    p.key = key;
    if (N_value2 <= 32 && N_value2 >= 1) {
		p.N_value2 = N_value2;
		memcpy(p.V_value2,V_value2, N_value2 * sizeof(double));
		
		//Se manda la peticion y se recibe la respuesta
		mq_send(q_server, (char *)&p, sizeof(p), 0);
		mq_receive(q_client, (char *)&r, sizeof(r), 0);
    }
    else {
    	r.res = -1;
	}
    
    //Se cierran las colas
    mq_close(q_client);
    mq_close(q_server);
    mq_unlink(q_clientname);
    return r.res;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2){
    //Función set_value que manda el mensaje de get_value al servidor
    // Se inicializan las colas
    mqd_t q_server, q_client;
    size_t cero = 0;

    // Se inicializan los atributos de las colas
    struct mq_attr attr_r;
    attr_r.mq_msgsize = sizeof(struct Respuesta);
    attr_r.mq_maxmsg = 1;

    //Se escoge el nombre de la cola cliente
    char q_clientname[16];
    sprintf(q_clientname, "/CLIENTE-%d", getpid());

    //Se abren las colas
    q_server = mq_open("/SERVIDOR", O_WRONLY);
    q_client = mq_open(q_clientname, O_CREAT | O_RDONLY, 0700, &attr_r);

    //Se crean los mensajes de peticion y respuesta
    struct Peticion p;
    struct Respuesta r;
    memset(&p, cero, sizeof(p));
    strcpy(p.q_clientname, q_clientname);
    p.op = 2;
    p.key = key;

    //Se manda la peticion y se recibe la respuesta
    mq_send(q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    //printf(" value_1 = %s, n_value2 = %d, v_value2 = %lf %lf\n", r.value1, r.N_or_exists, r.V_value2[0], r.V_value2[1]);
    strcpy(value1 , r.value1);
    *N_value2 = r.N_or_exists;
    memcpy(V_value2, r.V_value2, r.N_or_exists * sizeof(double));

    //Se cierran las colas
    mq_close(q_client);
    mq_close(q_server);
    mq_unlink(q_clientname);
    return r.res;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2){
    //Función modify_value que manda el mensaje de modify_value al servidor
    // Se inicializan las colas
    mqd_t q_server, q_client;
    size_t cero = 0;

    // Se inicializan los atributos de las colas
    struct mq_attr attr_r;
    attr_r.mq_msgsize = sizeof(struct Respuesta);
    attr_r.mq_maxmsg = 1;

    //Se escoge el nombre de la cola cliente
    char q_clientname[16];
    sprintf(q_clientname, "/CLIENTE-%d", getpid());

    //Se abren las colas
    q_server = mq_open("/SERVIDOR", O_WRONLY);
    q_client = mq_open(q_clientname, O_CREAT | O_RDONLY, 0700, &attr_r);

    //Se crean los mensajes de peticion y respuesta
    struct Peticion p;
    struct Respuesta r;
    memset(&p, cero, sizeof(p));
    strcpy(p.q_clientname, q_clientname);
    p.op = 3;
    strcpy(p.value1, value1);
    p.key = key;
    p.N_value2 = N_value2;
    memcpy(p.V_value2,V_value2, N_value2 * sizeof(double));

    if (N_value2 <= 32 && N_value2 >= 1) {
        p.N_value2 = N_value2;
        memcpy(p.V_value2,V_value2, N_value2 * sizeof(double));

        //Se manda la peticion y se recibe la respuesta
        mq_send(q_server, (char *)&p, sizeof(p), 0);
        mq_receive(q_client, (char *)&r, sizeof(r), 0);
    }
    else {
        r.res = -1;
    }

    //Se cierran las colas
    mq_close(q_client);
    mq_close(q_server);
    mq_unlink(q_clientname);
    return r.res;
}

int delete_key(int key){
    //Función delete_key que manda el mensaje de delete_key al servidor
    // Se inicializan las colas
    mqd_t q_server, q_client;
    size_t cero = 0;

    // Se inicializan los atributos de las colas
    struct mq_attr attr_r;
    attr_r.mq_msgsize = sizeof(struct Respuesta);
    attr_r.mq_maxmsg = 1;

    //Se escoge el nombre de la cola cliente
    char q_clientname[16];
    sprintf(q_clientname, "/CLIENTE-%d", getpid());

    //Se abren las colas
    q_server = mq_open("/SERVIDOR", O_WRONLY);
    q_client = mq_open(q_clientname, O_CREAT | O_RDONLY, 0700, &attr_r);

    //Se crean los mensajes de peticion y respuesta
    struct Peticion p;
    struct Respuesta r;
    memset(&p, cero, sizeof(p));
    strcpy(p.q_clientname, q_clientname);
    p.op = 4;
    p.key = key;

    //Se manda la peticion y se recibe la respuesta
    mq_send(q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    //Se cierran las colas
    mq_close(q_client);
    mq_close(q_server);
    mq_unlink(q_clientname);
    return r.res;
}
int exist(int key){
    //Función exist que manda el mensaje de exist al servidor
    // Se inicializan las colas
    mqd_t q_server, q_client;
    size_t cero = 0;

    // Se inicializan los atributos de las colas
    struct mq_attr attr_r;
    attr_r.mq_msgsize = sizeof(struct Respuesta);
    attr_r.mq_maxmsg = 1;

    //Se escoge el nombre de la cola cliente
    char q_clientname[16];
    sprintf(q_clientname, "/CLIENTE-%d", getpid());

    //Se abren las colas
    q_server = mq_open("/SERVIDOR", O_WRONLY);
    q_client = mq_open(q_clientname, O_CREAT | O_RDONLY, 0700, &attr_r);

    //Se crean los mensajes de peticion y respuesta
    struct Peticion p;
    struct Respuesta r;
    memset(&p, cero, sizeof(p));
    strcpy(p.q_clientname, q_clientname);
    p.op = 5;
    p.key = key;

    //Se manda la peticion y se recibe la respuesta
    mq_send(q_server, (char *)&p, sizeof(p), 0);
    mq_receive(q_client, (char *)&r, sizeof(r), 0);

    //Se cierran las colas
    mq_close(q_client);
    mq_close(q_server);
    mq_unlink(q_clientname);
    return r.res;
}
