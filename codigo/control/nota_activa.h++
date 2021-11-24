#ifndef NOTA_ACTIVA
#define NOTA_ACTIVA

#include "../recursos/color.h++"
#include "../libmidi/Note.h++"

struct Nota_Activa
{
	NoteId id_nota = 0;
	unsigned char canal = 0;
	unsigned long int pista = 0;
	unsigned long int posicion = 0;
	unsigned int contador_clic = 0;
	Color color;
	bool sonido = false;
	bool correcta = false;
};

#endif
