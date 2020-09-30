#include "boton.h++"

Boton::Boton(float x, float y, float ancho, float alto, std::string texto, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), m_texto(recursos)
{
	m_rectangulo = recursos->figura(F_Rectangulo);
	m_textura_boton = recursos->textura(T_Boton);
	m_color_texto = Color(0.0f, 0.0f, 0.0f);

	this->inicializar();

	m_texto.texto(texto);
	m_texto.tipografia(recursos->tipografia(LetraMediana));
	m_texto.posicion(this->x(), this->y());
	if(m_texto.largo_texto()+20 > ancho)
	{
		//El ancho del texto mas 10 pixeles por lado
		m_texto.dimension(m_texto.largo_texto()+20, alto);
		this->dimension(m_texto.largo_texto()+20, alto);
	}
	else
		m_texto.dimension(ancho, alto);
	m_texto.centrado(true);
	m_texto.color(m_color_texto);

	m_habilitado = true;
}

Boton::Boton(float x, float y, float ancho, float alto, std::string texto, ModeloLetra modelo_letra, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), m_texto(recursos)
{
	m_rectangulo = recursos->figura(F_Rectangulo);
	m_textura_boton = recursos->textura(T_Boton);
	m_color_texto = Color(0.0f, 0.0f, 0.0f);

	this->inicializar();

	m_texto.texto(texto);
	m_texto.tipografia(recursos->tipografia(modelo_letra));
	m_texto.posicion(this->x(), this->y());
	if(m_texto.largo_texto()+20 > ancho)
	{
		//El ancho del texto mas 10 pixeles por lado
		m_texto.dimension(m_texto.largo_texto()+20, alto);
		this->dimension(m_texto.largo_texto()+20, alto);
	}
	else
		m_texto.dimension(ancho, alto);
	m_texto.centrado(true);
	m_texto.color(m_color_texto);

	m_habilitado = true;
}

Boton::~Boton()
{
}

void Boton::inicializar()
{
	m_sobre_boton = false;
	m_boton_pre_activado = false;
	m_boton_activado = false;

	Color color(1.0f, 1.0f, 1.0f);
	m_color_boton_normal = color;
	m_color_boton_actual = color;
	m_color_boton_sobre = color - 0.1f;
}

void Boton::actualizar(unsigned int /*diferencia_tiempo*/)
{
}

void Boton::dibujar()
{
	//Dibuja el fondo
	m_textura_boton->activar();
	m_rectangulo->textura(true);
	m_rectangulo->dibujar(this->x(), this->y(), this->ancho(), this->alto(), m_color_boton_actual);

	//Dibuja el texto
	m_texto.dibujar();
}

void Boton::evento_raton(Raton *raton)
{
	if(!m_habilitado)
		return;
	if(raton->esta_sobre(this->x(), this->y(), this->ancho(), this->alto()))
	{
		if(raton->activado(BotonIzquierdo) && m_sobre_boton)
			m_boton_pre_activado = true;
		else if(!raton->activado(BotonIzquierdo))
		{
			m_color_boton_actual = m_color_boton_sobre;
			m_sobre_boton = true;
			if(m_boton_pre_activado)
			{
				m_boton_activado = true;
				m_boton_pre_activado = false;
			}
		}
	}
	else
	{
		m_color_boton_actual = m_color_boton_normal;
		m_sobre_boton = false;
		m_boton_pre_activado = false;
		m_boton_activado = false;
	}
}

void Boton::posicion(float x, float y)
{
	this->_posicion(x, y);
	m_texto.posicion(this->x(), this->y());
}

void Boton::centrado(bool centrado)
{
	this->_centrado(centrado);
	m_texto.posicion(this->x(), this->y());
}

bool Boton::esta_activado()
{
	bool estado = m_boton_activado;
	m_boton_activado = false;
	return estado;
}

void Boton::textura(Textura2D *textura)
{
	m_textura_boton = textura;
}

void Boton::color_boton(Color color)
{
	m_color_boton_normal = color;
	m_color_boton_actual = color;
	m_color_boton_sobre = color - 0.1f;
}

void Boton::color_texto(Color color)
{
	m_texto.color(color);
}

void Boton::tipografia(Tipografia *tipografia)
{
	m_texto.tipografia(tipografia);
}

void Boton::habilitado(bool estado)
{
	m_habilitado = estado;
	if(m_habilitado)
		m_color_texto = m_color_texto - 0.5f;
	else
	{
		m_color_texto = m_color_texto + 0.5f;
		m_color_boton_actual = m_color_boton_normal;
	}

	m_texto.color(m_color_texto);
}
