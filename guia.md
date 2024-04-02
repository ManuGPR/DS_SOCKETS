# Pasos para compilar y ejecutar

1. Primero, compila el proyecto con el fichero Makefile usando make. Este fichero compilará también las pruebas.

2. En este paso se exportarán las variables de entorno necesarias. Hay dos maneras de hacerlo, con el paso 2.1 o con el paso 2.2.

	**2.1. Usar el fichero de setup:** Antes de ejecutar el cliente y el servidor, ejecuta el fichero *setup.sh* con la sentencia **. setup.sh**. Si se quieren modificar los valores del puerto y la ip del servidor, hay que modificar el fichero *setup.sh* y cambiar lso valores por defecto de *PORT_TUPLAS* e *IP_TUPLAS*, respectivamente. El valor de *LD_LIBRARY_PATH* no debe ser cambiado.

	**2.2. Exportarlas manualmente:** Si se quiere exportar manualmente las variables de entorno, se debe usar el comando *export*. *LD_LIBRARY_PATH* deberá tener el path absoluto al directorio del proyecto. Otra opción para el uso de *IP_TUPLAS* y de *PORT_TUPLAS* es usar el comando: *env IP_TUPLAS=**valor** PORT_TUPLAS=**valor** ./cliente*, haciendo que en la propia ejecución del cliente se determine el valor de las variables de entorno.

3. Ejecuta el servidor con *./servidor PUERTO*, donde *PUERTO* es el valor del puerto del servidor, el cual ha de ser el mismo que el de la variable *PORT_TUPLAS*. Luego, se ejecuta el cliente. Si se ha seguido el paso 2.1, el cliente se ejecutará simplemente con el comando *./cliente*. Si se ha seguido el paso 2.2, el cliente se ejecutará con el comando *./cliente* si se usó el comando *export*; o con el comando *env*, tal y como se describe en el apartado 2.2. Si el cliente y el servidor se ejecutan en dos terminales distintas, **ambas** tienen que tener la variable de entorno *LD_LIBRARY_PATH* correctamente exportada.
 
4. Si se quieren ejecutar los test, ejecuta el fichero *test.sh*. De la misma manera que cliente, si se ha seguido el paso 2.1 o el paso 2.2 con el comando *export*, los test se ejecutarán con *./test.sh*. Si se ha seguido el paso 2.2 con el comando *env*, entonces las pruebas se ejecutarán con el comando: *env IP_TUPLAS=**valor** PORT_TUPLAS=**valor** ./test.sh*.