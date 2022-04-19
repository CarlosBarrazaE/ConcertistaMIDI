#include "configuracion_dispositivo.h++"

Configuracion_Dispositivo::Configuracion_Dispositivo(float x, float y, float ancho, Dispositivo_Midi dispositivo, Administrador_Recursos *recursos) : Elemento(x, y, ancho, 40), m_desplegar(x+10, y+10, 20, 20, "", recursos), m_nombre(recursos), m_habilitado(x+ancho-40, y+5, 30, 30, "", recursos)
{
	m_datos_dispositivo = dispositivo;
	m_recursos = recursos;
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_flecha_abajo = recursos->textura(T_FlechaAbajo);
	m_flecha_arriba = recursos->textura(T_FlechaArriba);
	m_desplegar.textura(m_flecha_abajo);

	std::string t_cliente = std::to_string(static_cast<unsigned int>(dispositivo.cliente()));
	std::string t_puerto = std::to_string(static_cast<unsigned int>(dispositivo.puerto()));

	m_nombre.posicion(x+40, y+10);
	m_nombre.dimension(x+ancho-40, 20);
	m_nombre.centrado_vertical(true);
	m_nombre.texto(dispositivo.nombre() + " - Puerto " + t_cliente + ":" + t_puerto);
	m_nombre.tipografia(recursos->tipografia(LetraMediana));

	if(dispositivo.habilitado())
		m_habilitado.estado(true);

	m_mostrar_configuracion = false;
	m_elementos_creados = false;
	m_cambio_altura = false;
	m_alto_minimo = this->alto();
	m_alto_maximo = this->alto();
	m_alto_nuevo = 0;
	m_direccion = 0;

	m_dispositivo_cambiado = false;
	m_cambio_estado_conexion = false;
}

Configuracion_Dispositivo::~Configuracion_Dispositivo()
{
	if(m_elementos_creados)
	{
		delete m_texto_tipo_dispositivo;
		delete m_opciones_tipo_dispositivo;

		if(m_datos_dispositivo.es_entrada())
		{
			//Entrada
			delete m_texto_entrada;
			delete m_texto_sensitivo;
			delete m_texto_volumen_entrada;
			delete m_texto_rango;
			delete m_sensitivo;
			delete m_volumen_entrada;
			delete m_rango_teclado;
		}

		if(m_datos_dispositivo.es_salida())
		{
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

	m_opciones_tipo_dispositivo = new Lista_Opciones(this->x()+220, this->y()+50, this->ancho()-240, 20, true, m_recursos);
	m_opciones_tipo_dispositivo->tipografia(m_recursos->tipografia(LetraMediana));

	std::vector<std::string> opciones_tipo;

	if(m_datos_dispositivo.es_entrada())
		opciones_tipo.push_back("Entrada");
	if(m_datos_dispositivo.es_salida())
		opciones_tipo.push_back("Salida");
	if(m_datos_dispositivo.es_entrada() && m_datos_dispositivo.es_salida())
		opciones_tipo.push_back("Entrada y Salida");

	m_opciones_tipo_dispositivo->opciones_textos(opciones_tipo);

	std::string texto_predeterminado;

	if(m_datos_dispositivo.entrada_activa() && m_datos_dispositivo.salida_activa())
		texto_predeterminado = "Entrada y Salida";
	else if(m_datos_dispositivo.entrada_activa())
		texto_predeterminado = "Entrada";
	else if(m_datos_dispositivo.salida_activa())
		texto_predeterminado = "Salida";

	m_opciones_tipo_dispositivo->opcion_predeterminada(texto_predeterminado);

	m_alto_maximo = m_alto_minimo + 40;
	if(m_datos_dispositivo.es_entrada())
	{
		m_texto_entrada = new Etiqueta(this->x()+10, this->y()+90, this->ancho()-20, 20, true, "Entrada", LetraMediana, m_recursos);
		m_texto_sensitivo = new Etiqueta(this->x()+10, this->y()+130, 200, 20, false, "Sensitivo:", LetraMediana, m_recursos);
		m_texto_volumen_entrada = new Etiqueta(this->x()+10, this->y()+170, 200, 20, false, "Volumen:", LetraMediana, m_recursos);
		m_texto_rango = new Etiqueta(this->x()+10, this->y()+210, 200, 20, false, "Tamaño del Teclado:", LetraMediana, m_recursos);

		m_texto_entrada->centrado_vertical(true);
		m_texto_sensitivo->centrado_vertical(true);
		m_texto_volumen_entrada->centrado_vertical(true);
		m_texto_rango->centrado_vertical(true);

		m_sensitivo = new Casilla_Verificacion(this->x()+220, this->y()+125, 30, 30, "", m_recursos);
		m_volumen_entrada = new Control_Deslizante(this->x()+220, this->y()+170, this->ancho()-240, 20, m_recursos);
		m_rango_teclado = new Lista_Opciones(this->x()+220, this->y()+210, this->ancho()-240, 20, true, m_recursos);
		m_rango_teclado->tipografia(m_recursos->tipografia(LetraMediana));
		std::vector<std::string> opciones_teclado;
		opciones_teclado.push_back("Teclado (24 teclas)");
		opciones_teclado.push_back("Organo de 37 teclas");
		opciones_teclado.push_back("Organo de 49 teclas");
		opciones_teclado.push_back("Organo de 61 teclas");
		opciones_teclado.push_back("Organo de 76 teclas");
		opciones_teclado.push_back("Organo de 88 teclas");
		m_rango_teclado->opciones_textos(opciones_teclado);

		m_sensitivo->estado(m_datos_dispositivo.sensitivo());
		m_volumen_entrada->cambiar_valor(m_datos_dispositivo.volumen_entrada());
		Rango_Organo rango_teclado = m_datos_dispositivo.rango_teclado();

		if(rango_teclado.numero_teclas() == 24)
			m_rango_teclado->opcion_predeterminada(0);
		else if(rango_teclado.numero_teclas() == 37)
			m_rango_teclado->opcion_predeterminada(1);
		else if(rango_teclado.numero_teclas() == 49)
			m_rango_teclado->opcion_predeterminada(2);
		else if(rango_teclado.numero_teclas() == 61)
			m_rango_teclado->opcion_predeterminada(3);
		else if(rango_teclado.numero_teclas() == 76)
			m_rango_teclado->opcion_predeterminada(4);
		else if(rango_teclado.numero_teclas() == 88)
			m_rango_teclado->opcion_predeterminada(5);

		if(this->mostrar_entrada())
			m_alto_maximo += 160;
	}

	if(m_datos_dispositivo.es_salida())
	{
		float desplazado = 0;
		if(this->mostrar_entrada())
			desplazado = 160;

		m_texto_salida = new Etiqueta(this->x()+10, this->y()+desplazado+90, this->ancho()-20, 20, true, "Salida", LetraMediana, m_recursos);
		m_texto_volumen_salida = new Etiqueta(this->x()+10, this->y()+desplazado+130, 200, 20, false, "Volumen:", LetraMediana, m_recursos);
		m_texto_teclado_luminoso = new Etiqueta(this->x()+10, this->y()+desplazado+170, 200, 20, false, "Teclado Luminoso:", LetraMediana, m_recursos);

		m_texto_salida->centrado_vertical(true);
		m_texto_volumen_salida->centrado_vertical(true);
		m_texto_teclado_luminoso->centrado_vertical(true);

		m_volumen_salida = new Control_Deslizante(this->x()+220, this->y()+desplazado+130, this->ancho()-240, 20, m_recursos);
		m_teclado_luminoso = new Lista_Opciones(this->x()+220, this->y()+desplazado+170, this->ancho()-240, 20, true, m_recursos);
		m_teclado_luminoso->tipografia(m_recursos->tipografia(LetraMediana));
		m_teclado_luminoso->opciones_textos(Teclas_Luminosas::Lista);

		m_volumen_salida->cambiar_valor(m_datos_dispositivo.volumen_salida());
		if(m_datos_dispositivo.id_teclas_luminosas() < Teclas_Luminosas::Lista.size())
			m_teclado_luminoso->opcion_predeterminada(m_datos_dispositivo.id_teclas_luminosas());

		if(this->mostrar_salida())
			m_alto_maximo += 120;
	}
}

bool Configuracion_Dispositivo::mostrar_entrada()
{
	if(!m_mostrar_configuracion && this->alto() <= 40)
		return false;

	if(m_datos_dispositivo.entrada_activa())
		return true;
	return false;
}

bool Configuracion_Dispositivo::mostrar_salida()
{
	if(!m_mostrar_configuracion && this->alto() <= 40)
		return false;

	if(m_datos_dispositivo.salida_activa())
		return true;
	return false;
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
			this->_dimension(this->ancho(), alto_nuevo);
			m_cambio_altura = true;
		}
		else
			m_direccion = 0;
	}
	else if(m_direccion < 0)
	{
		if(m_direccion == -1)//Ocultar todo
		{
			if(this->alto() > m_alto_minimo)
			{
				float alto_nuevo = this->alto() - (10.0f * (static_cast<float>(diferencia_tiempo)/((1.0f/60.0f)*1000000000.0f)));
				if(alto_nuevo < m_alto_minimo)
					alto_nuevo = m_alto_minimo;
				this->_dimension(this->ancho(), alto_nuevo);
				m_cambio_altura = true;
			}
			else
				m_direccion = 0;
		}
		else if(m_direccion == -2)//Reducir tamaño al nuevo alto maximo
		{
			if(this->alto() > m_alto_maximo)
			{
				float alto_nuevo = this->alto() - (10.0f * (static_cast<float>(diferencia_tiempo)/((1.0f/60.0f)*1000000000.0f)));
				if(alto_nuevo < m_alto_maximo)
					alto_nuevo = m_alto_maximo;
				this->_dimension(this->ancho(), alto_nuevo);
				m_cambio_altura = true;
			}
			else
				m_direccion = 0;
		}
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

	if((m_mostrar_configuracion || this->y() > 40) && m_elementos_creados)
	{
		//Recorta solo la parte que se desplega y deja un margen de 10px abajo
		m_recursos->recortar_pantalla(this->x(), this->y()+40, this->ancho(), this->alto()-50);
		m_rectangulo->textura(false);
		m_rectangulo->dibujar(this->x(), this->y()+40, this->ancho(), 1.0f, Color(0.9f, 0.9f, 0.9f));
		m_rectangulo->dibujar(this->x(), this->y()+80, this->ancho(), 40.0f, Color(0.9f, 0.9f, 0.9f));
		if(m_datos_dispositivo.es_entrada() && m_datos_dispositivo.es_salida() && m_opciones_tipo_dispositivo->opcion_seleccionada() == 2)
			m_rectangulo->dibujar(this->x(), this->y()+240, this->ancho(), 40.0f, Color(0.9f, 0.9f, 0.9f));

		m_texto_tipo_dispositivo->dibujar();
		m_opciones_tipo_dispositivo->dibujar();

		if(this->mostrar_entrada())
		{
			//Entrada
			m_texto_entrada->dibujar();
			m_texto_sensitivo->dibujar();
			m_texto_volumen_entrada->dibujar();
			m_texto_rango->dibujar();
			m_sensitivo->dibujar();
			m_volumen_entrada->dibujar();
			m_rango_teclado->dibujar();
		}

		if(this->mostrar_salida())
		{
			//Salida
			m_texto_salida->dibujar();
			m_texto_volumen_salida->dibujar();
			m_texto_teclado_luminoso->dibujar();
			m_volumen_salida->dibujar();
			m_teclado_luminoso->dibujar();
		}
		m_recursos->revertir_recorte();
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
	if(m_habilitado.cambio_estado())
	{
		m_datos_dispositivo.habilitado(m_habilitado.activado());
		m_dispositivo_cambiado = true;
		m_cambio_estado_conexion = true;
	}

	//Eventos de elementos si fueron desplegados
	if(m_mostrar_configuracion && m_elementos_creados)
	{
		m_opciones_tipo_dispositivo->evento_raton(raton);
		if(m_opciones_tipo_dispositivo->cambio_opcion_seleccionada())
		{
			if(m_opciones_tipo_dispositivo->opcion_seleccionada_texto() == "Entrada")
				m_datos_dispositivo.capacidad_activa(ENTRADA);
			else if(m_opciones_tipo_dispositivo->opcion_seleccionada_texto() == "Salida")
				m_datos_dispositivo.capacidad_activa(SALIDA);
			else if(m_opciones_tipo_dispositivo->opcion_seleccionada_texto() == "Entrada y Salida")
				m_datos_dispositivo.capacidad_activa(ENTRADA | SALIDA);

			m_dispositivo_cambiado = true;

			m_alto_nuevo = m_alto_minimo + 40;
			if(this->mostrar_entrada())
				m_alto_nuevo += 160;
			if(this->mostrar_salida())
				m_alto_nuevo += 120;

			if(m_alto_nuevo > this->alto())//Aumenta Tamaño
			{
				m_direccion = 1;
				m_alto_maximo = m_alto_nuevo;
			}
			else if (m_alto_nuevo < this->alto())//Reduce Tamaño
			{
				m_direccion = -2;//Mover hasta el nuevo m_alto_maximo
				m_alto_maximo = m_alto_nuevo;
			}
		}

		if(this->mostrar_entrada())
		{
			m_sensitivo->evento_raton(raton);
			if(m_sensitivo->cambio_estado())
			{
				m_datos_dispositivo.sensitivo(m_sensitivo->activado());
				m_dispositivo_cambiado = true;
			}

			m_volumen_entrada->evento_raton(raton);
			if(m_volumen_entrada->cambio_valor())
			{
				m_datos_dispositivo.volumen_entrada(m_volumen_entrada->valor_actual());
				m_dispositivo_cambiado = true;
			}

			m_rango_teclado->evento_raton(raton);
			if(m_rango_teclado->cambio_opcion_seleccionada())
			{
				if(m_rango_teclado->opcion_seleccionada() == 0)
					m_datos_dispositivo.rango_teclado("48,24");
				else if(m_rango_teclado->opcion_seleccionada() == 1)
					m_datos_dispositivo.rango_teclado("48,37");
				else if(m_rango_teclado->opcion_seleccionada() == 2)
					m_datos_dispositivo.rango_teclado("36,49");
				else if(m_rango_teclado->opcion_seleccionada() == 3)
					m_datos_dispositivo.rango_teclado("36,61");
				else if(m_rango_teclado->opcion_seleccionada() == 4)
					m_datos_dispositivo.rango_teclado("28,76");
				else if(m_rango_teclado->opcion_seleccionada() == 5)
					m_datos_dispositivo.rango_teclado("21,88");

				m_dispositivo_cambiado = true;
			}
		}

		if(this->mostrar_salida())
		{
			m_volumen_salida->evento_raton(raton);
			if(m_volumen_salida->cambio_valor())
			{
				m_datos_dispositivo.volumen_salida(m_volumen_salida->valor_actual());
				m_dispositivo_cambiado = true;
			}

			m_teclado_luminoso->evento_raton(raton);
			if(m_teclado_luminoso->cambio_opcion_seleccionada())
			{
				m_datos_dispositivo.teclas_luminosas(static_cast<unsigned int>(m_teclado_luminoso->opcion_seleccionada()));
				m_dispositivo_cambiado = true;
			}
		}
	}
}

void Configuracion_Dispositivo::posicion(float x, float y)
{
	this->_posicion(x, y);
	m_desplegar.posicion(this->x()+10, this->y()+10);
	m_nombre.posicion(this->x()+40, this->y()+10);
	m_habilitado.posicion(this->x()+this->ancho()-40, this->y()+5);

	if(m_elementos_creados)
	{
		m_texto_tipo_dispositivo->posicion(this->x()+10, this->y()+50);
		m_opciones_tipo_dispositivo->posicion(this->x()+220, this->y()+50);

		if(this->mostrar_entrada())
		{
			//Entrada
			m_texto_entrada->posicion(this->x()+10, this->y()+90);
			m_texto_sensitivo->posicion(this->x()+10, this->y()+130);
			m_texto_volumen_entrada->posicion(this->x()+10, this->y()+170);
			m_texto_rango->posicion(this->x()+10, this->y()+210);
			m_sensitivo->posicion(this->x()+220, this->y()+125);
			m_volumen_entrada->posicion(this->x()+220, this->y()+170);
			m_rango_teclado->posicion(this->x()+220, this->y()+210);
		}

		if(this->mostrar_salida())
		{
			float desplazado = 0;
			if(this->mostrar_entrada())
				desplazado = 160;

			//Salida
			m_texto_salida->posicion(this->x()+10, this->y()+desplazado+90);
			m_texto_volumen_salida->posicion(this->x()+10, this->y()+desplazado+130);
			m_texto_teclado_luminoso->posicion(this->x()+10, this->y()+desplazado+170);

			m_volumen_salida->posicion(this->x()+220, this->y()+desplazado+130);
			m_teclado_luminoso->posicion(this->x()+220, this->y()+desplazado+170);
		}
	}
}

void Configuracion_Dispositivo::dimension(float ancho, float /*alto*/)
{
	this->_dimension(ancho, this->alto());
	//No se usa el alto en este elemento porque se calcula dependiendo del contenido
	m_habilitado.posicion(this->x() + ancho - 40, m_habilitado.y());

	if(m_elementos_creados)
	{
		m_opciones_tipo_dispositivo->dimension(this->ancho()-240, m_opciones_tipo_dispositivo->alto());

		if(m_datos_dispositivo.es_entrada())
		{
			//Entrada
			m_texto_entrada->dimension(this->ancho()-20, m_texto_entrada->alto());
			m_volumen_entrada->dimension(this->ancho()-240, m_volumen_entrada->alto());
			m_rango_teclado->dimension(this->ancho()-240, m_rango_teclado->alto());
		}

		if(m_datos_dispositivo.es_salida())
		{
			//Salida
			m_texto_salida->dimension(this->ancho()-20, m_texto_salida->alto());
			m_volumen_salida->dimension(this->ancho()-240, m_volumen_salida->alto());
			m_teclado_luminoso->dimension(this->ancho()-240, m_teclado_luminoso->alto());
		}
	}
}

bool Configuracion_Dispositivo::cambio_altura()
{
	bool estado = m_cambio_altura;
	m_cambio_altura = false;
	return estado;
}

Dispositivo_Midi Configuracion_Dispositivo::configuracion()
{
	return m_datos_dispositivo;
}

bool Configuracion_Dispositivo::dispositivo_cambiado()
{
	return m_dispositivo_cambiado;
}

bool Configuracion_Dispositivo::cambio_estado_conexion()
{
	return m_cambio_estado_conexion;
}
