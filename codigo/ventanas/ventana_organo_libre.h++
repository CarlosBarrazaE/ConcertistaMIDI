#ifndef VENTANAORGANOLIBRE_H
#define VENTANAORGANOLIBRE_H

#include "ventana.h++"
#include "../elementos_graficos/etiqueta.h++"
#include "../elementos_graficos/boton.h++"
#include "../version.h++"
#include "../control/configuracion.h++"
#include <vector>

class VentanaOrganoLibre : public Ventana
{
private:
	//Recursos
	Rectangulo *m_rectangulo;

	//Datos
	Configuracion *m_configuracion;

public:
	VentanaOrganoLibre(Configuracion *configuracion, Administrador_Recursos *recursos);
	~VentanaOrganoLibre();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;

	void evento_raton(Raton *raton) override;
	void evento_teclado(Tecla tecla, bool estado) override;
	void evento_pantalla(float ancho, float alto) override;
};
#endif
