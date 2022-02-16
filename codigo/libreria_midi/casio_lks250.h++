#ifndef CASIO_LKS250_H
#define CASIO_LKS250_H

#include "teclas_luminosas.h++"

class CasioLks250 : public Teclas_Luminosas
{
private:
	unsigned int m_tiempo;
public:
	CasioLks250(unsigned int identificador);
	~CasioLks250();

	Evento_Midi actualizar_virtual(unsigned int diferencia_tiempo) override;
	Evento_Midi encender_virtual(unsigned char id_nota) override;
	Evento_Midi apagar_virtual(unsigned char id_nota) override;
};

#endif
