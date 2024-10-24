#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>

#include "generacion.h"

#define BUFFER_LENGTH 100

static sem_t sem;

void usage(char *program_name) {
    fprintf(stderr, "Uso: %s [temperatura|humedad|viento|precipitacion]\n", program_name);
    exit(1);
}

void signal_handler() {
    printf("Se ha recibido una señal\n");
    sem_post(&sem);
}

void sensor(log_function log, int delay, int error) {
    char buffer[BUFFER_LENGTH];

    // Se inicializa el semaforo
    if (sem_init(&sem, 0, 0)) {
        perror("Error al inicializar el semaforo");
        exit(1);
    }

    // Se asigna el signal 
    if (signal(SIGUSR1, signal_handler)) {
        perror("Error al asignar el signal");
        exit(1);
    }

    // Se abre el pipe de lectura
    int pipe = open("clima.fifo", O_WRONLY);
    if (pipe < 0) {
        perror("Error al abrir el pipe");
        exit(1);
    }
    
    for (;;) {        
        // tiempo de espera aleatorio
        sleep((int) normal(delay, error));
        log(buffer, BUFFER_LENGTH); // Se genera la temperatura
        
        // Se espera a que se reciba una señal
        sem_wait(&sem);

        // Se imprime la humedad
        printf("%s", buffer);

        // Se escribe la humedad en el pipe
        write(pipe, buffer, strlen(buffer));
    }

    // Se cierra el pipe
    close(pipe);

    // Se destruye el semaforo
    if (sem_destroy(&sem)) {
        perror("Error al destruir el semaforo");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    // Se comprueba que se haya pasado un argumento
    if (argc != 2) {
        usage(argv[0]);
    }

    // Se comprueba que el argumento sea correcto
    if (strncmp(argv[1], "temperatura", 11) == 0) {
        sensor(log_temperatura, 10, 2);
    } else if (strncmp(argv[1], "humedad", 7) == 0) {
        sensor(log_humedad, 12, 2);
    } else if (strncmp(argv[1], "viento", 6) == 0) {
        sensor(log_viento, 5, 2);
    } else if (strncmp(argv[1], "precipitacion", 13) == 0) {
        sensor(log_precipitacion, 5, 3);
    } else {
        usage(argv[0]);
    }
}