#include "casio_lks250.h++"

CasioLks250::CasioLks250(unsigned int identificador) : TeclasLuminosas(identificador)
{
	m_tiempo = 0;
}

CasioLks250::~CasioLks250()
{
}

void CasioLks250::actualizar(unsigned int diferencia_tiempo, MidiCommOut *dispositivo_salida)
{

	m_tiempo += diferencia_tiempo;

	//Se requiere un minimo de 500 ms para mantener las luces encendidas
	if(m_tiempo/1000000 > 450)//450 ms
	{
		char *datos = new char [9] {'\xF0','\x44','\x7E','\x7E','\x7F','\x00','\x03','\xF7','0'};

		if(dispositivo_salida != NULL)
			dispositivo_salida->evento_sysex(datos, 9);
		m_tiempo = 0;
		delete[] datos;
	}

	//Evento desconocido
	//char *desconocido = new char[10] {'\xF0','\x44','\x7E','\x7E','\x7F','\x00', '\x06','\x00','\xF7','0'};
	//if(dispositivo_salida != NULL)
	//	dispositivo_salida->evento_sysex(desconocido, 10);
}

void CasioLks250::encender(unsigned int id_nota, MidiCommOut *dispositivo_salida)
{
	char *datos = new char[11] {'\xF0','\x44','\x7E','\x7E','\x7F','\x02','\x00', static_cast<char>(id_nota),'\x01','\xF7','0'};
	if(dispositivo_salida != NULL)
		dispositivo_salida->evento_sysex(datos, 11);
	delete[] datos;
}

void CasioLks250::apagar(unsigned int id_nota, MidiCommOut *dispositivo_salida)
{
	char *datos = new char[11] {'\xF0','\x44','\x7E','\x7E','\x7F','\x02','\x00', static_cast<char>(id_nota),'\x00','\xF7','0'};
	if(dispositivo_salida != NULL)
		dispositivo_salida->evento_sysex(datos, 11);
	delete[] datos;
}
