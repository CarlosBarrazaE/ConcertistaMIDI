#include "organo_generico.h++"

OrganoGenerico::OrganoGenerico(unsigned char canal, unsigned int identificador) : TeclasLuminosas(identificador)
{
	m_canal = canal;
}

OrganoGenerico::~OrganoGenerico()
{
}

void OrganoGenerico::actualizar(unsigned int /*diferencia_tiempo*/, MidiCommOut */*dispositivo_salida*/)
{
}

void OrganoGenerico::encender(unsigned int /*id_nota*/, MidiCommOut */*dispositivo_salida*/)
{
}

void OrganoGenerico::apagar(unsigned int /*id_nota*/, MidiCommOut */*dispositivo_salida*/)
{
}
