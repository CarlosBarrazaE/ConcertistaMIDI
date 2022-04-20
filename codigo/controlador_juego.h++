#ifndef CONTROLADOR_JUEGO_H
#define CONTROLADOR_JUEGO_H

#include "recursos/administrador_recursos.h++"
#include "archivos/imagen_tga.h++"
#include "elementos_graficos/etiqueta.h++"
#include "elementos_graficos/notificacion.h++"
#include "dispositivos/teclas.h++"
#include "dispositivos/teclado.h++"
#include "dispositivos/raton.h++"
#include "dispositivos/pantalla.h++"
#include "fps.h++"

#include "ventanas/ventana.h++"
#include "ventanas/ventana_titulo.h++"
#include "ventanas/ventana_seleccion_musica.h++"
#include "ventanas/ventana_seleccion_pista.h++"
#include "ventanas/ventana_organo.h++"
#include "ventanas/ventana_organo_libre.h++"
#include "ventanas/ventana_configuracion.h++"

#include "control/configuracion.h++"
#include "control/datos_musica.h++"

#include <SDL2/SDL.h>

class Controlador_Juego
{
private:
	Administrador_Recursos *m_recursos;
	Rectangulo *m_rectangulo;
	Etiqueta *m_texto_fps;
	Etiqueta *m_informacion;
	Notificacion *m_notificaciones;

	//Control de fps
	bool m_fps_dinamico;
	bool m_fps_reducido;
	float m_contador_inactividad;

	//Control
	bool m_consumir_eventos;
	bool m_depurar;
	bool m_pantalla_completa;
	bool m_modo_alambre;
	bool m_finalizar;
	bool m_aviso_fps_mostrado;
	float m_tiempo_espera_aviso;
	std::set<unsigned int> m_teclas_pulsadas;

	//Eventos
	Raton m_raton;
	Ventana *m_ventana_actual;

	Configuracion m_configuracion;
	Datos_Musica m_musica;

	int m_fotograma;

	void reiniciar_contador_inactividad();
public:
	Controlador_Juego();
	~Controlador_Juego();
	void asignar_administrador_recursos(Administrador_Recursos * recursos);
	Administrador_Recursos *obtener_administrador_recursos();
	void actualizar();

	bool es_pantalla_completa();
	bool modo_alambre_activado();
	bool terminar();

	Raton *raton();

	void eventos_raton();
	void eventos_teclado(Tecla tecla, bool estado);
	void evento_ventana(float ancho, float alto);
	void evento_salir();

	Configuracion *configuracion();
};

#endif
