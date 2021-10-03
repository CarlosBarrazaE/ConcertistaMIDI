#ifndef CASIO_LKS250_H
#define CASIO_LKS250_H

#include "teclas_luminosas.h++"

class CasioLks250 : public TeclasLuminosas
{
public:
	CasioLks250(unsigned int identificador);
	~CasioLks250();

	void actualizar(unsigned int diferencia_tiempo);
	void encender(unsigned int id_nota);
	void apagar(unsigned int id_nota);
};

#endif
