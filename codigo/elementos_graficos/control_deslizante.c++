#include "control_deslizante.h++"

Control_Deslizante::Control_Deslizante(float x, float y, float ancho, float alto, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), m_texto_valor(recursos)
{
	m_recursos = recursos;
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_texto_valor.dimension(10, alto);
	m_texto_valor.centrado_vertical(true);
	m_texto_valor.tipografia(recursos->tipografia(LetraChica));
	m_texto_valor.texto("100%");
	m_texto_valor.posicion(x+ancho - m_texto_valor.largo_texto(), y);

	m_largo_barra = ancho - (m_texto_valor.largo_texto() + 10);

	m_valor_minimo = 0.0;
	m_valor_maximo = 1.5;
	m_valor_paso = 0.05;
	m_valor_actual = 1.0;
	m_cambio_valor = false;
	m_moviendo_pre_activado = false;
	m_moviendo_barra = false;

	this->calcular_valores_nuevos();
}

Control_Deslizante::~Control_Deslizante()
{
}

void Control_Deslizante::calcular_valores_nuevos()
{
	double largo_total = m_valor_maximo - m_valor_minimo;
	double posicion_actual = m_valor_actual - m_valor_minimo;
	double porcentaje = posicion_actual / largo_total;

	m_largo_barra_color = m_largo_barra * static_cast<float>(porcentaje);

	//Actualiza el texto
	std::string valor = std::to_string(m_valor_actual*100);
	m_texto_valor.texto(valor.substr(0, valor.find(".")) + "%");
	m_texto_valor.posicion(this->x()+this->ancho() - m_texto_valor.largo_texto(), this->y());
}

void Control_Deslizante::actualizar(unsigned int /*diferencia_tiempo*/)
{

}

void Control_Deslizante::dibujar()
{
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(this->x(), this->y(), m_largo_barra, this->alto(), Color(0.9f, 0.9f, 0.9f));
	m_rectangulo->dibujar(this->x(), this->y(), m_largo_barra_color, this->alto(), Color(0.0f, 0.5f, 1.0f));
	m_texto_valor.dibujar();
}

void Control_Deslizante::evento_raton(Raton *raton)
{
	if(raton->esta_sobre(this->x(), this->y(), m_largo_barra, this->alto()))
	{
		if(m_moviendo_pre_activado && raton->activado(BotonIzquierdo))
			m_moviendo_barra = true;
		else if(!raton->activado(BotonIzquierdo))
			m_moviendo_pre_activado = true;

		if(!m_moviendo_barra)
		{
			if(raton->dy() > 0)
				this->cambiar_valor(m_valor_actual + m_valor_paso) ;
			if(raton->dy() < 0)
				this->cambiar_valor(m_valor_actual - m_valor_paso) ;
		}
	}
	else
		m_moviendo_pre_activado = false;

	if(!raton->activado(BotonIzquierdo))
		m_moviendo_barra = false;

	if(m_moviendo_barra)
	{
		float posicion_x = static_cast<float>(raton->x()) - this->x();
		if(posicion_x > m_largo_barra)
			this->cambiar_valor(m_valor_maximo);
		else if(posicion_x < 0)
			this->cambiar_valor(m_valor_minimo);
		else
		{
			double rango_total = m_valor_maximo - m_valor_minimo;
			double porcentaje = static_cast<double>(posicion_x) / static_cast<double>(m_largo_barra);
			this->cambiar_valor(m_valor_minimo + (rango_total * porcentaje));
		}
	}
}

void Control_Deslizante::posicion(float /*x*/, float /*y*/)
{
}

void Control_Deslizante::dimension(float /*ancho*/, float /*alto*/)
{
}

void Control_Deslizante::valor_minimo(double valor)
{
	m_valor_minimo = valor;
	if(m_valor_actual < m_valor_minimo)
		m_valor_actual = m_valor_minimo;

	this->calcular_valores_nuevos();
}

void Control_Deslizante::valor_maximo(double valor)
{
	m_valor_maximo = valor;
	if(m_valor_actual > m_valor_maximo)
		m_valor_actual = m_valor_maximo;

	this->calcular_valores_nuevos();
}

void Control_Deslizante::cambiar_valor(double valor)
{
	if(valor > m_valor_maximo)
	{
		if(!Funciones::comparar_double(m_valor_actual, m_valor_maximo, 0.001))
			m_cambio_valor = true;
		m_valor_actual = m_valor_maximo;
	}
	else if(valor < m_valor_minimo)
	{
		if(!Funciones::comparar_double(m_valor_actual, m_valor_minimo, 0.001))
			m_cambio_valor = true;
		m_valor_actual = m_valor_minimo;
	}
	else
	{
		if(!Funciones::comparar_double(m_valor_actual, valor, 0.001))
			m_cambio_valor = true;
		m_valor_actual = valor;
	}

	if(m_cambio_valor)
	{
		this->calcular_valores_nuevos();
	}
}

void Control_Deslizante::valor_paso(double valor)
{
	m_valor_paso = valor;
}

double Control_Deslizante::valor_actual()
{
	return m_valor_actual;
}

bool Control_Deslizante::cambio_valor()
{
	bool valor = m_cambio_valor;
	m_cambio_valor = false;
	return valor;
}
