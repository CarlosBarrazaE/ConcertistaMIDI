#ifndef TECLAS_LUMINOSAS_H
#define TECLAS_LUMINOSAS_H

#include "../libmidi/MidiComm.h++"
#include <string>
#include <vector>

class TeclasLuminosas
{
private:
	unsigned int m_identificador;

public:
	static std::vector<std::string> Lista;
	static TeclasLuminosas *Cargar_tecla_luminosa(unsigned int identificador);

	TeclasLuminosas(unsigned int identificador);
	virtual ~TeclasLuminosas();

	virtual void actualizar(unsigned int diferencia_tiempo, MidiCommOut *dispositivo_salida) = 0;
	virtual void encender(unsigned int id_nota, MidiCommOut *dispositivo_salida) = 0;
	virtual void apagar(unsigned int id_nota, MidiCommOut *dispositivo_salida) = 0;

	unsigned int identificador();
};

#endif
