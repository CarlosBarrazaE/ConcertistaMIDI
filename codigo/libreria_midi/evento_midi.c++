#include "evento_midi.h++"
#include "../registro.h++"

Evento_Midi::Evento_Midi()
{
	m_delta_pulso = 0;
	m_tipo_evento = EventoMidi_Nulo;
	m_tipo_metaevento = MetaEventoMidi_Nulo;
	m_datos = NULL;
	m_largo_evento = 0;
}

Evento_Midi::Evento_Midi(const Evento_Midi &evento)
{
	Registro::Aviso("Copiando evento");
	m_delta_pulso = evento.m_delta_pulso;
	m_tipo_evento = evento.m_tipo_evento;
	m_tipo_metaevento = evento.m_tipo_metaevento;

	m_datos = new unsigned char[evento.m_largo_evento];
	for(unsigned long int x=0; x<evento.m_largo_evento; x++)
		m_datos[x] = evento.m_datos[x];

	m_largo_evento = evento.m_largo_evento;
}

Evento_Midi::Evento_Midi(EventoMidi tipo_evento, unsigned char *datos, unsigned long int largo)
{
	m_delta_pulso = 0;
	m_tipo_evento = tipo_evento;
	m_tipo_metaevento = MetaEventoMidi_Nulo;
	m_datos = datos;
	m_largo_evento = largo;
}

Evento_Midi::Evento_Midi(EventoMidi tipo_evento, MetaEventoMidi tipo_metaevento, unsigned char *datos, unsigned long int largo)
{
	m_delta_pulso = 0;
	m_tipo_evento = tipo_evento;
	m_tipo_metaevento = tipo_metaevento;
	m_datos = datos;
	m_largo_evento = largo;
}

Evento_Midi::~Evento_Midi()
{
	if(m_datos != NULL)
		delete[] m_datos;
}

EventoMidi Evento_Midi::tipo_evento() const
{
	return m_tipo_evento;
}

MetaEventoMidi Evento_Midi::tipo_metaevento() const
{
	return m_tipo_metaevento;
}

unsigned char Evento_Midi::id_nota() const
{
	if(m_datos == NULL || m_largo_evento < 3)
		return 0;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida &&
		m_tipo_evento != EventoMidi_DespuesDeTocarTecla)
		return 0;

	return m_datos[1];
}

void Evento_Midi::canal(unsigned char canal)
{
	if(m_datos == NULL || m_largo_evento < 2)
		return;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida &&
		m_tipo_evento != EventoMidi_DespuesDeTocarTecla &&
		m_tipo_evento != EventoMidi_CambioControl &&
		m_tipo_evento != EventoMidi_CambioPrograma &&
		m_tipo_evento != EventoMidi_DespuesDeTocarCanal &&
		m_tipo_evento != EventoMidi_CambioRuedaDeTono)
		return;

	m_datos[0] = canal;
}

unsigned char Evento_Midi::canal() const
{
	if(m_datos == NULL || m_largo_evento < 2)
		return 0;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida &&
		m_tipo_evento != EventoMidi_DespuesDeTocarTecla &&
		m_tipo_evento != EventoMidi_CambioControl &&
		m_tipo_evento != EventoMidi_CambioPrograma &&
		m_tipo_evento != EventoMidi_DespuesDeTocarCanal &&
		m_tipo_evento != EventoMidi_CambioRuedaDeTono)
		return 0;

	return m_datos[0];
}

void Evento_Midi::velocidad(unsigned char velocidad)
{
	if(m_datos == NULL || m_largo_evento < 3)
		return;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida)
		return;

	m_datos[2] = velocidad;
}

unsigned char Evento_Midi::velocidad() const
{
	if(m_datos == NULL || m_largo_evento < 3)
		return 0;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida)
		return 0;

	return m_datos[2];
}

unsigned char Evento_Midi::programa() const
{
	if(m_datos == NULL || m_largo_evento < 2)
		return 0;

	if(	m_tipo_evento != EventoMidi_CambioPrograma)
		return 0;

	return m_datos[1];
}

std::string Evento_Midi::texto() const
{
	return "texto evento - reemplazar por valor real";
}

unsigned char *Evento_Midi::datos() const
{
	return m_datos;
}

unsigned long int Evento_Midi::largo_datos() const
{
	return m_largo_evento;
}
