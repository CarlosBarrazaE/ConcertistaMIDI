#include "configuracion_pista.h++"

Configuracion_Pista::Configuracion_Pista(float x, float y, float ancho, float alto, Pista pista, Administrador_Recursos *recursos)
: Elemento(x, y, ancho, alto), m_texto_instrumento(recursos), m_texto_datos(recursos), m_texto_sonido(recursos), m_seleccion_modo(20, 85, 70, 55, false, recursos), m_seleccion_color(137, 85, 70, 55, false, recursos), m_datos_pista(pista)
{
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_texto_instrumento.texto(m_datos_pista.instrumento());
	m_texto_instrumento.tipografia(recursos->tipografia(LetraMediana));
	m_texto_instrumento.color(Color(1.0f, 1.0f, 1.0f));
	m_texto_instrumento.posicion(20, 15);

	m_texto_datos.texto(std::to_string(m_datos_pista.numero_notas()) + " notas - Canal " + std::to_string(static_cast<unsigned int>(m_datos_pista.canal())));
	m_texto_datos.tipografia(recursos->tipografia(LetraMuyChica));
	m_texto_datos.color(Color(1.0f, 1.0f, 1.0f));
	m_texto_datos.posicion(20, 35);

	m_texto_sonido.tipografia(recursos->tipografia(LetraMuyChica));
	m_texto_sonido.color(Color(1.0f, 1.0f, 1.0f));
	m_texto_sonido.dimension(100, 8);
	m_texto_sonido.centrado(true);
	m_texto_sonido.posicion(242, 128);

	m_textura_fondo = recursos->textura(T_ConfiguracionPista);

	m_textura_reproducir = recursos->textura(T_Reproducir);
	m_textura_pausar = recursos->textura(T_Pausar);

	m_textura_sonido_activado = recursos->textura(T_SonidoActivado);
	m_textura_sonido_desactivado = recursos->textura(T_SonidoDesactivado);

	std::vector<std::string> opcion_modo;
	opcion_modo.push_back("Fondo");
	opcion_modo.push_back("Tocar");
	opcion_modo.push_back("Aprender");

	std::vector<std::string> opcion_color;
	opcion_color.push_back("Invisible");
	opcion_color.push_back("Verde");
	opcion_color.push_back("Azul");
	opcion_color.push_back("Rosado");
	opcion_color.push_back("Naranja");
	opcion_color.push_back("Verde Claro");
	opcion_color.push_back("Celeste");
	opcion_color.push_back("Durazno");
	opcion_color.push_back("Amarillo");
	opcion_color.push_back("Lila");

	std::vector<Textura2D*> icono_modos;
	icono_modos.push_back(recursos->textura(T_MusicaFondo));
	icono_modos.push_back(recursos->textura(T_Tocar));
	icono_modos.push_back(recursos->textura(T_Aprender));

	std::vector<Textura2D*> icono_color;
	icono_color.push_back(recursos->textura(T_Color_Invisible));
	icono_color.push_back(recursos->textura(T_Color_1));
	icono_color.push_back(recursos->textura(T_Color_2));
	icono_color.push_back(recursos->textura(T_Color_3));
	icono_color.push_back(recursos->textura(T_Color_4));
	icono_color.push_back(recursos->textura(T_Color_5));
	icono_color.push_back(recursos->textura(T_Color_6));
	icono_color.push_back(recursos->textura(T_Color_7));
	icono_color.push_back(recursos->textura(T_Color_8));
	icono_color.push_back(recursos->textura(T_Color_9));

	m_seleccion_modo.dimension_icono(40, 40);
	m_seleccion_modo.opciones_textos(opcion_modo);
	m_seleccion_modo.opciones_iconos(icono_modos);
	m_seleccion_modo.color_texto(Color(1.0f, 1.0f, 1.0f));
	m_seleccion_modo.tipografia(recursos->tipografia(LetraMuyChica));

	m_seleccion_color.dimension_icono(40, 40);
	m_seleccion_color.opciones_textos(opcion_color);
	m_seleccion_color.opciones_iconos(icono_color);
	m_seleccion_color.color_texto(Color(1.0f, 1.0f, 1.0f));
	m_seleccion_color.tipografia(recursos->tipografia(LetraMuyChica));
	m_seleccion_color.opcion_predeterminada(1);

	m_vista_previa = new Boton(300, 22, 30, 30, "", recursos);
	m_vista_previa->color_boton(Color(1.0f, 1.0f, 1.0f));
	m_vista_previa->textura(m_textura_reproducir);

	m_boton_sonido = new Boton(272, 87, 40, 40, "", recursos);
	m_boton_sonido->color_boton(Color(1.0f, 1.0f, 1.0f));

	//Se lee la configuracion del color
	for(unsigned int i=0; i<=NUMERO_COLORES_PISTA; i++)
	{
		//Se busca el color de la pista
		if(Pista::Colores_pista[i] == m_datos_pista.color())
		{
			m_seleccion_color.opcion_predeterminada(i);
			i = NUMERO_COLORES_PISTA;//Termina el ciclo
		}
	}

	//Se lee la configuracion del modo
	if(m_datos_pista.modo() == Fondo)
		m_seleccion_modo.opcion_predeterminada(0);
	else if(m_datos_pista.modo() == Tocar)
		m_seleccion_modo.opcion_predeterminada(1);
	else if(m_datos_pista.modo() == Aprender)
		m_seleccion_modo.opcion_predeterminada(2);

	//Se lee la configuracion del sonido
	if(m_datos_pista.sonido())
	{
		m_boton_sonido->textura(m_textura_sonido_activado);
		m_texto_sonido.texto("Sonido activado");
	}
	else
	{
		m_boton_sonido->textura(m_textura_sonido_desactivado);
		m_texto_sonido.texto("Sin sonido");
	}

	m_estado_vista_previa = false;
}

Configuracion_Pista::~Configuracion_Pista()
{
	delete m_vista_previa;
	delete m_boton_sonido;
}

void Configuracion_Pista::actualizar(unsigned int diferencia_tiempo)
{
	m_seleccion_modo.actualizar(diferencia_tiempo);
	m_seleccion_color.actualizar(diferencia_tiempo);
	m_vista_previa->actualizar(diferencia_tiempo);
	m_boton_sonido->actualizar(diferencia_tiempo);
}

void Configuracion_Pista::dibujar()
{
	//Dibuja el fondo
	m_textura_fondo->activar();
	m_rectangulo->textura(true);
	m_rectangulo->dibujar(this->x(), this->y(), this->ancho(), this->alto(), m_datos_pista.color());
	//m_rectangulo->dibujar(this->x(), this->y()+128, this->ancho(), 8, Color(0.8f, 0.2f, 0.7f)); //Para alinear el texto

	//Dibuja los componentes
	m_vista_previa->dibujar();
	m_boton_sonido->dibujar();

	m_seleccion_modo.dibujar();
	m_seleccion_color.dibujar();

	m_texto_instrumento.dibujar();
	m_texto_datos.dibujar();
	m_texto_sonido.dibujar();
}

void Configuracion_Pista::evento_raton(Raton *raton)
{
	m_seleccion_modo.evento_raton(raton);
	m_seleccion_color.evento_raton(raton);
	m_vista_previa->evento_raton(raton);
	m_boton_sonido->evento_raton(raton);

	if(m_seleccion_modo.cambio_opcion_seleccionada())
	{
		unsigned long int modo_seleccionado = m_seleccion_modo.opcion_seleccionada();
		if(modo_seleccionado == 0)
			m_datos_pista.modo(Tocar);
		else if(modo_seleccionado == 1)
			m_datos_pista.modo(Aprender);
		else if(modo_seleccionado == 2)
			m_datos_pista.modo(Fondo);
	}

	if(m_seleccion_color.cambio_opcion_seleccionada())
	{
		m_datos_pista.color(Pista::Colores_pista[m_seleccion_color.opcion_seleccionada()]);

		if(m_seleccion_color.opcion_seleccionada() == NUMERO_COLORES_PISTA)
			m_datos_pista.visible(false);
		else
			m_datos_pista.visible(true);
	}

	if(m_vista_previa->esta_activado())
	{
		m_estado_vista_previa = !m_estado_vista_previa;
		if(m_estado_vista_previa)
			m_vista_previa->textura(m_textura_pausar);
		else
			m_vista_previa->textura(m_textura_reproducir);
	}

	if(m_boton_sonido->esta_activado())
	{

		bool estado = m_datos_pista.sonido();
		estado = !estado;
		m_datos_pista.sonido(estado);
		if(estado)
		{
			m_texto_sonido.texto("Sonido activado");
			m_boton_sonido->textura(m_textura_sonido_activado);
		}
		else
		{
			m_texto_sonido.texto("Sin sonido");
			m_boton_sonido->textura(m_textura_sonido_desactivado);
		}
	}
}

void Configuracion_Pista::posicion(float x, float y)
{
	this->_posicion(x, y);
	//20, 85, 70, 55, recursos), m_seleccion_color(137, 85,
	m_seleccion_modo.posicion(x+20, y+85);
	m_seleccion_color.posicion(x+137, y+85);
	m_vista_previa->posicion(x+300, y+22);
	m_boton_sonido->posicion(x+272, y+87);
	m_texto_instrumento.posicion(x+20, y+15);
	m_texto_datos.posicion(x+20, y+35);
	m_texto_sonido.posicion(x+242, y+128);
}

Pista Configuracion_Pista::pista()
{
	return m_datos_pista;
}
