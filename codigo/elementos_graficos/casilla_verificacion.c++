#include "casilla_verificacion.h++"

Casilla_Verificacion::Casilla_Verificacion(float x, float y, float ancho, float alto, std::string texto, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto)
{
	m_rectangulo = recursos->figura(F_Rectangulo);
	m_textura_activa = recursos->textura(T_CasillaActiva);
	m_textura_inactiva = recursos->textura(T_CasillaInactiva);
	m_textura_sombra = recursos->textura(T_CasillaSombra);

	if(texto.length() > 0)
	{
		m_texto = new Etiqueta(recursos);
		m_texto->texto(texto);
		m_texto->tipografia(recursos->tipografia(LetraMediana));
		m_texto->posicion(this->x()+this->alto()+10, this->y());//Margen para el texto
		m_texto->dimension(this->ancho(), this->alto());
		m_texto->centrado_vertical(true);
		m_texto->color(Color(0.0f, 0.0f, 0.0f));
	}
	else
		m_texto = NULL;

	m_color_textura = Color(1.0f, 1.0f, 1.0f);

	m_activo = false;
	m_clic = false;
	m_cambio_estado = false;

	m_boton_pre_activado = false;
	m_boton_sobre = false;
}

Casilla_Verificacion::~Casilla_Verificacion()
{
	if(m_texto != NULL)
		delete m_texto;
}

void Casilla_Verificacion::actualizar(unsigned int /*diferencia_tiempo*/)
{
}

void Casilla_Verificacion::dibujar()
{
	m_rectangulo->textura(true);

	if(!m_clic)
	{
		m_textura_sombra->activar();
		m_rectangulo->dibujar(this->x()-2, this->y()-2, this->alto()+4, this->alto()+4, Color(1.0f, 1.0f, 1.0f));
	}

	if(m_activo)
		m_textura_activa->activar();
	else
		m_textura_inactiva->activar();
	m_rectangulo->dibujar(this->x()-2, this->y()-2, this->alto()+4, this->alto()+4, Color(1.0f, 1.0f, 1.0f));

	if(m_texto != NULL)
		m_texto->dibujar();
}

void Casilla_Verificacion::evento_raton(Raton *raton)
{
	if(raton->esta_sobre(this->x(), this->y(), this->ancho(), this->alto()))
	{
		if(raton->activado(BotonIzquierdo) && m_boton_sobre)
		{
			//Solo se aceptan el clic cuando estaba sobre el boton antes del evento
			m_clic = true;
			m_boton_pre_activado = true;
		}
		else if(!raton->activado(BotonIzquierdo))
		{
			//Se confirma que esta sobre el boton antes del evento
			m_clic = false;
			m_boton_sobre = true;
			if(m_boton_pre_activado)
			{
				//Se activa al soltar el clic sobre el boton
				m_activo = !m_activo;
				m_cambio_estado = true;
				m_boton_pre_activado = false;
			}
		}
	}
	else
	{
		//Se borra todo
		m_clic = false;
		m_boton_sobre = false;
		m_boton_pre_activado = false;
	}
}

void Casilla_Verificacion::estado(bool estado)
{
	m_activo = estado;
}

bool Casilla_Verificacion::activado()
{
	return m_activo;
}

bool Casilla_Verificacion::cambio_estado()
{
	bool estado = m_cambio_estado;
	m_cambio_estado = false;
	return estado;
}
