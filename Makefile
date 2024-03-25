CC=gcc
CFLAGS=-g -Wall
QFLAGS=-lrt
BIN_FILES= servidor cliente test/test_init test/test_set_value test/test_get_value test/test_modify_value test/test_delete_key test/test_exist test/test_concurrencia

.PHONY: all clean 

all: $(BIN_FILES)

clean: 
	rm -f $(BIN_FILES) *.o
	rm -f *.so

cliente: cliente.c libcomm.so libclaves.so tuplas
	$(CC) $(CFLAGS) $(QFLAGS) cliente.c -L. -lclaves -lcomm -o $@

test/%: test/%.c libcomm.so libclaves.so tuplas
	$(CC) $(CFLAGS) $(QFLAGS) $< -L. -lclaves -lcomm -o $@

tuplas:
	mkdir -p tuplas

servidor: servidor.c libcomm.so
	$(CC) $(CFLAGS) $(QFLAGS) servidor.c -L. -lcomm -o $@ 

libcomm.so: comm.o
	$(CC) -shared $(CFLAGS) -o libcomm.so comm.o
	 
comm.o: comm.c
	$(CC) $(CFLAGS) -c comm.c -o comm.o
	
libclaves.so: claves.o libcomm.so
	$(CC) -shared $(CFLAGS) -o libclaves.so claves.o -L. -lcomm
	
claves.o: claves.c
	$(CC) $(CFLAGS) -c claves.c -o claves.o
	
