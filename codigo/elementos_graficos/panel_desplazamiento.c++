#include "panel_desplazamiento.h++"
#include "../registro.h++"

Panel_Desplazamiento::Panel_Desplazamiento(float x, float y, float ancho, float alto, float fila, float margen_fila, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto)
{
	m_columna = 0;
	m_margen_columna = 0;
	m_fila = fila;
	m_margen_fila = margen_fila;

	this->inicializar(recursos);
}

Panel_Desplazamiento::Panel_Desplazamiento(float x, float y, float ancho, float alto, float columna, float fila, float margen_columna, float margen_fila, Administrador_Recursos *recursos)
: Elemento(x, y, ancho, alto)
{
	m_columna = columna;
	m_fila = fila;
	m_margen_columna = margen_columna;
	m_margen_fila = margen_fila;

	this->inicializar(recursos);
}

Panel_Desplazamiento::~Panel_Desplazamiento()
{
}

void Panel_Desplazamiento::inicializar(Administrador_Recursos *recursos)
{
	m_alto_actual = 0;
	m_desplazamiento_x = 0;
	m_desplazamiento_y = 0;

	m_calcular_posicion = true;

	m_rectangulo = recursos->figura(F_Rectangulo);
	m_barra = recursos->textura(T_Barra);

	m_sobre_barra = false;
	m_boton_activado = false;
	m_enviar_evento = false;
	m_proporcion = 0;

	//Un evento de raton ficticio con una posicion muy lejana
	m_raton_ficticio.actualizar_posicion(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
}

void Panel_Desplazamiento::actualizar(unsigned int diferencia_tiempo)
{
	if(m_calcular_posicion)
		this->actualizar_dimension();
	for(unsigned int i=0; i<m_elementos.size(); i++)
	{
		m_elementos[i]->actualizar(diferencia_tiempo);
	}

	//0.05 de opacidad por fotograma
	if(m_enviar_evento && m_animacion < 1)
		m_animacion += (static_cast<float>(diferencia_tiempo)/1000000000.0f) * 3;
	else if(!m_enviar_evento && m_animacion > 0)
		m_animacion -= (static_cast<float>(diferencia_tiempo)/1000000000.0f) * 3;
}

void Panel_Desplazamiento::dibujar()
{

	glScissor(static_cast<int>(this->x()), static_cast<int>(Pantalla::Alto-this->y()-this->alto()), static_cast<int>(this->ancho()), static_cast<int>(this->alto()));
	glEnable(GL_SCISSOR_TEST);
	Elemento *e;
	for(unsigned int i=0; i<m_elementos.size(); i++)
	{
		e = m_elementos.at(i);
		if(e->y() + e->alto() > this->y() && e->y() < this->y() + this->alto() &&
			e->x() + e->ancho() > this->x() && e->x() < this->x() + this->ancho())
		{
			e->dibujar();
		}
	}

	if(this->alto() < m_alto_actual)
	{
		m_barra->activar();
		m_rectangulo->extremos_fijos(false, true);
		m_rectangulo->textura(true);
		m_rectangulo->color(Color(0.7f, 0.7f, 0.7f, m_animacion));
		m_rectangulo->dibujar_estirable(this->x()+this->ancho()-10, this->y()+10, 10, this->alto()-20, 0, 10);
		m_rectangulo->color(Color(0.5f, 0.5f, 0.5f, m_animacion));
		m_rectangulo->dibujar_estirable(this->x()+this->ancho()-10, this->y()+10-m_desplazamiento_y*m_proporcion, 10, this->alto() * m_proporcion, 0, 10);
		m_rectangulo->extremos_fijos(false, false);
	}
	glDisable(GL_SCISSOR_TEST);
}

void Panel_Desplazamiento::evento_raton(Raton *raton)
{
	//Solo si el raton esta dentro del espacio del componente se envian el evento
	Raton *evento_raton;
	if(raton->esta_sobre(this->x(), this->y(), this->ancho(), this->alto()))
	{
		evento_raton = raton;
		m_enviar_evento = true;
	}
	else
		evento_raton = &m_raton_ficticio;

	//Eventos fuera del area no se envian
	if(!m_enviar_evento)
		return;

	//Se envia un evento de raton ficticio solo una vez para desmarcar los componentes
	if(evento_raton == &m_raton_ficticio)
		m_enviar_evento = false;

	float desplazamiento_nuevo_y = 0;
	float desplazamiento_anterior_y = 0;
	if(this->alto() < m_alto_actual)
	{
		desplazamiento_anterior_y = m_desplazamiento_y;
		if(evento_raton->dy() != 0)
		{
			//Desplazamiento con ruedita
			m_desplazamiento_y += static_cast<float>(evento_raton->dy()*20);
		}
		else if(raton->esta_sobre(this->x()+this->ancho()-10, this->y()+10, this->ancho(), this->alto()-20))
		{
			//Desplazamiento con clic en la barra
			if(evento_raton->activado(BotonIzquierdo) && m_sobre_barra)
			{
				m_boton_activado = true;
				//El inicio de la barra esta en this->y() + 20, el centro de la barra desplazable esta en (this->alto() * m_proporcion) / 2)
				m_desplazamiento_y = -(static_cast<float>(evento_raton->y()) - (this->y() + 20 + (this->alto() * m_proporcion) / 2)) / m_proporcion;
			}
			else if(!evento_raton->activado(BotonIzquierdo))
				m_sobre_barra = true;
		}
		else if(m_boton_activado)//Desplazamiento arrastrando la barra
			m_desplazamiento_y = -(static_cast<float>(evento_raton->y()) - (this->y() + 20 + (this->alto() * m_proporcion) / 2)) / m_proporcion;
		else
			m_sobre_barra = false;

		if(!evento_raton->activado(BotonIzquierdo) && m_boton_activado)
			m_boton_activado = false;

		if(m_desplazamiento_y > 0)
				m_desplazamiento_y = 0;
		else if(m_desplazamiento_y < this->alto() - m_alto_actual)
			m_desplazamiento_y = this->alto() - m_alto_actual;

		desplazamiento_nuevo_y = m_desplazamiento_y - desplazamiento_anterior_y;
	}

	for(unsigned int i=0; i<m_elementos.size(); i++)
	{
		if(this->alto() < m_alto_actual)
			m_elementos[i]->posicion(m_elementos[i]->x(), m_elementos[i]->y() + desplazamiento_nuevo_y);
		m_elementos[i]->evento_raton(evento_raton);
	}
}

void Panel_Desplazamiento::posicion(float x, float y)
{
	this->_posicion(x, y);
	m_calcular_posicion = true;
}

void Panel_Desplazamiento::dimension(float ancho, float alto)
{
	this->_dimension(ancho, alto);
	m_calcular_posicion = true;
}

void Panel_Desplazamiento::desplazar_y(int dy)
{
	float nuevo_desplazamiento = m_desplazamiento_y + static_cast<float>(dy);
	if(nuevo_desplazamiento > 0)
		nuevo_desplazamiento = 0;
	else if(nuevo_desplazamiento < this->alto() - m_alto_actual)
		nuevo_desplazamiento = this->alto() - m_alto_actual;

	for(unsigned int i=0; i<m_elementos.size(); i++)
	{
		if(this->alto() < m_alto_actual)
			m_elementos[i]->posicion(m_elementos[i]->x(), m_elementos[i]->y() + nuevo_desplazamiento - m_desplazamiento_y);
	}
	m_desplazamiento_y = nuevo_desplazamiento;
}

void Panel_Desplazamiento::agregar_elemento(Elemento *e)
{
	m_elementos.push_back(e);
}

void Panel_Desplazamiento::vaciar()
{
	m_elementos.clear();
	m_calcular_posicion = true;
}

void Panel_Desplazamiento::actualizar_dimension()
{
	int numero_columnas = 1;
	float x_inicio = this->x();
	//Si m_columna es mayor a cero, puede existir mas de una columna y es centrado
	if(m_columna > 0)
	{
		numero_columnas = static_cast<int>(this->ancho() / (m_columna + m_margen_columna));
		float ancho_actual = (static_cast<float>(numero_columnas) * m_columna) + (static_cast<float>(numero_columnas - 1) * m_margen_columna);
		x_inicio = (this->ancho() - ancho_actual) / 2 + this->x();
	}
	float x_actual = x_inicio;
	float y_actual = this->y();
	int contador_columnas = 1;
	for(unsigned int i=0; i<m_elementos.size(); i++)
	{
		m_elementos[i]->posicion(x_actual, y_actual);

		if(contador_columnas < numero_columnas)
		{
			x_actual += m_columna + m_margen_columna;
		}
		else
		{
			contador_columnas = 0;
			x_actual = x_inicio;
			if(i<m_elementos.size()-1)
				y_actual += m_fila + m_margen_fila;
		}
		contador_columnas++;
	}

	m_alto_actual = y_actual + m_fila - this->y();
	m_desplazamiento_y = 0;

	m_calcular_posicion = false;
	if(this->alto() < m_alto_actual)
		m_proporcion = (this->alto()-20) / m_alto_actual;
}
