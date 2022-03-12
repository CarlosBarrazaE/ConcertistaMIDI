#ifndef CASILLA_VERIFICACION_H
#define CASILLA_VERIFICACION_H

#include "elemento.h++"
#include "etiqueta.h++"

class Casilla_Verificacion : public Elemento
{
private:
	Textura2D *m_textura_activa, *m_textura_inactiva, *m_textura_sombra;
	Rectangulo *m_rectangulo;

	Etiqueta *m_texto;
	Color m_color_textura;

	bool m_activo, m_clic, m_cambio_estado;
	bool m_boton_pre_activado, m_boton_sobre;

public:
	Casilla_Verificacion(float x, float y, float ancho, float alto, std::string texto, Administrador_Recursos *recursos);
	~Casilla_Verificacion();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void estado(bool estado);
	bool activado();
	bool cambio_estado();
};

#endif
