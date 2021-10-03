#ifndef ORGANO_GENERICO_H
#define ORGANO_GENERICO_H

#include "teclas_luminosas.h++"

class OrganoGenerico : public TeclasLuminosas
{
private:
	unsigned char m_canal;
public:
	OrganoGenerico(const std::string &nombre, unsigned char canal, unsigned int identificador);
	~OrganoGenerico();

	void actualizar(unsigned int diferencia_tiempo);
	void encender(unsigned int id_nota);
	void apagar(unsigned int id_nota);
};

#endif
