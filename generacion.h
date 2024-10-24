#ifndef GENERACION_H
#define GENERACION_H

typedef void (*log_function)(char*, int);

double normal(double media, double desviacion);
void log_temperatura(char* msg, int n);
void log_humedad(char* msg, int n);
void log_viento(char* msg, int n);
void log_precipitacion(char* msg, int n);

#endif