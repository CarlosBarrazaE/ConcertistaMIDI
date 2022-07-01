#include "fila.h++"
Fila::Fila(float x, float y, float ancho, float alto, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), m_color_fondo(0.95f, 0.95f, 0.95f)
{
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_sobre_boton = false;
	m_boton_pre_activado = false;
	m_boton_seleccionado = false;

	m_color_actual = m_color_fondo;
	m_color_seleccion = Color(m_color_fondo.rojo()-0.08f, m_color_fondo.verde()-0.08f, m_color_fondo.azul()-0.08f);
}

Fila::~Fila()
{
	for(Etiqueta* e : m_celda)
		delete e;
}

void Fila::actualizar(unsigned int /*diferencia_tiempo*/)
{
	if(m_boton_seleccionado)
		m_color_actual = m_color_seleccion;
	else
		m_color_actual = m_color_fondo;

	if(m_sobre_boton)
		m_color_actual = Color(m_color_actual.rojo()-0.08f, m_color_actual.verde()-0.08f, m_color_actual.azul()-0.08f);
}

void Fila::dibujar()
{
	//Dibuja el color de fondo
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(this->x(), this->y(), this->ancho(), this->alto(), m_color_actual);

	//Dibuja cada una de las celdas
	for(unsigned int x=0; x<m_celda.size(); x++)
		m_celda.at(x)->dibujar();

	//Dibula el borde de abajo
	m_rectangulo->dibujar(this->x(), this->y()+this->alto()-1, this->ancho(), 1, Color(0.9f, 0.9f, 0.9f));
}

void Fila::evento_raton(Raton *raton)
{
	if(raton->esta_sobre(this->x(), this->y(), this->ancho(), this->alto()))
	{
		if(raton->activado(BotonIzquierdo) && m_sobre_boton)
			m_boton_pre_activado = true;
		else if(!raton->activado(BotonIzquierdo))
		{
			m_sobre_boton = true;
			if(m_boton_pre_activado)
			{
				m_boton_seleccionado = true;
				m_boton_pre_activado = false;
			}
		}
	}
	else
	{
		m_sobre_boton = false;
		m_boton_pre_activado = false;
	}
}

void Fila::posicion(float x, float y)
{
	float desplazado = x - this->x();
	this->_posicion(x, y);
	for(Etiqueta* e : m_celda)
		e->posicion(e->x()+desplazado, this->y());
}

void Fila::agregar_celda(Etiqueta *celda)
{
	m_celda.push_back(celda);
}

std::vector<Etiqueta *> *Fila::celdas()
{
	return &m_celda;
}

bool Fila::esta_seleccionado()
{
	return m_boton_seleccionado;
}

void Fila::seleccionar()
{
	m_boton_seleccionado = true;
}

void Fila::deseleccionar()
{
	m_boton_seleccionado = false;
}
