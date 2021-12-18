#ifndef VENTANATITULO_H
#define VENTANATITULO_H

#include "ventana.h++"
#include "../elementos_graficos/etiqueta.h++"
#include "../elementos_graficos/boton.h++"
#include "../control/configuracion.h++"
#include "version.h++"
#include <vector>

class VentanaTitulo : public Ventana
{
private:
	//Recursos
	Rectangulo *m_rectangulo;
	Textura2D *m_textura_fondo, *m_textura_titulo;
	Etiqueta m_texto_dispositivo_entrada, m_texto_dispositivo_salida;
	Etiqueta m_texto_version;

	//Componetes
	Boton *m_boton_tocar_cancion;
	Boton *m_boton_tocar;
	Boton *m_boton_configurar;
	Boton *m_boton_salir;

public:
	VentanaTitulo(Configuracion *configuracion, Administrador_Recursos *recursos);
	~VentanaTitulo();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;

	void evento_raton(Raton *raton) override;
	void evento_teclado(Tecla tecla, bool estado) override;
	void evento_pantalla(float ancho, float alto) override;
};
#endif
