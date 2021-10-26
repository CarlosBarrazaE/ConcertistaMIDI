#include "pista.h++"

//El ultimo color es para poner la pista invisible
Color Pista::Colores_pista[NUMERO_COLORES_PISTA+1] = {
	Color(0.5f, 0.5f, 0.5f),//127.5    127.5     127.5
	Color(0.0f, 0.598f, 0.0f),//0     152.5     0
	Color(0.0f, 0.598f, 1.0f),//0     152.5     255
	Color(1.0f, 0.598f, 1.0f),//255   152.5     255
	Color(1.0f, 0.424f, 0.0f),//255   108.1     0
	Color(0.0f, 0.761f, 0.467f),//0   194.1     119.1
	Color(0.0f, 0.8f, 1.0f),//0       204       255
	Color(1.0f, 0.761f, 0.609f),//255 194.1     155.3
	Color(1.0f, 0.761f, 0.0f),//255   194.1     0
	Color(0.407f, 0.348f, 1.0f),//103,8   88,7    255
};

Pista::Pista(std::string instrumento, unsigned long int numero_notas, unsigned char canal, Color color, Modo modo, bool visible, bool sonido)
{
	m_color_pista = color;
	m_modo_pista = modo;
	m_visible = visible;
	m_sonido = sonido;
	m_instrumento = instrumento;
	m_numero_notas = numero_notas;
	m_canal = canal;
}

Pista::~Pista()
{
}

void Pista::color(Color color)
{
	m_color_pista = color;
}

void Pista::modo(Modo modo)
{
	m_modo_pista = modo;
}

void Pista::visible(bool estado)
{
	m_visible = estado;
}

void Pista::sonido(bool estado)
{
	m_sonido = estado;
}

void Pista::canal(unsigned char canal)
{
	m_canal = canal;
}

Color Pista::color()
{
	return m_color_pista;
}

Modo Pista::modo()
{
	return m_modo_pista;
}

bool Pista::visible()
{
	return m_visible;
}

bool Pista::sonido()
{
	return m_sonido;
}

std::string Pista::instrumento()
{
	return m_instrumento;
}

unsigned long int Pista::numero_notas()
{
	return m_numero_notas;
}

unsigned char Pista::canal()
{
	return m_canal;
}
