#ifndef VENTANA_H
#define VENTANA_H

#include "../dispositivos/pantalla.h++"
#include "../dispositivos/teclas.h++"
#include "../dispositivos/raton.h++"
#include "../registro.h++"

enum Accion
{
	Ninguna,
	CambiarATitulo,
	CambiarAConfiguracion,
	CambiarASeleccionMusica,
	CambiarASeleccionPista,
	CambiarAOrgano,
	CambiarAOrganoLibre,
	Salir,
	EntrarPantallaCompleta,
	SalirPantallaCompleta,
	EntrarModoAlambre,
	SalirModoAlambre,
	EntrarModoDesarrollo,
	SalirModoDesarrollo,
};

class Ventana
{
protected:
	Accion m_accion;
public:
	Ventana();
	virtual ~Ventana();

	virtual void actualizar(unsigned int diferencia_tiempo) = 0;
	virtual void dibujar() = 0;
	virtual void evento_raton(Raton *raton) = 0;
	virtual void evento_teclado(Tecla tecla, bool estado) = 0;
	virtual void evento_pantalla(float ancho, float alto) = 0;

	Accion obtener_accion();
};
#endif
