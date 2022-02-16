#include "color.h++"

Color::Color()
{
	this->establecer_valores(0.0f, 0.0f, 0.0f, 1.0f);
}

Color::Color(const Color &color)
{
	m_rojo = color.m_rojo;
	m_verde = color.m_verde;
	m_azul = color.m_azul;
	m_alfa = color.m_alfa;
}

Color::Color(int rojo, int verde, int azul)
{
	this->establecer_valores(static_cast<float>(rojo) / 254.0f, static_cast<float>(verde) / 254.0f, static_cast<float>(azul) / 254.0f, 1.0);
}

Color::Color(float rojo, float verde, float azul)
{
	this->establecer_valores(rojo, verde, azul, 1.0f);
}

Color::Color(int rojo, int verde, int azul, int alfa)
{
	this->establecer_valores(static_cast<float>(rojo) / 254.0f, static_cast<float>(verde) / 254.0f, static_cast<float>(azul) / 254.0f, static_cast<float>(alfa) / 254.0f);
}

Color::Color(float rojo, float verde, float azul, float alfa)
{
	this->establecer_valores(rojo, verde, azul, alfa);
}

Color::~Color()
{
}

float Color::valor_limitado(float valor)
{
	//Limita el valor entre 0 y 1
	if(valor < 0.0f)
		return 0.0f;
	else if(valor > 1.0f)
		return 1.0f;
	else
		return valor;
}

void Color::establecer_valores(float rojo, float verde, float azul, float alfa)
{
	m_rojo = this->valor_limitado(rojo);
	m_verde = this->valor_limitado(verde);
	m_azul = this->valor_limitado(azul);
	m_alfa = this->valor_limitado(alfa);
}

bool Color::comparar(float valor_1, float valor_2) const
{
	int resultado_1 = static_cast<int>(valor_1 * 254.0f);
	int resultado_2 = static_cast<int>(valor_2 * 254.0f);
	return resultado_1 == resultado_2;
}

void Color::color(int rojo, int verde, int azul)
{
	this->establecer_valores(static_cast<float>(rojo) / 254.0f, static_cast<float>(verde) / 254.0f, static_cast<float>(azul) / 254.0f, 1.0f);
}

void Color::color(float rojo, float verde, float azul)
{
	this->establecer_valores(rojo, verde, azul, 1.0f);
}

void Color::color(int rojo, int verde, int azul, int alfa)
{
	this->establecer_valores(static_cast<float>(rojo) / 254.0f, static_cast<float>(verde) / 254.0f, static_cast<float>(azul) / 254.0f, static_cast<float>(alfa) / 254.0f);
}

void Color::color(float rojo, float verde, float azul, float alfa)
{
	this->establecer_valores(rojo, verde, azul, alfa);
}

float Color::rojo()
{
	return m_rojo;
}

float Color::verde()
{
	return m_verde;
}

float Color::azul()
{
	return m_azul;
}

float Color::alfa()
{
	return m_alfa;
}

void Color::rojo(float valor)
{
	m_rojo = this->valor_limitado(m_rojo+valor);
}

void Color::verde(float valor)
{
	m_verde = this->valor_limitado(m_verde+valor);
}

void Color::azul(float valor)
{
	m_azul = this->valor_limitado(m_azul+valor);
}

void Color::alfa(float valor)
{
	m_alfa = this->valor_limitado(m_alfa+valor);
}

void Color::tono(float valor)
{
	m_rojo = this->valor_limitado(m_rojo+valor);
	m_verde = this->valor_limitado(m_verde+valor);
	m_azul = this->valor_limitado(m_azul+valor);
}

bool Color::operator == (const Color &c) const
{
	return comparar(m_rojo, c.m_rojo) && comparar(m_verde, c.m_verde) && comparar(m_azul, c.m_azul) && comparar(m_alfa, c.m_alfa);
}

bool Color::operator != (const Color &c) const
{
	return !comparar(m_rojo, c.m_rojo) || !comparar(m_verde, c.m_verde) || !comparar(m_azul, c.m_azul) || !comparar(m_alfa, c.m_alfa);
}

Color& Color::operator = (const Color &c)
{
	if(this != &c)
	{
		m_rojo = c.m_rojo;
		m_verde = c.m_verde;
		m_azul = c.m_azul;
		m_alfa = c.m_alfa;
	}
	return *this;
}

Color& Color::operator + (const Color &c)
{
	m_rojo = this->valor_limitado(m_rojo + c.m_rojo);
	m_verde = this->valor_limitado(m_verde + c.m_verde);
	m_azul = this->valor_limitado(m_azul + c.m_azul);
	m_alfa = this->valor_limitado(m_alfa + c.m_alfa);
	return *this;
}

Color& Color::operator - (const Color &c)
{
	m_rojo = this->valor_limitado(m_rojo - c.m_rojo);
	m_verde = this->valor_limitado(m_verde - c.m_verde);
	m_azul = this->valor_limitado(m_azul - c.m_azul);
	m_alfa = this->valor_limitado(m_alfa - c.m_alfa);
	return *this;
}

Color& Color::operator + (const float &valor)
{
	m_rojo = this->valor_limitado(m_rojo + valor);
	m_verde = this->valor_limitado(m_verde + valor);
	m_azul = this->valor_limitado(m_azul + valor);
	return *this;
}

Color& Color::operator - (const float &valor)
{
	m_rojo = this->valor_limitado(m_rojo - valor);
	m_verde = this->valor_limitado(m_verde - valor);
	m_azul = this->valor_limitado(m_azul - valor);
	return *this;
}
