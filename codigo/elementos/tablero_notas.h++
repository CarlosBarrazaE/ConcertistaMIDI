#ifndef TABLERO_NOTAS_H
#define TABLERO_NOTAS_H

#include "elemento.h++"
#include "../recursos/textura_2d.h++"
#include "../recursos/sombreador.h++"
#include "../recursos/rectangulo.h++"
#include "../libmidi/Midi.h"
#include "../octava.h++"
#include "../pista.h++"

class Tablero_Notas : public Elemento
{
private:
	Sombreador *sombreador, *sombreador_solido;
	Textura2D *textura_nota_blanca, *textura_nota_negra, *textura_sombra_nota;
	Rectangulo *fondo, *estructura_nota;
	Texto *texto;

	int x, y, ancho, alto;
	int tiempo_actual_midi;
	int ancho_blanca, ancho_negra;
	int ajuste_x;
	int velocidad_caida;
	TranslatedNoteSet notas;
	MidiEventMicrosecondList lineas;
	std::map<int, Pista*> *pistas;

	void dibujar_lineas_horizontales();
	void dibujar_lineas_verticales();
	void dibujar_notas(Textura2D *textura_nota_blanca, Textura2D *textura_nota_negra);
public:
	Tablero_Notas(int x, int y, int alto, int ancho, Administrador_Recursos *recursos);
	~Tablero_Notas();

	void e_tiempo(int tiempo);
	void e_notas(TranslatedNoteSet notas);
	void e_lineas(MidiEventMicrosecondList lineas);
	void e_pistas(std::map<int, Pista*> *pistas);
	void e_dimension(int ancho, int alto);
	void e_ancho_blanca(int valor);
	void e_ancho_negra(int valor);
	void e_ajuste_x(int valor);
	void c_velocidad_caida(int valor);

	void actualizar(Raton *raton);
	void dibujar();
};

#endif