#include "generacion.h"
#include <stdio.h>
#include <stdlib.h>

double normal(double media, double desviacion) {
    double aleatorio = (double) rand() / RAND_MAX;
    return media + desviacion * (2 * aleatorio - 1);
}

void log_temperatura(char* msg, int n) {
    double value = normal(25.0, 5.0);
    snprintf(msg, n, "Temperatura: %f *C\n", value);
}

void log_humedad(char* msg, int n) {
    double value = normal(50.0, 25.0);
    snprintf(msg, n, "Humedad: %f%%\n", value);
}

void log_viento(char* msg, int n) {
    double value = normal(10.0, 5.0);
    snprintf(msg, n, "Viento: %f km/h\n", value);
}


void log_precipitacion(char* msg, int n) {
    double value = normal(0.5, 0.5);
    snprintf(msg, n, "Precipitacion: %f mm\n", value);
}