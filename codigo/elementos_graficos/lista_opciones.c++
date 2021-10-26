#include "lista_opciones.h++"

Lista_Opciones::Lista_Opciones(float x, float y, float ancho, float alto, bool mostrar_flechas, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), m_texto_seleccion(recursos)
{
	m_rectangulo = recursos->figura(F_Rectangulo);
	m_opcion_actual = 0;
	m_ancho_icono = 10;
	m_alto_icono = 10;
	m_centrado_icono = (ancho - m_ancho_icono)/2;
	m_cambio_opcion_seleccionada = false;
	m_mostrar_flechas = mostrar_flechas;
	if(m_mostrar_flechas)
	{
		float ancho_boton = this->alto();
		m_flecha_izquierda = new Boton(this->x(), this->y(), ancho_boton, this->alto(), "", recursos);
		m_flecha_izquierda->color_boton(Color(1.0f, 1.0f, 1.0f));
		m_flecha_izquierda->textura(recursos->textura(T_FlechaIzquierda));

		m_flecha_derecha = new Boton(this->x()+this->ancho()-ancho_boton, this->y(), ancho_boton, this->alto(), "", recursos);
		m_flecha_derecha->color_boton(Color(1.0f, 1.0f, 1.0f));
		m_flecha_derecha->textura(recursos->textura(T_FlechaDerecha));
	}

	m_texto_seleccion.tipografia(recursos->tipografia(LetraChica));
	m_texto_seleccion.color(Color(0.0f, 0.0f, 0.0f));
	m_texto_seleccion.posicion(x, y);
	m_texto_seleccion.dimension(ancho, alto);
	m_texto_seleccion.centrado(true);

	Color color(1.0f, 1.0f, 1.0f);
	m_color_normal = color;
	m_color_actual = color;
	m_color_sobre = color - 0.1f;
	m_color_activado = color - 0.15f;
	m_direccion = NoCambia;
}

Lista_Opciones::~Lista_Opciones()
{
	if(m_mostrar_flechas)
	{
		delete m_flecha_izquierda;
		delete m_flecha_derecha;
	}
}

void Lista_Opciones::actualizar(unsigned int /*diferencia_tiempo*/)
{
}

void Lista_Opciones::dibujar()
{
	m_rectangulo->textura(true);
	if(m_usar_iconos)
	{
		//Dibuja el icono si existe
		m_iconos[m_opcion_actual]->activar();
		m_rectangulo->dibujar(this->x()+m_centrado_icono, this->y(), m_ancho_icono, m_alto_icono, m_color_actual);
	}
	if(m_mostrar_flechas && m_opciones.size() > 1)
	{
		m_flecha_izquierda->dibujar();
		m_flecha_derecha->dibujar();
	}
	//Dibuja el texto
	m_texto_seleccion.dibujar();
}

void Lista_Opciones::evento_raton(Raton *raton)
{
	//Si no hay opciones o solo una se omite
	if(m_opciones.size() <= 1)
		return;

	if(m_mostrar_flechas)
	{
		m_flecha_izquierda->evento_raton(raton);
		m_flecha_derecha->evento_raton(raton);

		if(m_flecha_izquierda->esta_activado())
			m_direccion = Anterior;
		else if(m_flecha_derecha->esta_activado())
			m_direccion = Siguiente;
	}
	else
	{
		if(raton->esta_sobre(this->x(), this->y(), this->ancho(), this->alto()))
		{
			m_color_actual = m_color_sobre;
			if((raton->activado(BotonIzquierdo) || raton->activado(BotonDerecho) || raton->activado(BotonCentral)) && m_sobre_boton)
			{
					m_boton_pre_activado = raton->boton_activado();
					m_color_actual = m_color_activado;
			}
			else if(!raton->activado(BotonIzquierdo) && !raton->activado(BotonCentral) && !raton->activado(BotonDerecho))
			{
				m_sobre_boton = true;
				if(!m_mostrar_flechas)
				{
					//Cambio solo con los distintos botones
					if(m_boton_pre_activado == BotonIzquierdo)
					{
						m_direccion = Siguiente;
						m_boton_pre_activado = Ninguno;
					}
					else if(m_boton_pre_activado == BotonCentral)
					{
						m_direccion = Inicial;
						m_boton_pre_activado = Ninguno;
					}
					else if(m_boton_pre_activado == BotonDerecho)
					{
						m_direccion = Anterior;
						m_boton_pre_activado = Ninguno;
					}
				}
			}
		}
		else
		{
			m_sobre_boton = false;
			m_boton_pre_activado = Ninguno;
			m_direccion = NoCambia;
			m_color_actual = m_color_normal;
		}
	}

	if(m_direccion == Siguiente)
	{
		m_opcion_actual++;
		if(m_opcion_actual >= m_opciones.size())
			m_opcion_actual = 0;
		m_texto_seleccion.texto(m_opciones[m_opcion_actual]);
		m_cambio_opcion_seleccionada = true;
		m_direccion = NoCambia;
	}
	else if(m_direccion == Inicial)
	{
		m_opcion_actual = 0;
		m_texto_seleccion.texto(m_opciones[m_opcion_actual]);
		m_cambio_opcion_seleccionada = true;
		m_direccion = NoCambia;
	}
	else if(m_direccion == Anterior)
	{
		if(m_opcion_actual == 0)
			m_opcion_actual = m_opciones.size()-1;
		else
			m_opcion_actual--;
		m_texto_seleccion.texto(m_opciones[m_opcion_actual]);
		m_cambio_opcion_seleccionada = true;
		m_direccion = NoCambia;
	}
}

void Lista_Opciones::posicion(float x, float y)
{
	this->_posicion(x, y);
	if(m_usar_iconos)
		m_texto_seleccion.posicion(x, y+43);
	else
		m_texto_seleccion.posicion(x, y);

	if(m_mostrar_flechas)
	{
		float ancho_boton = this->alto();
		m_flecha_izquierda->posicion(this->x(), this->y());
		m_flecha_derecha->posicion(this->x()+this->ancho()-ancho_boton, this->y());
	}
}

void Lista_Opciones::dimension(float ancho, float alto)
{
	this->_dimension(ancho, alto);
	float ancho_boton = this->alto();
	if(m_mostrar_flechas && ancho > ancho_boton*2 + m_texto_seleccion.largo_texto() && alto > 0)
	{
		m_flecha_derecha->posicion(this->x()+this->ancho()-ancho_boton, this->y());
		m_texto_seleccion.dimension(ancho, alto);
	}
}

void Lista_Opciones::dimension_icono(float ancho, float alto)
{
	m_ancho_icono = ancho;
	m_alto_icono = alto;
	m_centrado_icono = (m_ancho - m_ancho_icono)/2;
}

void Lista_Opciones::opciones_textos(std::vector<std::string> opciones)
{
	m_opciones = opciones;
	m_texto_seleccion.texto(m_opciones[m_opcion_actual]);
}

void Lista_Opciones::opciones_iconos(std::vector<Textura2D*> iconos)
{
	m_iconos = iconos;
	m_usar_iconos = true;
	m_texto_seleccion.dimension(m_texto_seleccion.ancho(), 0);//Para anular centrado vertical
	m_texto_seleccion.posicion(0, 43);//Posicion para iconos
}

void Lista_Opciones::tipografia(Tipografia *tipografia)
{
	m_texto_seleccion.tipografia(tipografia);
}

void Lista_Opciones::color_texto(const Color &color)
{
	m_texto_seleccion.color(color);
}

void Lista_Opciones::opcion_predeterminada(unsigned long int opcion)
{
	if(opcion < m_opciones.size())
	{
		m_opcion_actual = opcion;
		m_texto_seleccion.texto(m_opciones[m_opcion_actual]);
	}
}

unsigned long int Lista_Opciones::opcion_seleccionada()
{
	return m_opcion_actual;
}

bool Lista_Opciones::cambio_opcion_seleccionada()
{
	bool estado = m_cambio_opcion_seleccionada;
	m_cambio_opcion_seleccionada = false;
	return estado;
}
