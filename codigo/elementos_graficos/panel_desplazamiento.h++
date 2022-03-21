#ifndef PANEL_DESPLAZAMIENTO_H
#define PANEL_DESPLAZAMIENTO_H

#include "elemento.h++"
#include "../dispositivos/pantalla.h++"
#include "../util/funciones.h++"
#include <vector>

class Panel_Desplazamiento : public Elemento
{
private:
	//Recursos
	Rectangulo *m_rectangulo;
	Textura2D *m_barra;

	float m_alto_actual;
	float m_desplazamiento_x, m_desplazamiento_y;
	float m_columna, m_fila;
	float m_margen_columna, m_margen_fila;
	bool m_calcular_posicion;
	float m_proporcion;
	float m_porcentaje_agarre_raton;

	float m_animacion = 1;
	float m_tiempo_inactividad = 0;
	bool m_sobre_barra;
	bool m_boton_activado;
	bool m_mostrar_barra;
	Raton m_raton_modificado;

	std::vector<Elemento*> m_elementos;

	void inicializar(Administrador_Recursos *recursos);

public:
	Panel_Desplazamiento(float x, float y, float ancho, float alto, float margen_fila, Administrador_Recursos *recursos);
	Panel_Desplazamiento(float x, float y, float ancho, float alto, float columna, float fila, float margen_columna, float margen_fila, Administrador_Recursos *recursos);
	~Panel_Desplazamiento();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void posicion(float x, float y) override;
	void dimension(float ancho, float alto) override;

	void desplazar_y(int dy);

	void agregar_elemento(Elemento *e);
	void vaciar();
	void actualizar_dimension();
};

#endif
