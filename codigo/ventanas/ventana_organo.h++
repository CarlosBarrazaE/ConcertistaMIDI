#ifndef VENTANAORGANO_H
#define VENTANAORGANO_H

#include "ventana.h++"
#include "../elementos_graficos/barra_progreso.h++"
#include "../elementos_graficos/etiqueta.h++"
#include "../elementos_graficos/tablero_notas.h++"
#include "../elementos_graficos/organo.h++"
#include "../elementos_graficos/titulo.h++"
#include "../elementos_graficos/puntuacion.h++"
#include "../control/pista.h++"
#include "../control/rango_organo.h++"
#include "../control/configuracion.h++"
#include "../control/datos_musica.h++"
#include "../control/nota_activa.h++"
#include "../libmidi/Midi.h++"
#include "../libmidi/MidiComm.h++"
#include "../util/texto.h++"

#include <map>
#include <array>

class VentanaOrgano : public Ventana
{
private:
	//Recursos
	Rectangulo *m_rectangulo;
	Textura2D *m_textura_subtitulo;

	//Componentes
	Barra_Progreso *m_barra;
	Tablero_Notas *m_tablero;
	Organo *m_organo;
	Titulo *m_titulo_musica;
	Puntuacion *m_puntaje;
	Etiqueta m_texto_velocidad;
	Etiqueta m_texto_pausa;
	Etiqueta m_subtitulos;
	Etiqueta m_texto_combos;

	//Controles
	double m_velocidad_musica;
	double m_volumen;
	bool m_cambio_velocidad;
	bool m_pausa;
	bool m_retorno_carro;
	bool m_mostrar_subtitulo;
	bool m_descartar_texto_inicial;
	unsigned int m_duracion_nota;
	double m_volumen_antes_mute;

	//Datos
	Controlador_Midi *m_controlador_midi;
	Rango_Organo m_teclado_visible, m_teclado_util;
	Configuracion *m_configuracion;
	Datos_Musica *m_musica;
	NotasPistas m_notas;
	std::string m_subtitulo_texto;
	std::map<unsigned int, Nota_Activa*> m_notas_activas;
	std::map<unsigned long int, std::vector<Tiempos_Nota>> *m_evaluacion;
	unsigned int m_notas_bloqueadas;
	std::array<Color, 128> m_color_teclas;
	std::array<float, 128> m_tiempo_espera;
	std::map<unsigned int, Color> m_notas_requeridas;
	std::vector<unsigned long int> m_primera_nota;//Ultima nota por cada pista
	std::vector<Pista> *m_pistas;
	microseconds_t m_tiempo_actual_midi;

	void inicializar();
	void recalcular_puntaje();
	void reproducir_eventos(unsigned int microsegundos_actualizar);
	void escuchar_eventos();
	void reproducir_subtitulos(const MidiEvent &evento);

	void calcular_teclas_activas(unsigned int diferencia_tiempo);
	void reiniciar();
	void insertar_nota_activa(unsigned int id_nota, unsigned char canal, unsigned long int pista, unsigned long int posicion, Color color, bool sonido, bool correcta);

	bool hay_nota_nueva(unsigned int id_nota);
	unsigned long int encontrar_nota_tocada(unsigned long int pista, unsigned int id_nota);
	void bloquear_nota(unsigned long int pista, unsigned long int numero_nota);
	void desbloquear_nota(unsigned long int pista, unsigned long int numero_nota);
	void desbloquear_notas(bool desbloquear_todas);

	void agregar_nota_requerida(unsigned int id_nota, const Color &color);
	void borrar_notas_requeridas();
	bool hay_notas_requeridas();

public:
	VentanaOrgano(Configuracion *configuracion, Datos_Musica *musica, Administrador_Recursos *recursos);
	~VentanaOrgano();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;

	void evento_raton(Raton *raton) override;
	void evento_teclado(Tecla tecla, bool estado) override;
	void evento_pantalla(float ancho, float alto) override;
};
#endif
