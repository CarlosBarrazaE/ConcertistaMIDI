#include "evento_midi.h++"
#include "../registro.h++"

Evento_Midi::Evento_Midi()
{
	m_delta_pulso = 0;
	m_tipo_evento = EventoMidi_Nulo;
	m_tipo_metaevento = MetaEventoMidi_Nulo;
	m_datos = NULL;
	m_largo_evento = 0;

	m_cliente = 0;
	m_puerto = 0;
}

Evento_Midi::Evento_Midi(const Evento_Midi &evento)
{
	m_delta_pulso = evento.m_delta_pulso;
	m_tipo_evento = evento.m_tipo_evento;
	m_tipo_metaevento = evento.m_tipo_metaevento;

	m_datos = new unsigned char[evento.m_largo_evento];
	for(unsigned long int x=0; x<evento.m_largo_evento; x++)
		m_datos[x] = evento.m_datos[x];

	m_largo_evento = evento.m_largo_evento;

	m_cliente = evento.m_cliente;
	m_puerto = evento.m_puerto;
}

Evento_Midi::Evento_Midi(EventoMidi tipo_evento)
{
	m_delta_pulso = 0;
	m_tipo_evento = tipo_evento;
	if(	m_tipo_evento == EventoMidi_CambioPrograma ||
		m_tipo_evento == EventoMidi_DespuesDeTocarCanal)
	{
		m_datos = new unsigned char[2] {0, 0};
		m_largo_evento = 2;
	}
	else if	(m_tipo_evento == EventoMidi_NotaApagada ||
			m_tipo_evento == EventoMidi_NotaEncendida ||
			m_tipo_evento == EventoMidi_DespuesDeTocarNota ||
			m_tipo_evento == EventoMidi_Controlador ||
			m_tipo_evento == EventoMidi_InflexionDeTono)
	{
		m_datos = new unsigned char[3] {0, 0, 0};
		m_largo_evento = 3;
	}
	else
	{
		m_datos = NULL;
		m_largo_evento = 0;
	}
	m_tipo_metaevento = MetaEventoMidi_Nulo;

	m_cliente = 0;
	m_puerto = 0;
}

Evento_Midi::Evento_Midi(EventoMidi tipo_evento, unsigned char *datos, unsigned long int largo)
{
	m_delta_pulso = 0;
	m_tipo_evento = tipo_evento;
	m_tipo_metaevento = MetaEventoMidi_Nulo;
	m_datos = datos;
	m_largo_evento = largo;

	m_cliente = 0;
	m_puerto = 0;
}

Evento_Midi::Evento_Midi(EventoMidi tipo_evento, MetaEventoMidi tipo_metaevento, unsigned char *datos, unsigned long int largo)
{
	m_delta_pulso = 0;
	m_tipo_evento = tipo_evento;
	m_tipo_metaevento = tipo_metaevento;
	m_datos = datos;
	m_largo_evento = largo;

	m_cliente = 0;
	m_puerto = 0;
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

void Evento_Midi::id_nota(unsigned char id_nota)
{
	if(m_datos == NULL || m_largo_evento < 3)
		return;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida &&
		m_tipo_evento != EventoMidi_DespuesDeTocarNota)
		return;

	m_datos[1] = id_nota & 0x7F;
}

unsigned char Evento_Midi::id_nota() const
{
	if(m_datos == NULL || m_largo_evento < 3)
		return 0;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida &&
		m_tipo_evento != EventoMidi_DespuesDeTocarNota)
		return 0;

	return m_datos[1];
}

void Evento_Midi::canal(unsigned char canal)
{
	if(m_datos == NULL || m_largo_evento < 2)
		return;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida &&
		m_tipo_evento != EventoMidi_DespuesDeTocarNota &&
		m_tipo_evento != EventoMidi_Controlador &&
		m_tipo_evento != EventoMidi_CambioPrograma &&
		m_tipo_evento != EventoMidi_DespuesDeTocarCanal &&
		m_tipo_evento != EventoMidi_InflexionDeTono)
		return;

	m_datos[0] = canal & 0x0F;
}

unsigned char Evento_Midi::canal() const
{
	if(m_datos == NULL || m_largo_evento < 2)
		return 0;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida &&
		m_tipo_evento != EventoMidi_DespuesDeTocarNota &&
		m_tipo_evento != EventoMidi_Controlador &&
		m_tipo_evento != EventoMidi_CambioPrograma &&
		m_tipo_evento != EventoMidi_DespuesDeTocarCanal &&
		m_tipo_evento != EventoMidi_InflexionDeTono)
		return 0;

	return m_datos[0];
}

void Evento_Midi::velocidad_nota(unsigned char velocidad)
{
	if(m_datos == NULL || m_largo_evento < 3)
		return;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida)
		return;

	if(velocidad > 127)
		m_datos[2] = 127;
	else
		m_datos[2] = velocidad;
}

unsigned char Evento_Midi::velocidad_nota() const
{
	if(m_datos == NULL || m_largo_evento < 3)
		return 0;

	if(	m_tipo_evento != EventoMidi_NotaApagada &&
		m_tipo_evento != EventoMidi_NotaEncendida)
		return 0;

	return m_datos[2];
}

void Evento_Midi::presion_nota(unsigned char presion)
{
	if(m_datos == NULL || m_largo_evento < 3)
		return;

	if(	m_tipo_evento != EventoMidi_DespuesDeTocarNota)
		return;

	if(presion > 127)
		m_datos[2] = 127;
	else
		m_datos[2] = presion;
}

unsigned char Evento_Midi::presion_nota() const
{
	if(m_datos == NULL || m_largo_evento < 3)
		return 0;

	if(	m_tipo_evento != EventoMidi_DespuesDeTocarNota)
		return 0;

	return m_datos[2];
}

void Evento_Midi::presion_canal(unsigned char presion)
{
	if(m_datos == NULL || m_largo_evento < 2)
		return;

	if(	m_tipo_evento != EventoMidi_DespuesDeTocarCanal)
		return;

	if(presion > 127)
		m_datos[1] = 127;
	else
		m_datos[1] = presion;
}

unsigned char Evento_Midi::presion_canal() const
{
	if(m_datos == NULL || m_largo_evento < 2)
		return 0;

	if(	m_tipo_evento != EventoMidi_DespuesDeTocarCanal)
		return 0;

	return m_datos[1];
}

void Evento_Midi::programa(unsigned char programa)
{
	if(m_datos == NULL || m_largo_evento < 2)
		return;

	if(	m_tipo_evento != EventoMidi_CambioPrograma)
		return;

	m_datos[1] = programa & 0x7F;
}

unsigned char Evento_Midi::programa() const
{
	if(m_datos == NULL || m_largo_evento < 2)
		return 0;

	if(	m_tipo_evento != EventoMidi_CambioPrograma)
		return 0;

	return m_datos[1];
}

void Evento_Midi::inflexion_de_tono(unsigned int valor)
{
	if(m_datos == NULL || m_largo_evento < 3)
		return;

	if(	m_tipo_evento != EventoMidi_InflexionDeTono)
		return;

	m_datos[1] = valor & 0x7F;
	m_datos[2] = (valor >> 7) & 0x7F;
}

unsigned int Evento_Midi::inflexion_de_tono() const
{
	if(m_datos == NULL || m_largo_evento < 3)
		return 0;

	if(	m_tipo_evento != EventoMidi_InflexionDeTono)
		return 0;

	return static_cast<unsigned int>((m_datos[2] << 7) | m_datos[1]);
}

void Evento_Midi::controlador_mensaje(unsigned char mensaje)
{
	if(m_datos == NULL || m_largo_evento < 3)
		return;

	if(	m_tipo_evento != EventoMidi_Controlador)
		return;

	m_datos[1] = mensaje & 0x7F;
}

unsigned char Evento_Midi::controlador_mensaje() const
{
	if(m_datos == NULL || m_largo_evento < 3)
		return 0;

	if(	m_tipo_evento != EventoMidi_Controlador)
		return 0;

	return m_datos[1];
}

void Evento_Midi::controlador_valor(unsigned char valor)
{
	if(m_datos == NULL || m_largo_evento < 3)
		return;

	if(	m_tipo_evento != EventoMidi_Controlador)
		return;

	m_datos[2] = valor & 0x7F;
}

unsigned char Evento_Midi::controlador_valor() const
{
	if(m_datos == NULL || m_largo_evento < 3)
		return 0;

	if(	m_tipo_evento != EventoMidi_Controlador)
		return 0;

	return m_datos[2];
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

unsigned char Evento_Midi::cliente() const
{
	return m_cliente;
}

void Evento_Midi::cliente(unsigned char cliente)
{
	m_cliente = cliente;
}

unsigned char Evento_Midi::puerto() const
{
	return m_puerto;
}

void Evento_Midi::puerto(unsigned char puerto)
{
	m_puerto = puerto;
}
