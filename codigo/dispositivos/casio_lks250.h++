#ifndef CASIO_LKS250_H
#define CASIO_LKS250_H

#include "teclas_luminosas.h++"

class CasioLks250 : public TeclasLuminosas
{
private:
	unsigned int m_tiempo;
public:
	CasioLks250(unsigned int identificador);
	~CasioLks250();

	void actualizar_virtual(unsigned int diferencia_tiempo, MidiCommOut *dispositivo_salida) override;
	void encender_virtual(unsigned int id_nota, MidiCommOut *dispositivo_salida) override;
	void apagar_virtual(unsigned int id_nota, MidiCommOut *dispositivo_salida) override;
};

#endif
