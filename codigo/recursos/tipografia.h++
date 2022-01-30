#ifndef TIPOGRAFIA_H
#define TIPOGRAFIA_H

#include <map>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <unicode/unistr.h>

#include "textura_2d.h++"

#include "../registro.h++"
#include "configuracion_cmake.h++"

enum Formato
{
	Normal,
	Negrita,
	Cursiva,
	CursivaNegrita
};

struct Caracter
{
	unsigned int codigo_unicode;
	int ancho;
	int alto;
	int ajuste_izquierda;
	int ajuste_arriba;
	long int avance_x;
	long int avance_y;
	int textura_x;//Posicion dentro del atlas
};

class Tipografia
{
private:
	FT_Library m_libreria;
	FT_Face m_tipografia;

	std::map<unsigned int, Caracter*> m_caracteres;
	unsigned int m_indice_atlas;
	int m_ancho_atlas;
	int m_alto_atlas;

	int m_tamanno_letra;

	void generar_caracteres();
public:
	Tipografia(Formato formato, int tamanno_letra);
	~Tipografia();

	int crear_texto(icu::UnicodeString texto, unsigned int *indice_objeto);
	int alto_texto();
	void activar();
};
#endif
