#ifndef TEXTO_H
#define TEXTO_H

#include <string>
#include <vector>
//Convertir a utf8
#include <unicode/unistr.h>
#include <unicode/ucnv.h>

#include "../registro.h++"

namespace Texto
{
	std::vector<std::string> dividir_texto(const std::string &texto, char caracter);
	std::string convertir_a_utf8(const char *entrada, int largo, const char *nombre_codificacion);
	bool esta_vacio(const std::string &texto);
	std::string bytes_a_texto(unsigned long int tamanno);
	std::string primera_letra_mayuscula(const std::string &texto);
	std::string quitar_espacios_en_extremos(std::string texto);
	bool contiene_caracter(const std::string &texto, char caracter);
}

#endif
