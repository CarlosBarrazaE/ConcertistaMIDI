#ifndef LISTA_OPCIONES_H
#define LISTA_OPCIONES_H

#include "elemento.h++"
#include "etiqueta.h++"
#include "boton.h++"
#include "../recursos/textura_2d.h++"
#include <vector>
#include <string>

enum Sentido
{
	NoCambia,
	Anterior,
	Siguiente,
	Inicial,
};

class Lista_Opciones : public Elemento
{
private:
	//Recursos
	Rectangulo *m_rectangulo;
	Etiqueta m_texto_seleccion;
	Boton *m_flecha_izquierda;
	Boton *m_flecha_derecha;
	Color m_color_actual;
	Color m_color_normal;
	Color m_color_sobre;
	Color m_color_activado;

	std::vector<std::string> m_opciones;
	std::vector<Textura2D*> m_iconos;
	bool m_usar_iconos = false;

	bool m_sobre_boton;
	BotonRaton m_boton_pre_activado;
	Sentido m_direccion;
	bool m_cambio_opcion_seleccionada;
	bool m_mostrar_flechas;

	float m_centrado_icono;
	float m_ancho_icono, m_alto_icono;
	unsigned long int m_opcion_actual;
public:
	Lista_Opciones(float x, float y, float ancho, float alto, bool mostrar_flechas, Administrador_Recursos *recursos);
	~Lista_Opciones();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void posicion(float x, float y) override;
	void dimension(float ancho, float alto) override;

	void dimension_icono(float ancho, float alto);
	void opciones_textos(std::vector<std::string> opciones);
	void opciones_iconos(std::vector<Textura2D*> iconos);
	void tipografia(Tipografia *tipografia);
	void color_texto(const Color &color);
	void opcion_predeterminada(unsigned long int opcion);
	void opcion_predeterminada(const std::string &texto);
	unsigned long int opcion_seleccionada();
	std::string opcion_seleccionada_texto();
	bool cambio_opcion_seleccionada();
};

#endif
