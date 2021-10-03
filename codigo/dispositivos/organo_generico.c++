#include "organo_generico.h++"

OrganoGenerico::OrganoGenerico(const std::string &nombre, unsigned char canal, unsigned int identificador) : TeclasLuminosas(nombre, identificador)
{
	m_canal = canal;
}

OrganoGenerico::~OrganoGenerico()
{
}

void OrganoGenerico::actualizar(unsigned int diferencia_tiempo)
{
}

void OrganoGenerico::encender(unsigned int id_nota)
{
}

void OrganoGenerico::apagar(unsigned int id_nota)
{
}
