#ifndef REGISTRO_H
#define REGISTRO_H

#include "util/usuario.h++"
#include "util/texto.h++"
#include "configuracion_cmake.h++"

#include <iostream>
#include <fstream>
#include <GL/glew.h>

enum CodigoEstado
{
	EstadoError = 0,
	EstadoAviso = 1,
	EstadoNota = 2,
	EstadoDepurar = 3,
	EstadoNinguno = 4
};

class Registro
{
private:
	static void Escribir_registro(CodigoEstado estado, std::string texto);
public:
	static void Error(std::string texto);
	static void Aviso(std::string texto);
	static void Nota(std::string texto);
	static void Depurar(std::string texto);

	static void Mostrar_detalles();
};

#endif
