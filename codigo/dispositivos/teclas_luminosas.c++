#include "teclas_luminosas.h++"

#include "organo_generico.h++"
#include "casio_lks250.h++"

std::vector<TeclasLuminosas*> TeclasLuminosas::Lista
{
	//NOTE El numero del identificador no se puede cambiarse ni repetirse
	//porque se usa para guardar la configuracion en la base de datos
	new OrganoGenerico("Generico - Canal 1", 0, 1),
	new OrganoGenerico("Generico - Canal 2", 1, 2),
	new OrganoGenerico("Generico - Canal 3", 2, 3),
	new OrganoGenerico("Generico - Canal 4", 3, 4),
	new OrganoGenerico("Generico - Canal 5", 4, 5),
	new OrganoGenerico("Generico - Canal 6", 5, 6),
	new OrganoGenerico("Generico - Canal 7", 6, 7),
	new OrganoGenerico("Generico - Canal 8", 7, 8),
	new OrganoGenerico("Generico - Canal 9", 8, 9),
	new OrganoGenerico("Generico - Canal 10", 9, 10),
	new OrganoGenerico("Generico - Canal 11", 10, 11),
	new OrganoGenerico("Generico - Canal 12", 11, 12),
	new OrganoGenerico("Generico - Canal 13", 12, 13),
	new OrganoGenerico("Generico - Canal 14", 13, 14),
	new OrganoGenerico("Generico - Canal 15", 14, 15),
	new OrganoGenerico("Generico - Canal 16", 15, 16),
	new CasioLks250(17),
};

TeclasLuminosas::TeclasLuminosas(const std::string &nombre, unsigned int identificador)
{
	if(identificador == 0)
		Registro::Error("El identificador tiene que ser mayor a 0");//TODO lanzar a una excepcion
	m_identificador = identificador;
	m_nombre = nombre;
}

TeclasLuminosas::~TeclasLuminosas()
{
}

unsigned int TeclasLuminosas::identificador()
{
	return m_identificador;
}

const std::string &TeclasLuminosas::nombre()
{
	return m_nombre;
}
