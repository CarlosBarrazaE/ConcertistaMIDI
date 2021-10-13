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

void OrganoGenerico::actualizar(unsigned int /*diferencia_tiempo*/, MidiCommOut */*dispositivo_salida*/)
{
}

void OrganoGenerico::encender(unsigned int id_nota, MidiCommOut *dispositivo_salida)
{
	MidiEvent evento_nuevo = MidiEvent::Build(MidiEventSimple(0x90 | m_canal, static_cast<unsigned char>(id_nota), 1));
	if(dispositivo_salida != NULL)
		dispositivo_salida->Write(evento_nuevo);
}

void OrganoGenerico::apagar(unsigned int id_nota, MidiCommOut *dispositivo_salida)
{
	MidiEvent evento_nuevo = MidiEvent::Build(MidiEventSimple(0x80 | m_canal, static_cast<unsigned char>(id_nota), 0));
	if(dispositivo_salida != NULL)
		dispositivo_salida->Write(evento_nuevo);
}
