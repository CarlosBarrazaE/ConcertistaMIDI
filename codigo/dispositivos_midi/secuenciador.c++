#include "secuenciador.h++"

Secuenciador::Secuenciador()
{
	m_cliente = 0;
	m_puerto_entrada = 0;
	m_puerto_salida = 0;
	m_puerto_virtual = 0;
	m_cambio_dispositivos = false;
}

Secuenciador::~Secuenciador()
{
}

unsigned char Secuenciador::cliente()
{
	return m_cliente;
}
