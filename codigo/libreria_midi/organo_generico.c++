#include "organo_generico.h++"

OrganoGenerico::OrganoGenerico(unsigned char canal, unsigned int identificador) : Teclas_Luminosas(identificador)
{
	if(canal > 15)
		m_canal = 15;
	else
		m_canal = canal;
}

OrganoGenerico::~OrganoGenerico()
{
}

Evento_Midi OrganoGenerico::actualizar_virtual(unsigned int /*diferencia_tiempo*/)
{
	return Evento_Midi();
}

Evento_Midi OrganoGenerico::encender_virtual(unsigned char id_nota)
{
	unsigned char *datos = new unsigned char[3] {m_canal, id_nota, 1};
	return Evento_Midi(EventoMidi_NotaEncendida, datos, 3);
}

Evento_Midi OrganoGenerico::apagar_virtual(unsigned char id_nota)
{
	unsigned char *datos = new unsigned char[3] {m_canal, id_nota, 0};
	return Evento_Midi(EventoMidi_NotaApagada, datos, 3);
}
