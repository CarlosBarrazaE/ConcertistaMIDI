#ifndef TIEMPOS_NOTA
#define TIEMPOS_NOTA

#include "../libmidi/MidiTypes.h++"

struct Tiempos_Nota
{
	microseconds_t inicio = 0;
	microseconds_t fin = 0;
	microseconds_t inicio_tocado = 0;
	microseconds_t fin_tocado = 0;
};

#endif
