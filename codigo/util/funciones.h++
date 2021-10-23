#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <string>
#include <vector>
#include <filesystem>

#include "../libmidi/Midi.h++"
#include "../libmidi/MidiTypes.h++"
#include "../registro.h++"

namespace Funciones
{
	std::string microsegundo_a_texto(microseconds_t ms, bool mostrar_vacio);
	microseconds_t duracion_midi(const std::string &ruta_midi);
	bool comparar_float(float valor1, float valor2, float diferencia_minima);
	bool comparar_double(double valor1, double valor2, double diferencia_minima);

	std::string nombre_archivo(const std::string &ruta, bool carpeta);
	std::string extencion_archivo(const std::string &nombre);
	bool es_midi(const std::string &extencion);
	unsigned int numero_de_archivos(const std::string &carpeta);
}

#endif
