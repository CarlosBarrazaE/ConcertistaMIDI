#include "control_deslizante.h++"

Control_Deslizante::Control_Deslizante(float x, float y, float ancho, float alto, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), m_texto_valor(recursos)
{
	m_recursos = recursos;
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_textura_fondo = recursos->textura(T_ControlDeslizante_Fondo);
	m_textura_relleno = recursos->textura(T_ControlDeslizante_Relleno);
	m_textura_boton = recursos->textura(T_ControlDeslizante_Boton);

	m_texto_valor.centrado_vertical(true);
	m_texto_valor.tipografia(recursos->tipografia(LetraChica));
	m_texto_valor.texto("100%");
	m_texto_valor.dimension(m_texto_valor.largo_texto(), alto);
	m_largo_maximo_texto = m_texto_valor.largo_texto();
	m_texto_valor.posicion(x+ancho - m_largo_maximo_texto, y);

	m_centro_vertical = y + alto/2;
	m_largo_barra = ancho - (m_largo_maximo_texto + 20);

	m_valor_minimo = 0.0;
	m_valor_maximo = 1.5;
	m_valor_paso = 0.05;
	m_valor_actual = 1.0;
	m_cambio_valor = false;
	m_moviendo_pre_activado = false;
	m_moviendo_barra = false;
	m_posicion_agarre = 0.0f;

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
	//10px es el alto del control deslizante
	//40x42 es el tamaño del boton incluyendo la sombra escalado
	m_rectangulo->textura(true);
	m_rectangulo->extremos_fijos(true, false);
	m_textura_fondo->activar();
	m_rectangulo->color(Color(1.0f, 1.0f, 1.0f));
	m_rectangulo->dibujar_estirable(this->x(), m_centro_vertical-5, m_largo_barra, 10, 20, 0);
	m_textura_relleno->activar();
	m_rectangulo->dibujar_estirable(this->x(), m_centro_vertical-5, m_largo_barra_color, 10, 20, 0);
	m_rectangulo->extremos_fijos(false, false);

	m_textura_boton->activar();
	m_rectangulo->dibujar(this->x()+m_largo_barra_color-20, m_centro_vertical-20, 40, 42);
	m_texto_valor.dibujar();
}

void Control_Deslizante::evento_raton(Raton *raton)
{
	//-3 y -4 porque se descarta la sompra por ese motivo el area solo es de 34x34
	bool sobre_boton = raton->esta_sobre(this->x()+m_largo_barra_color-(20-3), m_centro_vertical-(20-4), 34, 34);
	if(sobre_boton || raton->esta_sobre(this->x(), m_centro_vertical-5, m_largo_barra, 10))
	{
		if(m_moviendo_pre_activado && raton->activado(BotonIzquierdo))
		{
			if(sobre_boton && !m_moviendo_barra)
				m_posicion_agarre = static_cast<float>(raton->x()) - (this->x() + m_largo_barra_color);
			m_moviendo_barra = true;
		}
		else if(!raton->activado(BotonIzquierdo))
			m_moviendo_pre_activado = true;

		if(!m_moviendo_barra)
		{
			if(raton->dy() > 0)
			{
				this->cambiar_valor(m_valor_actual + m_valor_paso);
				raton->anular_desplazamiento();//Anula el evento de desplazamiento una vez utilizado
			}
			if(raton->dy() < 0)
			{
				this->cambiar_valor(m_valor_actual - m_valor_paso);
				raton->anular_desplazamiento();//Anula el evento de desplazamiento una vez utilizado
			}
		}
	}
	else
		m_moviendo_pre_activado = false;

	if(!raton->activado(BotonIzquierdo))
	{
		m_moviendo_barra = false;
		m_posicion_agarre = 0.0f;
	}

	if(m_moviendo_barra)
	{
		float posicion_x = static_cast<float>(raton->x()) - (this->x() + m_posicion_agarre);
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

void Control_Deslizante::posicion(float x, float y)
{
	this->_posicion(x, y);
	m_centro_vertical = this->y() + this->alto()/2;
	m_texto_valor.posicion(this->x()+this->ancho() - m_largo_maximo_texto, this->y());
}

void Control_Deslizante::dimension(float ancho, float alto)
{
	this->_dimension(ancho, alto);
	m_centro_vertical = this->y() + this->alto()/2;
	m_largo_barra = this->ancho() - (m_largo_maximo_texto+20);
	m_texto_valor.dimension(this->x()+this->ancho() - m_largo_maximo_texto, this->alto());
	this->calcular_valores_nuevos();
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
