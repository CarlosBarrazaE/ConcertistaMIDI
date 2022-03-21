#include "panel_desplazamiento.h++"
#include "../registro.h++"

Panel_Desplazamiento::Panel_Desplazamiento(float x, float y, float ancho, float alto, float margen_fila, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto)
{
	//Fila Unica
	m_columna = 0;
	m_margen_columna = 0;
	m_fila = 0;
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
	m_alto_actual = 0.0f;
	m_desplazamiento_x = 0.0f;
	m_desplazamiento_y = 0.0f;

	m_calcular_posicion = true;

	m_rectangulo = recursos->figura(F_Rectangulo);
	m_barra = recursos->textura(T_Barra);

	m_sobre_barra = false;
	m_boton_activado = false;
	m_adentro_panel = false;
	m_proporcion = 0.0f;
	m_porcentaje_agarre_raton = 0.0f;
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
	if(m_adentro_panel && m_animacion < 1)
		m_animacion += (static_cast<float>(diferencia_tiempo)/1000000000.0f) * 3;
	else if(!m_adentro_panel && m_animacion > 0)
		m_animacion -= (static_cast<float>(diferencia_tiempo)/1000000000.0f) * 3;
}

void Panel_Desplazamiento::dibujar()
{
	glScissor(static_cast<int>(this->x()), static_cast<int>(Pantalla::Alto-this->y()-this->alto()), static_cast<int>(this->ancho()), static_cast<int>(this->alto()));
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
	glScissor(0, 0, static_cast<int>(Pantalla::Ancho), static_cast<int>(Pantalla::Alto));
}

void Panel_Desplazamiento::evento_raton(Raton *raton)
{
	//Solo si el raton esta dentro del espacio del componente se envian el evento
	Raton *raton_actual;
	if(raton->esta_sobre(this->x(), this->y(), this->ancho(), this->alto()))
	{
		m_adentro_panel = true;
		raton_actual = raton;
	}
	else
	{
		m_adentro_panel = false;
		m_raton_modificado = *raton;
		int nueva_x = 0, nueva_y = 0;
		if(raton->x() < static_cast<int>(this->x()))
			nueva_x = -1000;
		else if(raton->x() > static_cast<int>(this->x() + this->ancho()))
			nueva_x = static_cast<int>(Pantalla::Ancho) + 1000;
		else
			nueva_x = raton->x();

		if(raton->y() < static_cast<int>(this->y()))
			nueva_y = -1000;
		else if(raton->y() > static_cast<int>(this->y() + this->alto()))
			nueva_y = static_cast<int>(Pantalla::Alto) + 1000;
		else
			nueva_y = raton->y();

		//Cuando sale, mueve la posicion lejos para no seleccionar ningun elemento por fuera del panel
		m_raton_modificado.actualizar_posicion(nueva_x, nueva_y);
		raton_actual = &m_raton_modificado;
	}

	float desplazamiento_nuevo_y = 0;
	float desplazamiento_anterior_y = 0;
	if(this->alto() < m_alto_actual)
	{
		desplazamiento_anterior_y = m_desplazamiento_y;
		if(raton_actual->dy() != 0 && !m_boton_activado)
		{
			//Desplazamiento con ruedita
			m_desplazamiento_y += static_cast<float>(raton_actual->dy()*20);
		}
		else if(raton_actual->esta_sobre(this->x()+this->ancho()-10, this->y()+10, this->ancho(), this->alto()-20))
		{
			//Desplazamiento con clic en la barra
			if(raton_actual->activado(BotonIzquierdo) && m_sobre_barra && !m_boton_activado)
			{
				m_boton_activado = true;
				//El inicio de la barra desplazable esta en this->y() + 10 si el desplazamiento es 0
				float inicio_barra = this->y()+10-m_desplazamiento_y*m_proporcion;//De la barra desplazable
				float largo_barra = this->alto() * m_proporcion;//De la barra desplazable
				float fin_barra = inicio_barra + largo_barra;//De la barra desplazable

				//0% agarro la barra desde arriba y 100% agarró la barra desde abajo
				if(raton->y() >= static_cast<int>(inicio_barra) && raton->y() <= static_cast<int>(fin_barra))
					m_porcentaje_agarre_raton = (static_cast<float>(raton->y()) - inicio_barra) / largo_barra;
				else
					m_porcentaje_agarre_raton = 0.5f;//Desde el centro porque hizo clic fuera de la barra desplazable

				float posicion_agarre_raton = largo_barra * m_porcentaje_agarre_raton;
				m_desplazamiento_y = -(static_cast<float>(raton_actual->y()) - (this->y() + 10 + posicion_agarre_raton)) / m_proporcion;
			}
			else if(!raton_actual->activado(BotonIzquierdo))
				m_sobre_barra = true;
		}
		else
			m_sobre_barra = false;

		if(m_boton_activado)//Desplazamiento arrastrando la barra
		{
			float largo_barra = this->alto() * m_proporcion;
			float posicion_agarre_raton = largo_barra * m_porcentaje_agarre_raton;
			m_desplazamiento_y = -(static_cast<float>(raton_actual->y()) - (this->y() + 10 + posicion_agarre_raton)) / m_proporcion;
		}

		if(!raton_actual->activado(BotonIzquierdo) && m_boton_activado)
			m_boton_activado = false;

		if(m_desplazamiento_y > 0)
				m_desplazamiento_y = 0;
		else if(m_desplazamiento_y < this->alto() - m_alto_actual)
			m_desplazamiento_y = this->alto() - m_alto_actual;

		desplazamiento_nuevo_y = m_desplazamiento_y - desplazamiento_anterior_y;
	}

	Elemento *e;
	for(unsigned int i=0; i<m_elementos.size(); i++)
	{
		e = m_elementos[i];
		//Desplaza los elementos si es necesario
		if(this->alto() < m_alto_actual)
			e->posicion(e->x(), e->y() + desplazamiento_nuevo_y);

		if(e->y() + e->alto() > this->y() && e->y() < this->y() + this->alto() &&
			e->x() + e->ancho() > this->x() && e->x() < this->x() + this->ancho())
		{
			e->evento_raton(raton_actual);
		}
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
	float ultimo_alto = 0;
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
			{
				//Si fila es 0 entonces el alto depende de cada elemento
				if(Funciones::comparar_float(m_fila ,0.0f, 0.1f))
					y_actual += m_elementos[i]->alto() + m_margen_fila;
				else
					y_actual += m_fila + m_margen_fila;
			}
			else
			{
				//El alto del ultimo elemento, no se agrega el margen
				ultimo_alto = m_elementos[i]->alto();
			}
		}
		contador_columnas++;
	}

	//Si fila es 0 entonces el alto depende de cada elemento
	if(Funciones::comparar_float(m_fila ,0.0f, 0.1f))
		m_alto_actual = y_actual + ultimo_alto - this->y();
	else
		m_alto_actual = y_actual + m_fila - this->y();
	m_desplazamiento_y = 0;

	m_calcular_posicion = false;
	if(this->alto() < m_alto_actual)
		m_proporcion = (this->alto()-20) / m_alto_actual;
}
