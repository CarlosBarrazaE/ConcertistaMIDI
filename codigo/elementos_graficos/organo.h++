#ifndef ORGANO_H
#define ORGANO_H

#define PROPORCION_BLANCA 6.52941f
#define PROPORCION_NEGRA 0.657f
#define SIN_NOTA 130

#include "elemento.h++"
#include "../recursos/generador_particulas.h++"
#include "../recursos/rectangulo.h++"
#include "../util/octava.h++"
#include "../control/pista.h++"
#include "../control/rango_organo.h++"
#include "../control/nota_activa.h++"

#include <array>
#include <vector>

class Organo : public Elemento
{
private:
	Rectangulo *m_rectangulo;

	Textura2D *m_tecla_blanca;
	Textura2D *m_tecla_blanca_presionada;
	Textura2D *m_tecla_blanca_presionada_doble;
	Textura2D *m_tecla_negra;
	Textura2D *m_tecla_negra_presionada;
	Textura2D *m_borde_negro;
	Textura2D *m_borde_rojo;
	Textura2D *m_circulo;

	Generador_Particulas *m_generador_particulas;
	float m_tiempo;
	unsigned int m_numero_particulas;

	Rango_Organo *m_teclado_visible, *m_teclado_util;
	float m_ancho_tecla_blanca, m_ancho_tecla_negra;
	float m_alto_tecla_blanca, m_alto_tecla_negra;

	std::array<Color, 128> *m_notas_activas;
	std::map<unsigned char, Color> *m_notas_requeridas;
	unsigned char m_nota_enviada_anterior;
	std::vector<std::pair<unsigned char, bool>> m_eventos;

	//Metodos
	void dibujar_blancas(float x, float y, unsigned char tecla_inicial, unsigned char numero_teclas);
	void dibujar_negras(float x, float y, unsigned char tecla_inicial, unsigned char numero_teclas);

public:
	Organo(float x, float y, float ancho, Rango_Organo *teclado_visible, Rango_Organo *teclado_util, Administrador_Recursos *recursos);
	~Organo();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void dimension(float ancho, float alto) override;

	void notas_activas(std::array<Color, 128> *notas);
	void notas_requeridas(std::map<unsigned char, Color> *notas_requeridas);

	void calcular_tamannos();
	bool hay_eventos();
	std::pair<unsigned char, bool> obtener_evento();
};

#endif
