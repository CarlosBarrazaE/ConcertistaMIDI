#include "configuracion_dispositivo.h++"

Configuracion_Dispositivo::Configuracion_Dispositivo(float x, float y, float ancho, Dispositivo_Midi *dispositivo, Administrador_Recursos *recursos) : Elemento(x, y, ancho, 40), m_desplegar(x+10, y+10, 20, 20, "", recursos), m_nombre(recursos), m_habilitado(x+ancho-40, y+5, 30, 30, "", recursos)
{
	m_dispositivo = dispositivo;
	m_recursos = recursos;
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_flecha_abajo = recursos->textura(T_FlechaAbajo);
	m_flecha_arriba = recursos->textura(T_FlechaArriba);
	m_desplegar.textura(m_flecha_abajo);

	std::string t_cliente = std::to_string(static_cast<unsigned int>(dispositivo->cliente()));
	std::string t_puerto = std::to_string(static_cast<unsigned int>(dispositivo->puerto()));

	m_nombre.posicion(x+40, y+10);
	m_nombre.dimension(x+ancho-40, 20);
	m_nombre.centrado_vertical(true);
	m_nombre.texto(dispositivo->nombre() + " - Puerto " + t_puerto);
	m_nombre.tipografia(recursos->tipografia(LetraMediana));

	if(dispositivo->habilitado())
		m_habilitado.estado(true);

	m_mostrar_configuracion = false;
	m_elementos_creados = false;
	m_alto_minimo = this->alto();
	m_alto_maximo = 360.0f;
	m_direccion = 0;

	m_cambio_altura = false;

	m_mostrar_linea = false;
}

Configuracion_Dispositivo::~Configuracion_Dispositivo()
{
	if(m_elementos_creados)
	{
		delete m_texto_tipo_dispositivo;
		delete m_opciones_tipo_dispositivo;

		if(m_dispositivo->es_entrada())
		{
			//Entrada
			delete m_texto_entrada;
			delete m_texto_sensitivo;
			delete m_texto_volumen_entrada;
			delete m_texto_rango;
			delete m_sensitivo;
			delete m_volumen_entrada;
			delete m_rango_teclado;

			//Salida
			delete m_texto_salida;
			delete m_texto_volumen_salida;
			delete m_texto_teclado_luminoso;
			delete m_volumen_salida;
			delete m_teclado_luminoso;
		}
	}
}

void Configuracion_Dispositivo::crear_nuevos_elementos()
{
	m_texto_tipo_dispositivo = new Etiqueta(this->x()+10, this->y()+50, 200, 20, false, "Modo Dispositivo:", LetraMediana, m_recursos);
	m_texto_tipo_dispositivo->centrado_vertical(true);

	m_opciones_tipo_dispositivo = new Lista_Opciones(this->x()+200, this->y()+50, this->ancho()-220, 20, true, m_recursos);
	m_opciones_tipo_dispositivo->tipografia(m_recursos->tipografia(LetraMediana));

	std::vector<std::string> opciones_tipo;
	unsigned long int numero_opciones = 0;
	unsigned long int opcion_seleccionada = 0;

	if(m_dispositivo->es_entrada())
	{
		opciones_tipo.push_back("Entrada");
		numero_opciones++;
	}
	if(m_dispositivo->es_salida())
	{
		opciones_tipo.push_back("Salida");
		if(!m_dispositivo->entrada_activa() && m_dispositivo->salida_activa())
			opcion_seleccionada = numero_opciones;
		numero_opciones++;
	}
	if(m_dispositivo->es_entrada() && m_dispositivo->es_salida())
	{
		opciones_tipo.push_back("Entrada y Salida");
		if(m_dispositivo->entrada_activa() && m_dispositivo->salida_activa())
			opcion_seleccionada = numero_opciones;
	}

	m_opciones_tipo_dispositivo->opciones_textos(opciones_tipo);
	m_opciones_tipo_dispositivo->opcion_predeterminada(opcion_seleccionada);

	m_texto_entrada = new Etiqueta(this->x()+10, this->y()+90, this->ancho()-20, 20, true, "Entrada", LetraMediana, m_recursos);
	m_texto_sensitivo = new Etiqueta(this->x()+10, this->y()+130, 200, 20, false, "Sensitivo:", LetraMediana, m_recursos);
	m_texto_volumen_entrada = new Etiqueta(this->x()+10, this->y()+170, 200, 20, false, "Volumen:", LetraMediana, m_recursos);
	m_texto_rango = new Etiqueta(this->x()+10, this->y()+210, 200, 20, false, "Tamaño del Teclado:", LetraMediana, m_recursos);

	m_texto_entrada->centrado_vertical(true);
	m_texto_sensitivo->centrado_vertical(true);
	m_texto_volumen_entrada->centrado_vertical(true);
	m_texto_rango->centrado_vertical(true);

	m_sensitivo = new Casilla_Verificacion(this->x()+200, this->y()+125, 30, 30, "", m_recursos);
	m_volumen_entrada = new Control_Deslizante(this->x()+200, this->y()+170, this->ancho()-220, 20, m_recursos);
	m_rango_teclado = new Lista_Opciones(this->x()+200, this->y()+210, this->ancho()-220, 20, true, m_recursos);
	m_rango_teclado->tipografia(m_recursos->tipografia(LetraMediana));
	std::vector<std::string> opciones_teclado;
	opciones_teclado.push_back("Teclado (24 teclas)");
	opciones_teclado.push_back("Organo de 37 teclas");
	opciones_teclado.push_back("Organo de 49 teclas");
	opciones_teclado.push_back("Organo de 61 teclas");
	opciones_teclado.push_back("Organo de 76 teclas");
	opciones_teclado.push_back("Organo de 88 teclas");
	m_rango_teclado->opciones_textos(opciones_teclado);

	m_texto_salida = new Etiqueta(this->x()+10, this->y()+250, this->ancho()-20, 20, true, "Salida", LetraMediana, m_recursos);
	m_texto_volumen_salida = new Etiqueta(this->x()+10, this->y()+290, 200, 20, false, "Volumen:", LetraMediana, m_recursos);
	m_texto_teclado_luminoso = new Etiqueta(this->x()+10, this->y()+330, 200, 20, false, "Teclado Luminoso:", LetraMediana, m_recursos);

	m_texto_salida->centrado_vertical(true);
	m_texto_volumen_salida->centrado_vertical(true);
	m_texto_teclado_luminoso->centrado_vertical(true);

	m_volumen_salida = new Control_Deslizante(this->x()+200, this->y()+290, this->ancho()-220, 20, m_recursos);
	m_teclado_luminoso = new Lista_Opciones(this->x()+200, this->y()+330, this->ancho()-220, 20, true, m_recursos);
	m_teclado_luminoso->tipografia(m_recursos->tipografia(LetraMediana));
	m_teclado_luminoso->opciones_textos(Teclas_Luminosas::Lista);
}

void Configuracion_Dispositivo::actualizar(unsigned int diferencia_tiempo)
{
	//Crea los elementos que estaban ocultos hasta ahora
	if(m_mostrar_configuracion && !m_elementos_creados)
	{
		m_elementos_creados = true;
		this->crear_nuevos_elementos();
	}

	if(m_direccion > 0)
	{
		if(this->alto() < m_alto_maximo)
		{
			float alto_nuevo = this->alto() + (10.0f * (static_cast<float>(diferencia_tiempo)/((1.0f/60.0f)*1000000000.0f)));
			if(alto_nuevo > m_alto_maximo)
				alto_nuevo = m_alto_maximo;
			this->dimension(this->ancho(), alto_nuevo);
			m_cambio_altura = true;
		}
		else
			m_direccion = 0;
	}
	else if(m_direccion < 0)
	{
		if(this->alto() > m_alto_minimo)
		{
			float alto_nuevo = this->alto() - (10.0f * (static_cast<float>(diferencia_tiempo)/((1.0f/60.0f)*1000000000.0f)));
			if(alto_nuevo < m_alto_minimo)
				alto_nuevo = m_alto_minimo;
			this->dimension(this->ancho(), alto_nuevo);
			m_cambio_altura = true;
		}
		else
			m_direccion = 0;
	}
}

void Configuracion_Dispositivo::dibujar()
{
	m_rectangulo->textura(false);
	m_rectangulo->color(Color(1.0f, 1.0f, 1.0f));
	m_rectangulo->dibujar(this->x(), this->y(), this->ancho(), this->alto());
	m_desplegar.dibujar();
	m_nombre.dibujar();
	m_habilitado.dibujar();

	if(m_mostrar_configuracion && m_elementos_creados)
	{
		m_rectangulo->textura(false);
		m_rectangulo->dibujar(this->x(), this->y()+40, this->ancho(), 1.0f, Color(0.9f, 0.9f, 0.9f));
		m_rectangulo->dibujar(this->x(), this->y()+80, this->ancho(), 40.0f, Color(0.9f, 0.9f, 0.9f));
		//if(m_mostrar_linea)
		m_rectangulo->dibujar(this->x(), this->y()+240, this->ancho(), 40.0f, Color(0.9f, 0.9f, 0.9f));

		m_texto_tipo_dispositivo->dibujar();
		m_opciones_tipo_dispositivo->dibujar();
		//m_rectangulo->textura(false);
		//m_rectangulo->dibujar(m_texto_tipo_dispositivo->x(), m_texto_tipo_dispositivo->y(), m_texto_tipo_dispositivo->ancho(), m_texto_tipo_dispositivo->alto(), Color(0.3f, 0.4f, 0.4f, 0.4f));

		//Entrada
		m_texto_entrada->dibujar();
		m_texto_sensitivo->dibujar();
		m_texto_volumen_entrada->dibujar();
		m_texto_rango->dibujar();
		m_sensitivo->dibujar();
		m_volumen_entrada->dibujar();
		m_rango_teclado->dibujar();

		//Salida
		m_texto_salida->dibujar();
		m_texto_volumen_salida->dibujar();
		m_texto_teclado_luminoso->dibujar();
		m_volumen_salida->dibujar();
		m_teclado_luminoso->dibujar();
	}
}

void Configuracion_Dispositivo::evento_raton(Raton *raton)
{
	m_desplegar.evento_raton(raton);
	m_habilitado.evento_raton(raton);

	if(m_desplegar.esta_activado())
	{
		m_mostrar_configuracion = !m_mostrar_configuracion;

		if(m_mostrar_configuracion)
		{
			m_desplegar.textura(m_flecha_arriba);
			m_direccion = 1;
		}
		else
		{
			m_desplegar.textura(m_flecha_abajo);
			m_direccion = -1;
		}
	}
	if(m_mostrar_configuracion && m_elementos_creados)
	{
		m_opciones_tipo_dispositivo->evento_raton(raton);
		m_sensitivo->evento_raton(raton);
		m_volumen_entrada->evento_raton(raton);
		m_rango_teclado->evento_raton(raton);
		m_volumen_salida->evento_raton(raton);
		m_teclado_luminoso->evento_raton(raton);
	}
}

void Configuracion_Dispositivo::posicion(float x, float y)
{
	m_desplegar.posicion(x+10, y+10);
	m_nombre.posicion(x+40, y+13);
	m_habilitado.posicion(x+this->ancho()-40, y+5);
	this->_posicion(x, y);
}
/*
void Configuracion_Dispositivo::dimension(float ancho, float alto)
{
	Registro::Aviso("Cambio de dimension");
	m_habilitado.posicion(m_habilitado.x(), ancho - 10);
	this->_dimension(ancho, alto);
}*/

bool Configuracion_Dispositivo::cambio_altura()
{
	bool estado = m_cambio_altura;
	m_cambio_altura = false;
	return estado;
}