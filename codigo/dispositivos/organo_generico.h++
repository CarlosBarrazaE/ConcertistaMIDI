#ifndef ORGANO_GENERICO_H
#define ORGANO_GENERICO_H

#include "teclas_luminosas.h++"

class OrganoGenerico : public TeclasLuminosas
{
private:
	unsigned char m_canal;
public:
	OrganoGenerico(unsigned char canal, unsigned int identificador);
	~OrganoGenerico();

	void actualizar(unsigned int diferencia_tiempo, MidiCommOut *dispositivo_salida);
	void encender(unsigned int id_nota, MidiCommOut *dispositivo_salida);
	void apagar(unsigned int id_nota, MidiCommOut *dispositivo_salida);
};

#endif
