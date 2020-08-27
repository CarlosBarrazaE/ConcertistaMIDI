#include "registro.h++"
#include <GL/glew.h>
void Registro::Escribir_registro(CodigoEstado estado, std::string texto)
{
	//TODO agregar despues de un \n la etiqueta que corresponda [Error / Aviso / Dato]
	std::ofstream archivo;
	archivo.open(ARCHIVO_REGISTRO, std::ios::app);
	if(estado == EstadoError)
		archivo << "[ERROR] " << texto << "\n";
	else if(estado == EstadoAviso && NIVEL_REGISTRO >= 1)
		archivo << "[AVISO] " << texto << "\n";
	else if(estado == EstadoNota && NIVEL_REGISTRO >= 2)
		archivo << "[NOTA] " << texto << "\n";
	else if(estado == EstadoDepurar && NIVEL_REGISTRO >= 3)
		archivo << "[DEPURAR] " << texto << "\n";

	if(IMPRIMIR_TERMINAL)
	{
		if(estado == EstadoError)
			std::cout << "\033[31m[ERROR]\033[0m " << texto << "\n";
		else if(estado == EstadoAviso && NIVEL_REGISTRO >= 1)
			std::cout << "\033[33m[AVISO]\033[0m " << texto << "\n";
		else if(estado == EstadoNota && NIVEL_REGISTRO >= 2)
			std::cout << "\033[32m[NOTA]\033[0m " << texto << "\n";
		else if(estado == EstadoDepurar && NIVEL_REGISTRO >= 3)
			std::cout << "\033[34m[DEPURAR]\033[0m " << texto << "\n";
	}
}

void Registro::Error(std::string texto)
{
	Escribir_registro(EstadoError, texto);
}

void Registro::Aviso(std::string texto)
{
	Escribir_registro(EstadoAviso, texto);
}

void Registro::Nota(std::string texto)
{
	Escribir_registro(EstadoNota, texto);
}

void Registro::Depurar(std::string texto)
{
	Escribir_registro(EstadoDepurar, texto);
}

void Registro::Mostrar_detalles()
{
	const GLubyte *marca = glGetString(GL_VENDOR);
	const GLubyte *tarjeta_grafica = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *version_glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);

	Registro::Nota("Vendedor: " + std::string(reinterpret_cast<const char*>(marca)));
	Registro::Nota("Tarjeta Grafica: " +std::string(reinterpret_cast<const char*>(tarjeta_grafica)));
	Registro::Nota("Version OpenGl: " + std::string(reinterpret_cast<const char*>(version)));
	Registro::Nota("Version GLSL: " + std::string(reinterpret_cast<const char*>(version_glsl)));

	GLenum parametros[] = {
		GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		GL_MAX_DRAW_BUFFERS,
		GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		GL_MAX_TEXTURE_IMAGE_UNITS,
		GL_MAX_TEXTURE_SIZE,
		GL_MAX_VARYING_FLOATS,
		GL_MAX_VERTEX_ATTRIBS,
		GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		GL_MAX_GEOMETRY_OUTPUT_VERTICES,
		GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS,
	};
	const char* nombres[] = {
		"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
		"GL_MAX_CUBE_MAP_TEXTURE_SIZE",
		"GL_MAX_DRAW_BUFFERS",
		"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
		"GL_MAX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_TEXTURE_SIZE",
		"GL_MAX_VARYING_FLOATS",
		"GL_MAX_VERTEX_ATTRIBS",
		"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_VERTEX_UNIFORM_COMPONENTS",
		"GL_MAX_GEOMETRY_OUTPUT_VERTICES",
		"GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS",
	};

	Registro::Depurar("---------------------------------------");
	Registro::Depurar("Parametros del Contexto GL:");

	int valor = 0;
	for (int x = 0; x < 12; x++)
	{
		glGetIntegerv(parametros[x], &valor);
		Registro::Depurar(std::string(nombres[x]) + " " + std::to_string(valor));
	}

	int valor_doble[2];
	valor_doble[0] = valor_doble[1] = 0;
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, valor_doble);
	Registro::Depurar("GL_MAX_VIEWPORT_DIMS " + std::to_string(valor_doble[0]) + " " + std::to_string(valor_doble[1]));

	unsigned char estereo = 0;
	glGetBooleanv(GL_STEREO, &estereo);
	Registro::Depurar("GL_STEREO " + std::to_string((unsigned int)estereo));
	Registro::Depurar("---------------------------------------");
}
