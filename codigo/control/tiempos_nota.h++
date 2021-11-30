#ifndef TIEMPOS_NOTA
#define TIEMPOS_NOTA

#define TIEMPO_DETECCION_INICIAL 350000
#define TIEMPO_DETECCION_FINAL TIEMPO_DETECCION_INICIAL/2
#define TIEMPO_TOLERANCIA TIEMPO_DETECCION_INICIAL/3

#include "../libmidi/MidiTypes.h++"

struct Tiempos_Nota
{
	unsigned int id_nota = 0;
	microseconds_t inicio = 0;
	microseconds_t fin = 0;
	microseconds_t inicio_tocado = 0;
	microseconds_t fin_tocado = 0;
	bool tocada = false;
};

#endif
