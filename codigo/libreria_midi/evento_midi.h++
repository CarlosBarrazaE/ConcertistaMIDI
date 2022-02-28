#ifndef EVENTO_MIDI
#define EVENTO_MIDI

#include "definiciones_midi.h++"

#include <string>

class Evento_Midi
{
private:
	unsigned long int m_delta_pulso;
	EventoMidi m_tipo_evento;
	MetaEventoMidi m_tipo_metaevento;
	unsigned char *m_datos;
	unsigned long int m_largo_evento = 0;

	unsigned char m_cliente;
	unsigned char m_puerto;
public:
	Evento_Midi();
	Evento_Midi(const Evento_Midi &evento);
	Evento_Midi(EventoMidi tipo_evento);
	Evento_Midi(EventoMidi tipo_evento, unsigned char *datos, unsigned long int largo);
	Evento_Midi(EventoMidi tipo_evento, MetaEventoMidi tipo_metaevento, unsigned char *datos, unsigned long int largo);
	~Evento_Midi();

	EventoMidi tipo_evento() const;
	MetaEventoMidi tipo_metaevento() const;

	void id_nota(unsigned char id_nota);
	unsigned char id_nota() const;

	void canal(unsigned char canal);
	unsigned char canal() const;

	void velocidad_nota(unsigned char velocidad);
	unsigned char velocidad_nota() const;

	void presion_nota(unsigned char presion);
	unsigned char presion_nota() const;

	void presion_canal(unsigned char presion);
	unsigned char presion_canal() const;

	void programa(unsigned char programa);
	unsigned char programa() const;

	void inflexion_de_tono(unsigned int valor);
	unsigned int inflexion_de_tono() const;

	void controlador_mensaje(unsigned char mensaje);
	unsigned char controlador_mensaje() const;

	void controlador_valor(unsigned char valor);
	unsigned char controlador_valor() const;

	std::string texto() const;

	unsigned char *datos() const;
	unsigned long int largo_datos() const;

	unsigned char cliente() const;
	void cliente(unsigned char cliente);

	unsigned char puerto() const;
	void puerto(unsigned char puerto);
};

#endif
