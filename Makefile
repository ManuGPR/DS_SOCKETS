CC=gcc
CFLAGS=-g -Wall
QFLAGS=-lrt
BIN_FILES= servidor cliente test/test_init test/test_set_value test/test_get_value test/test_modify_value test/test_delete_key test/test_exist test/test_concurrencia

.PHONY: all clean 

all: $(BIN_FILES)

clean: 
	rm -f $(BIN_FILES) *.o
	rm -f *.so

cliente: cliente.c libclaves.so tuplas
	$(CC) $(CFLAGS) $(QFLAGS) cliente.c -L. -lclaves -o $@

test/%: test/%.c libclaves.so tuplas
	$(CC) $(CFLAGS) $(QFLAGS) $< -L. -lclaves -o $@

tuplas:
	mkdir -p tuplas

servidor: servidor.c
	$(CC) $(CFLAGS) $(QFLAGS) $^ -o $@ 

libclaves.so: claves.o
	$(CC) -shared $(CFLAGS) -o libclaves.so claves.o 
	
claves.o: claves.c
	$(CC) $(CFLAGS) -c claves.c -o claves.o
