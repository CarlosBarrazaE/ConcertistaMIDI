#ifndef VENTANAORGANOLIBRE_H
#define VENTANAORGANOLIBRE_H

#include "ventana.h++"
#include "../elementos_graficos/etiqueta.h++"
#include "../elementos_graficos/boton.h++"
#include "../elementos_graficos/tablero_notas.h++"
#include "../elementos_graficos/organo.h++"
#include "../control/configuracion.h++"
#include "../control/teclado_organo.h++"
#include "../control/nota_activa.h++"
#include "../control/pista.h++"
//#include "../libmidi/Midi.h++"

#include <vector>

class VentanaOrganoLibre : public Ventana
{
private:
	//Recursos
	Rectangulo *m_rectangulo;

	//Componentes
	Tablero_Notas *m_tablero;
	Organo *m_organo;

	//Datos
	Configuracion *m_configuracion;
	Controlador_Midi *m_controlador_midi;
	Teclado_Organo m_teclado_visible, m_teclado_util;
	std::array<Color, 128> m_color_teclas;
	std::map<unsigned int, Nota_Activa*> m_notas_activas;

	//Datos Musica
	std::vector<Pista> m_pistas;

	void insertar_nota(unsigned int id_nota);
	void eliminar_nota(unsigned int id_nota);

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
