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

	void actualizar_virtual(unsigned int diferencia_tiempo, MidiCommOut *dispositivo_salida) override;
	void encender_virtual(unsigned int id_nota, MidiCommOut *dispositivo_salida) override;
	void apagar_virtual(unsigned int id_nota, MidiCommOut *dispositivo_salida) override;
};

#endif
