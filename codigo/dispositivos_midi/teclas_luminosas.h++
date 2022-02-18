#ifndef TECLAS_LUMINOSAS_H
#define TECLAS_LUMINOSAS_H

#include "../libreria_midi/evento_midi.h++"
#include <string>
#include <vector>

class Teclas_Luminosas
{
private:
	unsigned int m_identificador;
	std::vector<unsigned char> m_luces_encendidas;

public:
	static std::vector<std::string> Lista;
	static Teclas_Luminosas *Cargar_tecla_luminosa(unsigned int identificador);

	Teclas_Luminosas(unsigned int identificador);
	virtual ~Teclas_Luminosas();

	virtual Evento_Midi actualizar_virtual(unsigned int diferencia_tiempo) = 0;
	virtual Evento_Midi encender_virtual(unsigned char id_nota) = 0;
	virtual Evento_Midi apagar_virtual(unsigned char id_nota) = 0;

	Evento_Midi actualizar(unsigned int diferencia_tiempo);
	Evento_Midi encender(unsigned char id_nota);
	Evento_Midi apagar(unsigned char id_nota);

	unsigned int identificador();
	bool quedan_luces_activas();
	Evento_Midi apagar_siguiente();
};

#endif
