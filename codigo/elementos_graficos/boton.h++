#ifndef BOTON_H
#define BOTON_H

#include "elemento.h++"
#include "etiqueta.h++"

//TODO Agregar animacion al cambiar de color
class Boton : public Elemento
{
private:
	Textura2D *m_textura_boton;
	Rectangulo *m_rectangulo;

	Etiqueta *m_texto;
	Color m_color_boton_actual;
	Color m_color_boton_normal;
	Color m_color_boton_sobre;
	Color m_color_boton_activado;
	Color m_color_texto;

	bool m_sobre_boton;
	bool m_boton_pre_activado;
	bool m_boton_activado;

	bool m_habilitado;

	void inicializar();

public:
	Boton(float x, float y, float ancho, float alto, std::string texto, Administrador_Recursos *recursos);
	Boton(float x, float y, float ancho, float alto, std::string texto, ModeloLetra modelo_letra, Administrador_Recursos *recursos);
	//Boton(float x, float y, float ancho, float alto, std::string texto, Textura2D *textura, Color color, bool centrado, Administrador_Recursos *recursos);
	~Boton();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void posicion(float x, float y) override;
	void centrado(bool centrado) override;

	void textura(Textura2D *textura);
	void color_boton(Color color);
	void color_texto(Color color);
	void tipografia(Tipografia *tipografia);

	bool esta_activado();
	void habilitado(bool estado);
};

#endif
