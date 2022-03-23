#ifndef CONTROL_DESLIZANTE
#define CONTROL_DESLIZANTE

#include "elemento.h++"
#include "etiqueta.h++"

class Control_Deslizante : public Elemento
{
private:
	Administrador_Recursos *m_recursos;
	Rectangulo *m_rectangulo;
	Textura2D *m_textura_fondo;
	Textura2D *m_textura_relleno;
	Textura2D *m_textura_boton;

	//Elementos
	Etiqueta m_texto_valor;

	//Datos
	double m_valor_minimo;
	double m_valor_maximo;
	double m_valor_actual;
	double m_valor_paso;

	//Control
	float m_centro_vertical;
	float m_largo_maximo_texto;
	float m_largo_barra;
	float m_largo_barra_color;
	bool m_cambio_valor;
	bool m_moviendo_pre_activado;
	bool m_moviendo_barra;
	float m_posicion_agarre;

	void calcular_valores_nuevos();
public:
	Control_Deslizante(float x, float y, float ancho, float alto, Administrador_Recursos *recursos);
	~Control_Deslizante();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void posicion(float x, float y) override;
	void dimension(float ancho, float alto) override;

	void valor_minimo(double valor);
	void valor_maximo(double valor);
	void cambiar_valor(double valor);
	void valor_paso(double valor);
	double valor_actual();
	bool cambio_valor();
};

#endif
