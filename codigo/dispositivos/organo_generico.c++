#include "organo_generico.h++"

OrganoGenerico::OrganoGenerico(unsigned char canal, unsigned int identificador) : TeclasLuminosas(identificador)
{
	if(canal > 15)
		m_canal = 15;
	else
		m_canal = canal;
}

OrganoGenerico::~OrganoGenerico()
{
}

void OrganoGenerico::actualizar_virtual(unsigned int /*diferencia_tiempo*/, MidiCommOut */*dispositivo_salida*/)
{
}

void OrganoGenerico::encender_virtual(unsigned int id_nota, MidiCommOut *dispositivo_salida)
{
	MidiEvent evento_nuevo = MidiEvent::Build(MidiEventSimple(0x90 | m_canal, static_cast<unsigned char>(id_nota), 1));
	dispositivo_salida->Write(evento_nuevo);
}

void OrganoGenerico::apagar_virtual(unsigned int id_nota, MidiCommOut *dispositivo_salida)
{
	MidiEvent evento_nuevo = MidiEvent::Build(MidiEventSimple(0x80 | m_canal, static_cast<unsigned char>(id_nota), 0));
	dispositivo_salida->Write(evento_nuevo);
}
