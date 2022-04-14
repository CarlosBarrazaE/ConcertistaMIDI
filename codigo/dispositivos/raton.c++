#include "raton.h++"

Raton::Raton()
{
	m_posicion_x = 0;
	m_posicion_y = 0;
	m_desplazamiento_x = 0;
	m_desplazamiento_y = 0;
	m_boton_izquierdo = false;
	m_boton_central = false;
	m_boton_derecho = false;
	m_numero_clics = 0;
	m_ultimo_evento = EventoVacio;
}

void Raton::actualizar_boton(BotonRaton boton, bool estado, int numero_clics)
{
	if(boton == BotonIzquierdo)
		m_boton_izquierdo = estado;
	else if(boton == BotonCentral)
		m_boton_central = estado;
	else if(boton == BotonDerecho)
		m_boton_derecho = estado;

	m_numero_clics = numero_clics;

	m_desplazamiento_x = 0;
	m_desplazamiento_y = 0;
	m_ultimo_evento = EventoClic;
}

void Raton::actualizar_posicion(int x, int y)
{
	m_posicion_x = x;
	m_posicion_y = y;
	m_ultimo_evento = EventoPosicion;
}

void Raton::actualizar_desplazamiento(int desplazamiento_x, int desplazamiento_y)
{
	m_desplazamiento_x = desplazamiento_x;
	m_desplazamiento_y = desplazamiento_y;
	m_ultimo_evento = EventoDesplazamiento;
}

void Raton::anular_desplazamiento()
{
	m_desplazamiento_x = 0;
	m_desplazamiento_y = 0;
}

bool Raton::activado(BotonRaton boton) const
{
	if(boton == BotonIzquierdo)
		return m_boton_izquierdo;
	else if(boton == BotonCentral)
		return m_boton_central;
	else if(boton == BotonDerecho)
		return m_boton_derecho;
	return false;
}

BotonRaton Raton::boton_activado() const
{
	if(m_boton_izquierdo)
		return BotonIzquierdo;
	else if(m_boton_central)
		return BotonCentral;
	else if(m_boton_derecho)
		return BotonDerecho;
	return Ninguno;
}

int Raton::numero_clics() const
{
	return m_numero_clics;
}

int Raton::x() const
{
	return m_posicion_x;
}

int Raton::y() const
{
	return m_posicion_y;
}

int Raton::dx() const
{
	return m_desplazamiento_x;
}

int Raton::dy() const
{
	return m_desplazamiento_y;
}

bool Raton::ultimo_evento_fue(const EventoRaton &tipo) const
{
	//Retorna
	if(m_ultimo_evento == tipo)
		return true;
	return false;
}

bool Raton::esta_sobre(float x, float y, float ancho, float alto) const
{
	if(this->x() >= static_cast<int>(x) && this->x() < static_cast<int>(x + ancho) &&
		this->y() >= static_cast<int>(y) && this->y() < static_cast<int>(y + alto))
		return true;
	return false;
}

Raton& Raton::operator = (const Raton &r)
{
	if(this != &r)
	{
		m_posicion_x = r.m_posicion_x;
		m_posicion_y = r.m_posicion_y;
		m_desplazamiento_x = r.m_desplazamiento_x;
		m_desplazamiento_y = r.m_desplazamiento_y;
		m_boton_izquierdo = r.m_boton_izquierdo;
		m_boton_central = r.m_boton_central;
		m_boton_derecho = r.m_boton_derecho;
		m_numero_clics = r.m_numero_clics;
	}
	return *this;
}
