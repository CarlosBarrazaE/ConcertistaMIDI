#ifndef ORGANO_GENERICO_H
#define ORGANO_GENERICO_H

#include "teclas_luminosas.h++"

class OrganoGenerico : public Teclas_Luminosas
{
private:
	unsigned char m_canal;
public:
	OrganoGenerico(unsigned char canal, unsigned int identificador);
	~OrganoGenerico();

	Evento_Midi actualizar_virtual(unsigned int diferencia_tiempo) override;
	Evento_Midi encender_virtual(unsigned char id_nota) override;
	Evento_Midi apagar_virtual(unsigned char id_nota) override;
};

#endif
