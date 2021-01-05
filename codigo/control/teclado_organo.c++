#include "teclado_organo.h++"
#include "../util/octava.h++"
Teclado_Organo::Teclado_Organo()
{
}

void Teclado_Organo::cambiar(unsigned int inicial, unsigned int largo)
{
	this->numero_teclas(largo);
	this->tecla_inicial(inicial);
}

unsigned int Teclado_Organo::tecla_inicial()
{
	return m_tecla_inicial;
}

void Teclado_Organo::tecla_inicial(unsigned int inicial)
{
	//Minimo dos octava
	if(inicial < 128+24 && inicial+m_numero_teclas <= 128)
		m_tecla_inicial = inicial;
}

unsigned int Teclado_Organo::numero_teclas()
{
	return m_numero_teclas;
}

void Teclado_Organo::numero_teclas(unsigned int largo)
{
	//Minimo dos octava
	if(largo >= 24 && largo <= 128)
	{
		m_numero_teclas = largo;
		if(m_tecla_inicial + largo > 128)
			m_tecla_inicial = 128 - largo;
	}
}

void Teclado_Organo::cargar(std::string datos)
{
	if(datos.length() >= 3)
	{
		std::vector<std::string> datos_divididos = Texto::dividir_texto(datos, ',');
		if(datos_divididos.size() == 2)
		{
			this->numero_teclas(static_cast<unsigned int>(std::stoi(datos_divididos[1])));
			this->tecla_inicial(static_cast<unsigned int>(std::stoi(datos_divididos[0])));
		}
	}
}

std::string Teclado_Organo::texto()
{
	return std::to_string(m_tecla_inicial) + "," + std::to_string(m_numero_teclas);
}
