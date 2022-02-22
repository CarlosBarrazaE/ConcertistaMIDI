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

	unsigned char id_nota() const;

	void canal(unsigned char canal);
	unsigned char canal() const;

	void velocidad(unsigned char velocidad);
	unsigned char velocidad() const;

	unsigned char programa() const;
	std::string texto() const;

	unsigned char *datos() const;
	unsigned long int largo_datos() const;

	unsigned char cliente() const;
	void cliente(unsigned char cliente);

	unsigned char puerto() const;
	void puerto(unsigned char puerto);
};

#endif
