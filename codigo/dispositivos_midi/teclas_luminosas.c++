#include "teclas_luminosas.h++"

#include "organo_generico.h++"
#include "casio_lks250.h++"

std::vector<std::string> Teclas_Luminosas::Lista
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

Teclas_Luminosas *Teclas_Luminosas::Cargar_tecla_luminosa(unsigned int identificador)
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
		default: return NULL;
	}
}

Teclas_Luminosas::Teclas_Luminosas(unsigned int identificador)
{
	m_identificador = identificador;
}

Teclas_Luminosas::~Teclas_Luminosas()
{
}

Evento_Midi Teclas_Luminosas::actualizar(unsigned int diferencia_tiempo)
{
	return this->actualizar_virtual(diferencia_tiempo);
}

Evento_Midi Teclas_Luminosas::encender(unsigned char id_nota)
{
	m_luces_encendidas.push_back(id_nota);
	return this->encender_virtual(id_nota);
}

Evento_Midi Teclas_Luminosas::apagar(unsigned char id_nota)
{
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

	return this->apagar_virtual(id_nota);
}

unsigned int Teclas_Luminosas::identificador() const
{
	return m_identificador;
}

bool Teclas_Luminosas::quedan_luces_activas() const
{
	if(m_luces_encendidas.size() > 0)
		return true;
	return false;
}

Evento_Midi Teclas_Luminosas::apagar_siguiente()
{
	if(m_luces_encendidas.size() > 0)
	{
		Evento_Midi evento = this->apagar_virtual(m_luces_encendidas[m_luces_encendidas.size()-1]);
		m_luces_encendidas.pop_back();//Borra el ultimo
		return evento;
	}

	return Evento_Midi();
}
