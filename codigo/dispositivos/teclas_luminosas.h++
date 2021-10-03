#ifndef TECLAS_LUMINOSAS_H
#define TECLAS_LUMINOSAS_H

#include "../libmidi/MidiComm.h++"
#include <string>
#include <vector>

class TeclasLuminosas
{
private:
	unsigned int m_identificador;
	std::string m_nombre;

public:
	static std::vector<TeclasLuminosas*> Lista;

	TeclasLuminosas(const std::string &nombre, unsigned int identificador);
	~TeclasLuminosas();

	virtual void actualizar(unsigned int diferencia_tiempo) = 0;
	virtual void encender(unsigned int id_nota) = 0;
	virtual void apagar(unsigned int id_nota) = 0;

	unsigned int identificador();
	const std::string &nombre();
};

#endif
