#include "rango_organo.h++"
#include "../util/octava.h++"
Rango_Organo::Rango_Organo()
{
}

void Rango_Organo::cambiar(unsigned char inicial, unsigned char largo)
{
	this->numero_teclas(largo);
	this->tecla_inicial(inicial);
}

unsigned char Rango_Organo::tecla_inicial() const
{
	return m_tecla_inicial;
}

void Rango_Organo::tecla_inicial(unsigned char inicial)
{
	//Minimo dos octava
	if(inicial < 128+24 && inicial+m_numero_teclas <= 128)
		m_tecla_inicial = inicial;
}

unsigned char Rango_Organo::tecla_final() const
{
	return static_cast<unsigned char>(m_tecla_inicial + m_numero_teclas - 1);
}

unsigned char Rango_Organo::numero_teclas() const
{
	return m_numero_teclas;
}

void Rango_Organo::numero_teclas(unsigned char largo)
{
	//Minimo dos octava
	if(largo >= 24 && largo <= 128)
	{
		m_numero_teclas = largo;
		if(m_tecla_inicial + largo > 128)
			m_tecla_inicial = 128 - largo;
	}
}

void Rango_Organo::cargar(std::string datos)
{
	if(datos.length() >= 3)
	{
		std::vector<std::string> datos_divididos = Texto::dividir_texto(datos, ',');
		if(datos_divididos.size() == 2)
		{
			//Se necesita el numero de teclas primero
			this->numero_teclas(static_cast<unsigned char>(std::stoi(datos_divididos[1])));
			this->tecla_inicial(static_cast<unsigned char>(std::stoi(datos_divididos[0])));
		}
	}
}

std::string Rango_Organo::texto()
{
	return std::to_string(static_cast<unsigned int>(m_tecla_inicial)) + "," + std::to_string(static_cast<unsigned int>(m_numero_teclas));
}

bool Rango_Organo::operator == (const Rango_Organo &tc) const
{
	return m_tecla_inicial == tc.m_tecla_inicial && m_numero_teclas == tc.m_numero_teclas;
}

bool Rango_Organo::operator != (const Rango_Organo &tc) const
{
	return m_tecla_inicial != tc.m_tecla_inicial || m_numero_teclas != tc.m_numero_teclas;
}

bool Rango_Organo::operator < (const Rango_Organo &tc) const
{
	return m_numero_teclas < tc.m_numero_teclas;
}

bool Rango_Organo::operator <= (const Rango_Organo &tc) const
{
	return m_numero_teclas <= tc.m_numero_teclas;
}

bool Rango_Organo::operator > (const Rango_Organo &tc) const
{
	return m_numero_teclas > tc.m_numero_teclas;
}

bool Rango_Organo::operator >= (const Rango_Organo &tc) const
{
	return m_numero_teclas >= tc.m_numero_teclas;
}
