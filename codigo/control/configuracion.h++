#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include "../libmidi/MidiComm.h++"

#include "base_de_datos.h++"
#include "../dispositivos/teclas_luminosas.h++"
#include "../util/usuario.h++"
#include "../elementos_graficos/notificacion.h++"

class Configuracion
{
private:
	Base_de_Datos m_datos;

	MidiCommIn *m_entrada;
	MidiCommOut *m_salida;

	TeclasLuminosas *m_teclas_luminosas;

public:
	Configuracion();
	~Configuracion();

	Base_de_Datos* base_de_datos();
	std::string leer(std::string atributo);
	bool escribir(std::string atributo, std::string valor);

	void dispositivo_entrada(unsigned int id_entrada);
	void dispositivo_salida(unsigned int id_salida);
	void teclas_luminosas(unsigned int identificador);

	MidiCommIn *dispositivo_entrada();
	MidiCommOut *dispositivo_salida();
	void reconectar();

	TeclasLuminosas *teclas_luminosas();
};

#endif
