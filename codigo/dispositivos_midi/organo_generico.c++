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
	Evento_Midi evento(EventoMidi_NotaEncendida);
	evento.canal(m_canal);
	evento.id_nota(id_nota);
	evento.velocidad_nota(1);
	return evento;
}

Evento_Midi OrganoGenerico::apagar_virtual(unsigned char id_nota)
{
	Evento_Midi evento(EventoMidi_NotaApagada);
	evento.canal(m_canal);
	evento.id_nota(id_nota);
	evento.velocidad_nota(0);
	return evento;
}
