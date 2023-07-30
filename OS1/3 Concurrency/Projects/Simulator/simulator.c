//ENRIQUE CAVANILLAS PUGA

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h> 



#define N_PARADAS 5		// número de paradas de la ruta
#define EN_RUTA 0		// autobús en ruta
#define EN_PARADA 1		// autobús en la parada
#define MAX_USUARIOS 40 // capacidad del autobús
#define USUARIOS 4		// numero de usuarios
// estado inicial
int estado = EN_RUTA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0;   // ocupantes que tiene el autobús
// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,...0};
// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0};
// Otras definiciones globales (comunicación y sincronización)

int usuarios_satisfechos = 0;


pthread_mutex_t m;
pthread_cond_t parado;
pthread_cond_t movimiento_usuarios;

pthread_mutex_t completado;


void init(){

	pthread_mutex_init(&m, NULL);
	pthread_mutex_init(&completado,NULL);

	pthread_cond_init(&parado, NULL);
	pthread_cond_init(&movimiento_usuarios, NULL);

}

void Autobus_En_Parada(){
/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
quieran bajar y/o subir la parada actual. Después se pone en marcha */
	pthread_mutex_lock(&m);
	estado = EN_PARADA;
	printf("> Autobus ha llegado a [%d]\n", parada_actual);
	printf("  ----------------------------------------\n");
	pthread_cond_broadcast(&parado);
	while(esperando_parada[parada_actual] + esperando_bajar[parada_actual] > 0){ //espera al ultimo
		pthread_cond_wait(&movimiento_usuarios,&m);	//solo chequea cuando ha habido un movimiento

	}
	printf("  ----------------------------------------\n");
	pthread_mutex_unlock(&m);
}
void Conducir_Hasta_Siguiente_Parada(){
/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */

	pthread_mutex_lock(&m);
	estado = EN_RUTA;
	printf("> Autobus sale de [%d]\n", parada_actual);
	sleep(2);
	printf("En movimiento (%d) pasajeros..\n", n_ocupantes);
	parada_actual = (parada_actual + 1) % N_PARADAS;
	pthread_mutex_unlock(&m);

}
void Subir_Autobus(int id_usuario, int origen){
/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
proporcionar información de depuración */

	pthread_mutex_lock(&m);
	esperando_parada[origen] = esperando_parada[origen] + 1;
	while(estado==EN_RUTA || origen!=parada_actual){
		pthread_cond_wait(&parado,&m);
	}

	n_ocupantes++;	//el enunciado indica que MAX_USUARIOS > USUARIOS asi que no tratamos la posibilidad de que el bus se llene
	esperando_parada[origen] = esperando_parada[origen] - 1;
	pthread_cond_signal(&movimiento_usuarios);
	
	printf("  Usuario [%d] sube \n", id_usuario);

	pthread_mutex_unlock(&m);
	


}
void Bajar_Autobus(int id_usuario, int destino){
/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
proporcionar información de depuración */

	pthread_mutex_lock(&m);
	esperando_bajar[destino]++;
	while(estado==EN_RUTA ||destino!=parada_actual){
		pthread_cond_wait(&parado,&m);
	}
	n_ocupantes--;
	esperando_bajar[destino]--;
	pthread_cond_signal(&movimiento_usuarios);
	printf("  Usuario [%d] baja \n", id_usuario);
	pthread_mutex_unlock(&m);

}

void Usuario(int id_usuario, int origen, int destino){

	Subir_Autobus(id_usuario, origen);

	Bajar_Autobus(id_usuario, destino);

}



void *thread_autobus(void *args)
{
	while (1)
	{
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();

		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}
void *thread_usuario(void *arg)
{
	int id_usuario;
	id_usuario = arg;
	// obtener el id del usario
	while (1)
	{
		int a = rand() % N_PARADAS;
		int b = rand() % N_PARADAS;
		while(a==b) b = rand() % N_PARADAS;

		printf("    Creado/modificado usuario: hilo [%d] trayecto [%d, %d]\n", id_usuario, a, b);

		Usuario(id_usuario, a, b);

		pthread_mutex_lock(&completado);
		usuarios_satisfechos = usuarios_satisfechos + 1;
		printf("      Usuarios satisfechos: %d\n", usuarios_satisfechos);
		pthread_mutex_unlock(&completado);

	}
}


int main(int argc, char *argv[]){
	int i;pthread_t users[USUARIOS];
	pthread_t autobus;
	// Definición de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas
	// Crear el thread Autobus
	
	init();

	for(i=0; i<USUARIOS; i++){
        pthread_create(&users[i], NULL, thread_usuario, (void*) i);
	}
	sleep(1);

    pthread_create(&autobus, NULL, thread_autobus, NULL);

    for(i=0; i<USUARIOS; i++){
        pthread_join(users[i],NULL);
	}

	pthread_join(autobus, NULL);

	return 1;
}