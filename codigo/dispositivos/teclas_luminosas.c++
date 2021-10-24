#include "teclas_luminosas.h++"

#include "organo_generico.h++"
#include "casio_lks250.h++"

std::vector<std::string> TeclasLuminosas::Lista
{
	"Desactivado",
	"Generico - Canal 1",
	"Generico - Canal 2",
	"Generico - Canal 3",
	"Generico - Canal 4",
	"Generico - Canal 5",
	"Generico - Canal 6",
	"Generico - Canal 7",
	"Generico - Canal 8",
	"Generico - Canal 9",
	"Generico - Canal 10",
	"Generico - Canal 11",
	"Generico - Canal 12",
	"Generico - Canal 13",
	"Generico - Canal 14",
	"Generico - Canal 15",
	"Generico - Canal 16",
	"Casio LK-S250 - SysEx",
};

TeclasLuminosas *TeclasLuminosas::Cargar_tecla_luminosa(unsigned int identificador)
{
	switch(identificador)
	{
		case 1: return new OrganoGenerico(0, 1);	//Generico - Canal 1
		case 2: return new OrganoGenerico(1, 2);	//Generico - Canal 2
		case 3: return new OrganoGenerico(2, 3);	//Generico - Canal 3
		case 4: return new OrganoGenerico(3, 4);	//Generico - Canal 4
		case 5: return new OrganoGenerico(4, 5);	//Generico - Canal 5
		case 6: return new OrganoGenerico(5, 6);	//Generico - Canal 6
		case 7: return new OrganoGenerico(6, 7);	//Generico - Canal 7
		case 8: return new OrganoGenerico(7, 8);	//Generico - Canal 8
		case 9: return new OrganoGenerico(8, 9);	//Generico - Canal 9
		case 10: return new OrganoGenerico(9, 10);	//Generico - Canal 10
		case 11: return new OrganoGenerico(10, 11);	//Generico - Canal 11
		case 12: return new OrganoGenerico(11, 12);	//Generico - Canal 12
		case 13: return new OrganoGenerico(12, 13);	//Generico - Canal 13
		case 14: return new OrganoGenerico(13, 14);	//Generico - Canal 14
		case 15: return new OrganoGenerico(14, 15);	//Generico - Canal 15
		case 16: return new OrganoGenerico(15, 16);	//Generico - Canal 16
		case 17: return new CasioLks250(17);
		default: return new OrganoGenerico(0, 0);	//Desactivado
	}
}

TeclasLuminosas::TeclasLuminosas(unsigned int identificador)
{
	m_identificador = identificador;
}

TeclasLuminosas::~TeclasLuminosas()
{
}

void TeclasLuminosas::actualizar(unsigned int diferencia_tiempo, MidiCommOut *dispositivo_salida)
{
	if(dispositivo_salida == NULL)
		return;
	this->actualizar_virtual(diferencia_tiempo, dispositivo_salida);
}

void TeclasLuminosas::encender(unsigned int id_nota, MidiCommOut *dispositivo_salida)
{
	if(dispositivo_salida == NULL)
		return;

	this->encender_virtual(id_nota, dispositivo_salida);
	m_luces_encendidas.push_back(id_nota);
}

void TeclasLuminosas::apagar(unsigned int id_nota, MidiCommOut *dispositivo_salida)
{
	if(dispositivo_salida == NULL)
		return;

	this->apagar_virtual(id_nota, dispositivo_salida);

	//Se borra de la lista
	bool borrado = false;
	for(unsigned int x=0; x<m_luces_encendidas.size() && !borrado; x++)
	{
		if(m_luces_encendidas[x] == id_nota)
		{
			if(x != m_luces_encendidas.size()-1)
				m_luces_encendidas[x] = m_luces_encendidas[m_luces_encendidas.size()-1];//Lo sobrescribe con el ultimo
			m_luces_encendidas.pop_back();//Borra el ultimo
			borrado = true;
		}
	}
}

unsigned int TeclasLuminosas::identificador()
{
	return m_identificador;
}

void TeclasLuminosas::reiniciar(MidiCommOut *dispositivo_salida)
{
	if(dispositivo_salida == NULL)
		return;

	for(unsigned int x=0; x<m_luces_encendidas.size(); x++)
		this->apagar(m_luces_encendidas[x], dispositivo_salida);
	m_luces_encendidas.clear();
}
