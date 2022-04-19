#ifndef ETIQUETA_H
#define ETIQUETA_H

#include <string>
#include <unicode/unistr.h>
#include "elemento.h++"
#include "../recursos/figura.h++"
#include "../recursos/tipografia.h++"
#include "../registro.h++"

class Etiqueta : public Elemento, public Figura
{
private:
	static Color Ultimo_color;
	Tipografia *m_tipografia = NULL;
	Color m_color;
	icu::UnicodeString m_texto;
	std::string m_texto_actual;
	bool m_centrado_horizontal = false;
	bool m_centrado_vertical = false;
	float m_ancho_texto = 0;
	float m_alto_texto = 0;
	float m_margen = 0;

	Rectangulo *m_rectangulo;
	bool m_mostrar_fondo = false;

	unsigned int m_indice_objeto = 0;

	void actualizar_texto();
	void limpiar();

public:
	Etiqueta(Administrador_Recursos *recursos);
	Etiqueta(float x, float y, bool centrado, const std::string &texto, ModeloLetra tipografia, Administrador_Recursos *recursos);
	Etiqueta(float x, float y, bool centrado, const std::string &texto, Tipografia *tipografia, Administrador_Recursos *recursos);
	Etiqueta(float x, float y, float ancho, float alto, bool centrado, const std::string &texto, ModeloLetra tipografia, Administrador_Recursos *recursos);
	~Etiqueta();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void centrado(bool centrado) override;
	void centrado_horizontal(bool centrado_h);
	void centrado_vertical(bool centrado_v);

	void margen(float margen);

	void texto(const std::string &texto);
	void color(const Color &color);
	void tipografia(Tipografia *tipografia);
	float largo_texto();
	float alto_texto();
	std::string& texto();

	void mostrar_fondo(bool estado);
};

#endif
