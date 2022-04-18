#ifndef TABLERO_NOTAS_H
#define TABLERO_NOTAS_H

#include "elemento.h++"
#include "etiqueta.h++"
#include "../recursos/rectangulo.h++"
#include "../util/octava.h++"
#include "../control/datos_musica.h++"
#include "../control/pista.h++"
#include "../control/rango_organo.h++"
#include "../control/tiempos_nota.h++"
#include "../libmidi/Midi.h++"

#include <vector>
#include <map>

enum Direccion
{
	Bajar,
	Subir,
};

class Tablero_Notas : public Elemento
{
private:
	Administrador_Recursos *m_recursos;
	Textura2D *m_textura_nota, *m_textura_nota_resaltada;
	Rectangulo *m_rectangulo;
	Tipografia *m_tipografia;
	std::map<int, Etiqueta*> m_texto_numeros;

	Rango_Organo *m_teclado_visible, *m_teclado_util;
	microseconds_t m_tiempo_actual_midi;
	float m_ancho_blanca, m_ancho_negra;
	unsigned int m_duracion_nota;
	NotasPistas m_notas;
	MidiEventMicrosecondList m_lineas;

	std::vector<unsigned int> m_ultima_nota;//Ultima nota por cada pista
	std::vector<Pista> *m_pistas;
	Evaluacion *m_evaluacion;

	Direccion m_desplazamiento;

	void calcular_tamannos();

	void dibujar_lineas_horizontales_bajar();
	void dibujar_lineas_horizontales_subir();
	void dibujar_lineas_verticales();
	void dibujar_notas_bajar(unsigned int pista);
	void dibujar_notas_subir(unsigned int pista);

public:
	Tablero_Notas(float x, float y, float ancho, float alto, Rango_Organo *teclado_visible, Rango_Organo *teclado_util, Evaluacion *evaluacion, Administrador_Recursos *recursos);
	~Tablero_Notas();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void dimension(float ancho, float alto) override;

	void tiempo(microseconds_t tiempo);
	void notas(NotasPistas notas);
	void lineas(MidiEventMicrosecondList lineas);
	void pistas(std::vector<Pista> *pistas);
	unsigned int duracion_nota();
	void duracion_nota(int valor);
	void modificar_duracion_nota(unsigned int valor);
	void reiniciar();

	void desplazamiento(Direccion direccion);
};

#endif
