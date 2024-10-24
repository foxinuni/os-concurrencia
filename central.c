#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SENSORS 4
#define BUFFER_LENGTH 100

const char *sensor_names[SENSORS] = {
    "temperatura",
    "humedad",
    "viento",
    "precipitacion"
};

static unsigned char salir = 0;

void handle_signal() {
    salir = 1;
}

int main() {
    int current = 0;
    char buffer[BUFFER_LENGTH];
    pid_t processes[SENSORS];

    printf("- Central de control - \n");

    // Se abre el pipe de lectura
    if (mkfifo("clima.fifo", 0666)) {
        perror("Error al crear el pipe");
        return 1;
    }

    // handle sigint
    if (signal(SIGINT, handle_signal)) {
        perror("Error al asignar el signal");
        return 1;
    }

    // Se crea el archivo de salida
    int output = open("clima.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (output < 0) {
        perror("Error al crear el archivo de salida");
        return 1;
    }

    // Se crean los procesos hijos
    printf("Creando procesos hijos para cada sensor...\n");
    for (int i = 0; i < SENSORS; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Error al crear el proceso hijo");
            return 1;
        }

        if (pid == 0) {
            // Se ejecuta el sensor
            execl("sensor", "sensor", sensor_names[i], NULL);
            perror("Error al ejecutar el sensor");
            return 1;
        } else {
            processes[i] = pid;
            printf("Proceso hijo %d [%s] creado.\n", pid, sensor_names[i]);
        }
    }

    // Se abre el pipe de lectura
    int pipe = open("clima.fifo", O_RDONLY);
    if (pipe < 0) {
        perror("Error al abrir el pipe");
        return 1;
    }

    while(!salir) {
        // Se envia la señal al proceso hijo
        if (kill(processes[current], SIGUSR1)) {
            perror("Error al enviar la señal");
            return 1;
        }

        // Se lee una linea del pipe
        ssize_t bytes = read(pipe, buffer, BUFFER_LENGTH);
        if (bytes == -1) {
            perror("Error al leer del pipe");
            return 1;
        }

        // Se escribe la linea en el archivo de salida
        if (write(output, buffer, bytes) == -1) {
            perror("Error al escribir en el archivo de salida");
            return 1;
        }

        // Se espera 5 segundos despues de leer de todos los sensores
        current = (current + 1) % SENSORS;
        if (current == 0) {
            sleep(5);
        }
    }

    // Se cierra el archivo de salida
    close(output);

    // Se cierra el pipe
    close(pipe);
    
    // Se eliminan los procesos hijos
    for (int i = 0; i < SENSORS; i++) {
        if (kill(processes[i], SIGKILL) == -1) {
            perror("Error al eliminar el proceso hijo");
            return 1;
        }
    }

    // Se elimina el pipe
    if (unlink("clima.fifo") == -1) {
        perror("Error al eliminar el pipe");
        return 1;
    }

    return 0;
}