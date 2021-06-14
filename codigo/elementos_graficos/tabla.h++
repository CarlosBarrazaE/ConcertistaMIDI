#ifndef TABLA_H
#define TABLA_H

#include "elemento.h++"
#include "etiqueta.h++"
#include "fila.h++"
#include "panel_desplazamiento.h++"

#include <vector>

struct Celda
{
	//NOTE posiblemente deberia agregar un campo para saber si es ocultable cuando la tabla es muy chica
	Etiqueta *texto;
	bool centrado;
	unsigned int numero_espacio;
};

class Tabla : public Elemento
{
private:
	Administrador_Recursos *m_recursos;
	std::vector<Celda> m_fila_titulo;//Fila titulo
	std::vector<Fila*> m_filas;

	float m_alto_fila;
	unsigned int m_espacio_total_columnas;
	unsigned long int m_fila_seleccionada;
	bool m_seleccion, m_seleccion_activada;

	//Etiqueta m_textos;
	Rectangulo *m_rectangulo;
	Color m_color_fondo;
	Panel_Desplazamiento *m_panel_desplazamiento;

	void actualizar_ancho_columnas();

public:
	Tabla(float x, float y, float ancho, float alto, float alto_fila, Administrador_Recursos *recursos);
	~Tabla();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void posicion(float x, float y) override;
	void dimension(float ancho, float alto) override;

	void agregar_columna(std::string texto, bool centrado, unsigned int numero_espacio);
	void insertar_fila(std::vector<std::string> texto);
	void vaciar();

	void cambiar_seleccion(int cambio);
	void seleccionar(unsigned long int seleccion);
	unsigned long int obtener_seleccion();
	bool seleccion_activada();
	bool seleccion();
};

#endif
