#include "elemento.h++"

Elemento::Elemento(int x, int y, int ancho, int alto)
{
	this->x = x;
	this->y = y;
	this->ancho = ancho;
	this->alto = alto;
	this->centrado = false;
	this->dx = 0;
	this->dy = 0;
}

Elemento::Elemento(int x, int y, int ancho, int alto, bool centrado)
{
	if(centrado)
		this->x = x - (ancho / 2);
	else
		this->x = x;

	this->y = y;
	this->ancho = ancho;
	this->alto = alto;
	this->centrado = centrado;
	this->dx = 0;
	this->dy = 0;
}

int Elemento::posicion_x()
{
	return this->x;
}

int Elemento::posicion_y()
{
	return this->y;
}

int Elemento::o_ancho()
{
	return this->ancho;
}

int Elemento::o_alto()
{
	return this->alto;
}

void Elemento::posicion_x(int x)
{
	if(this->centrado)
		this->x = x - (this->ancho / 2);
	else
		this->x = x;
}

void Elemento::posicion_y(int y)
{
	this->y = y;
}

void Elemento::e_ancho(int ancho)
{
	this->ancho = ancho;
}

void Elemento::e_alto(int alto)
{
	this->alto = alto;
}

void Elemento::ajuste(int dx, int dy)
{
	this->dx = dx;
	this->dy = dy;
}

void Elemento::ajuste_x(int dx)
{
	this->dx = dx;
}

void Elemento::ajuste_y(int dy)
{
	this->dy = dy;
}