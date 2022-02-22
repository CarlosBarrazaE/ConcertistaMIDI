#include "casio_lks250.h++"

CasioLks250::CasioLks250(unsigned int identificador) : Teclas_Luminosas(identificador)
{
	m_tiempo = 0;
}

CasioLks250::~CasioLks250()
{
}

Evento_Midi CasioLks250::actualizar_virtual(unsigned int diferencia_tiempo)
{

	m_tiempo += diferencia_tiempo;

	//Se requiere un minimo de 500 ms para mantener las luces encendidas
	if(m_tiempo/1000000 > 450)//450 ms
	{
		m_tiempo = 0;
		unsigned char *datos = new unsigned char[8] {0xF0,0x44,0x7E,0x7E,0x7F,0x00,0x03,0xF7};
		return Evento_Midi(EventoMidi_ExclusivoDelSistema, datos, 8);
	}

	//Evento desconocido
	//unsigned char *desconocido = new unsigned char[9] {0xF0,0x44,0x7E,0x7E,0x7F,0x00,0x06,0x00,0xF7};

	return Evento_Midi();
}

Evento_Midi CasioLks250::encender_virtual(unsigned char id_nota)
{
	unsigned char *datos = new unsigned char[10] {0xF0,0x44,0x7E,0x7E,0x7F,0x02,0x00,id_nota,0x01,0xF7};
	return Evento_Midi(EventoMidi_ExclusivoDelSistema, datos, 10);
}

Evento_Midi CasioLks250::apagar_virtual(unsigned char id_nota)
{
	unsigned char *datos = new unsigned char[10] {0xF0,0x44,0x7E,0x7E,0x7F,0x02,0x00,id_nota,0x00,0xF7};
	return Evento_Midi(EventoMidi_ExclusivoDelSistema, datos, 10);
}
