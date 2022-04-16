#include "puntuacion.h++"

Puntuacion::Puntuacion(float x, float y, float ancho, float alto, const Rango_Organo &rango, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), m_texto_puntaje(recursos), m_texto_aciertos(recursos), m_texto_maxcombo(recursos), m_texto_errores(recursos)
{
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_rango_organo = rango;
	m_ultimo_tiempo = 0;
	m_puntaje_total = 0;
	m_notas_totales = 0;
	m_notas_tocadas = 0;
	m_errores = 0;
	m_maximo_combo = 0;
	m_combos = 0;

	Color color_letra(1.0f, 1.0f, 1.0f);
	m_texto_puntaje.texto("Puntaje: 0");
	m_texto_puntaje.tipografia(recursos->tipografia(LetraMediana));
	m_texto_puntaje.posicion(this->x()+5, this->y()+5);//Margen para el texto
	m_texto_puntaje.color(color_letra);

	m_texto_aciertos.texto("Notas: 0/0");
	m_texto_aciertos.tipografia(recursos->tipografia(LetraMediana));
	m_texto_aciertos.posicion(this->x()+5, this->y()+25);//Margen para el texto
	m_texto_aciertos.color(color_letra);

	m_texto_maxcombo.texto("Maximo Combo: 0");
	m_texto_maxcombo.tipografia(recursos->tipografia(LetraMediana));
	m_texto_maxcombo.posicion(this->x()+5, this->y()+45);//Margen para el texto
	m_texto_maxcombo.color(color_letra);

	m_texto_errores.texto("Errores: 0");
	m_texto_errores.tipografia(recursos->tipografia(LetraMediana));
	m_texto_errores.posicion(this->x()+5, this->y()+65);//Margen para el texto
	m_texto_errores.color(color_letra);

	m_actualizar_texto_puntajes = false;
	m_actualizar_texto_aciertos = false;
	m_actualizar_texto_maxcombo = false;
	m_actualizar_texto_errores = false;
}

Puntuacion::~Puntuacion()
{
}

void Puntuacion::actualizar(unsigned int /*diferencia_tiempo*/)
{
	if(m_notas_totales > 0)
	{
		if(m_actualizar_texto_puntajes)
		{
			m_texto_puntaje.texto("Puntaje: " + std::to_string(m_puntaje_total));
			m_actualizar_texto_puntajes = false;
		}

		if(m_actualizar_texto_aciertos)
		{
			m_texto_aciertos.texto("Notas: " + std::to_string(m_notas_tocadas) + "/" + std::to_string(m_notas_totales));
			m_actualizar_texto_aciertos = false;
		}

		if(m_actualizar_texto_errores)
		{
			m_texto_errores.texto("Errores: " + std::to_string(m_errores));
			m_actualizar_texto_errores = false;
		}

		if(m_actualizar_texto_maxcombo)
		{
			m_texto_maxcombo.texto("Maximo Combo: " + std::to_string(m_maximo_combo));
			m_actualizar_texto_maxcombo = false;
		}
	}
}

void Puntuacion::dibujar()
{
	if(m_notas_totales > 0)
	{
		m_rectangulo->textura(false);
		m_rectangulo->dibujar(this->x(), this->y(), this->ancho(), this->alto(), Color(0.5f, 0.5f, 0.5f, 0.5f));
		m_texto_puntaje.dibujar();
		m_texto_aciertos.dibujar();
		m_texto_maxcombo.dibujar();
		m_texto_errores.dibujar();
	}
}

void Puntuacion::evento_raton(Raton */*raton*/)
{
}

void Puntuacion::nota_correcta(unsigned char id_nota, microseconds_t tiempo_inicio, double velocidad_reproducion)
{
	Puntos nota;
	nota.id_nota = id_nota;
	nota.tiempo = tiempo_inicio;
	m_ultimo_tiempo = tiempo_inicio;

	//En el modo aprender la velocidad no importa por lo que envia 0
	if(velocidad_reproducion <= 0)
		velocidad_reproducion = 1;
	if(m_combos > 0)
		nota.puntaje = static_cast<int>(100.0f * static_cast<float>(velocidad_reproducion)) + static_cast<int>(m_combos);
	else
		nota.puntaje = static_cast<int>(100.0f * static_cast<float>(velocidad_reproducion));
	m_notas_tocadas ++;
	m_puntaje_total += nota.puntaje;
	m_puntuacion.push_back(nota);
	m_actualizar_texto_puntajes = true;
	m_actualizar_texto_aciertos = true;
}

void Puntuacion::notas_totales(unsigned int total_nota)
{
	m_notas_totales = total_nota;
	m_actualizar_texto_aciertos = true;
}

void Puntuacion::cambiar_a(microseconds_t tiempo_nuevo)
{
	//Revierte el puntaje si se retrocede el tiempo
	long int posicion_eliminar = 0;
	bool eliminar = false;
	if(tiempo_nuevo < m_ultimo_tiempo)
	{
		for(unsigned long int x = 0; x<m_puntuacion.size(); x++)
		{
			if(m_puntuacion[x].tiempo >= tiempo_nuevo)
			{
				//Encontro el punto inicial desde donde eliminar
				if(!eliminar)
				{
					eliminar = true;
					posicion_eliminar = static_cast<long int>(x);
				}
				//Quita el puntaje que esta despues cuando se retrocede, contando solo dentro del rango del organo
				//Los errores se quedan
				if(m_puntuacion[x].id_nota >= m_rango_organo.tecla_inicial() &&
					m_puntuacion[x].id_nota <= m_rango_organo.tecla_final())
				{
					m_puntaje_total -= m_puntuacion[x].puntaje;
					m_notas_tocadas--;
				}
			}
			else
			{
				//Recalcula el numero de notas y el ultimo tiempo registrado
				m_ultimo_tiempo = m_puntuacion[x].tiempo;
			}
		}
		if(eliminar)
		{
			m_puntuacion.erase(m_puntuacion.begin() + posicion_eliminar, m_puntuacion.end());
			m_actualizar_texto_puntajes = true;
			m_actualizar_texto_aciertos = true;
		}
	}
}

void Puntuacion::sumar_error()
{
	m_puntaje_total--;
	m_errores++;
	m_actualizar_texto_puntajes = true;
	m_actualizar_texto_errores = true;
}

void Puntuacion::cambiar_rango(const Rango_Organo &rango)
{
	if(m_rango_organo != rango)
	{
		m_rango_organo = rango;

		m_puntaje_total = 0;
		m_notas_tocadas = 0;

		//Recalcular
		for(unsigned long int x=0; x<m_puntuacion.size(); x++)
		{
			//No se borra, solo no se cuenta si esta fuera de rango
			if(m_puntuacion[x].id_nota >= m_rango_organo.tecla_inicial() &&
				m_puntuacion[x].id_nota <= m_rango_organo.tecla_final())
			{
				m_puntaje_total += m_puntuacion[x].puntaje;
				m_ultimo_tiempo = m_puntuacion[x].tiempo;
				m_notas_tocadas++;
			}
		}

		m_actualizar_texto_puntajes = true;
		m_actualizar_texto_aciertos = true;
	}
}

void Puntuacion::combo(unsigned int combo)
{
	m_combos += combo;
	if(m_combos > m_maximo_combo)
	{
		m_maximo_combo = m_combos;
		m_actualizar_texto_maxcombo = true;
	}
}

unsigned int Puntuacion::combo()
{
	return m_combos;
}

void Puntuacion::reiniciar_combo()
{
	m_combos = 0;
}
