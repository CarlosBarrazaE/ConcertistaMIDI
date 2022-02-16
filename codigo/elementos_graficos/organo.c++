#include "organo.h++"

Organo::Organo(float x, float y, float ancho, Teclado_Organo *teclado_visible, Teclado_Organo *teclado_util, Administrador_Recursos *recursos) : Elemento(x, y, ancho, 0)
{
	//El origen del organo esta abajo a la izquierda
	m_teclado_visible = teclado_visible;
	m_teclado_util = teclado_util;

	m_tecla_blanca = recursos->textura(T_TeclaBlanca);
	m_tecla_negra = recursos->textura(T_TeclaNegra);

	m_tecla_blanca_presionada = recursos->textura(T_TeclaBlancaPresionada);
	m_tecla_blanca_presionada_doble = recursos->textura(T_TeclaBlancaPresionadaDoble);
	m_tecla_negra_presionada = recursos->textura(T_TeclaNegraPresionada);
	m_circulo = recursos->textura(T_Circulo);

	m_borde_negro = recursos->textura(T_BordeOrganoNegro);
	m_borde_rojo = recursos->textura(T_BordeOrganoRojo);

	m_rectangulo = recursos->figura(F_Rectangulo);

	Sombreador *particulas = recursos->sombreador(S_Particula);
	Textura2D *t_particula = recursos->textura(T_ParticulaNota);
	m_generador_particulas = new Generador_Particulas(particulas, t_particula, 500, 0);//La escala se cambia al calcular los tamaños
	m_tiempo = 0;
	m_numero_particulas = 0;

	m_nota_enviada_anterior = SIN_NOTA;

	m_notas_activas = NULL;
	m_notas_requeridas = NULL;

	calcular_tamannos();
}

Organo::~Organo()
{
	delete m_generador_particulas;
}

void Organo::dibujar_blancas(float x, float y, unsigned int tecla_inicial, unsigned int numero_teclas)
{
	float desplazamiento = x;
	bool tecla_presionada_anterior = false;
	for(unsigned int n=tecla_inicial; n<tecla_inicial+numero_teclas; n++)
	{
		//Se salta las negras
		if(!Octava::es_blanca(n))
			continue;

		Color color_actual = m_notas_activas->at(n);
		if(color_actual != Color(0.0f, 0.0f, 0.0f))
		{
			//La tecla es tocada y la tecla se cambia de color
			m_rectangulo->color(color_actual);

			//Se agregan las particulas del mismo color que la nota
			if(color_actual != Pista::Colores_pista[0])//No se generan particulas para notas erroneas
				m_generador_particulas->agregar_particulas(desplazamiento + m_ancho_tecla_blanca/2.0f - m_ancho_tecla_blanca/2.0f, y, m_numero_particulas, color_actual);
			m_notas_activas->at(n) = Color(0.0f, 0.0f, 0.0f);

			//Cambia el efectos de sombra
			if(tecla_presionada_anterior)
				m_tecla_blanca_presionada_doble->activar();
			else
				m_tecla_blanca_presionada->activar();
			tecla_presionada_anterior = true;
		}
		else
		{
			//La tecla no es tocada, color normal
			m_tecla_blanca->activar();
			//Fuera del m_teclado_util tiene un color mas oscuro
			if(n < m_teclado_util->tecla_inicial() || n > m_teclado_util->tecla_final())
				m_rectangulo->color(Color(0.7f, 0.7f, 0.7f));
			else
				m_rectangulo->color(Color(1.0f, 1.0f, 1.0f));
			tecla_presionada_anterior = false;
		}

		m_rectangulo->dibujar(desplazamiento, y, m_ancho_tecla_blanca - 1, m_alto_tecla_blanca);

		//Dibuja un punto indicando que debe tocar la nota
		if(m_notas_requeridas != NULL)
		{
			std::map<unsigned int, Color>::iterator respuesta = m_notas_requeridas->find(n);
			if(respuesta != m_notas_requeridas->end())
			{
				m_rectangulo->color(respuesta->second);
				m_circulo->activar();
				m_rectangulo->dibujar(desplazamiento+m_ancho_tecla_blanca/4.0f, y+m_alto_tecla_blanca-(m_ancho_tecla_blanca/2.0f)-10, (m_ancho_tecla_blanca/2.0f)-1, (m_ancho_tecla_blanca/2.0f)-1);
			}
		}

		desplazamiento += m_ancho_tecla_blanca;
	}
}

void Organo::dibujar_negras(float x, float y, unsigned int tecla_inicial, unsigned int numero_teclas)
{
	float desplazamiento = 0;
	unsigned int numero_blancas = 0;
	for(unsigned int n=tecla_inicial; n<tecla_inicial+numero_teclas; n++)
	{
		//Se salta las blancas
		if(Octava::es_blanca(n))
		{
			numero_blancas++;
			continue;
		}

		//Numero de la tecla dentro de la octava
		desplazamiento = x + static_cast<float>(numero_blancas) * m_ancho_tecla_blanca + m_ancho_tecla_negra * Octava::desplazamiento_negra(n);

		Color color_actual = m_notas_activas->at(n);
		if(color_actual != Color(0.0f, 0.0f, 0.0f))
		{
			//La tecla es tocada, se cambia de color
			m_rectangulo->color(color_actual);
			m_tecla_negra_presionada->activar();
			if(color_actual != Pista::Colores_pista[0])//No se generan particulas para notas erroneas
				m_generador_particulas->agregar_particulas(desplazamiento + m_ancho_tecla_negra/2.0f - m_ancho_tecla_blanca/2.0f, y, m_numero_particulas, color_actual);
			m_notas_activas->at(n) = Color(0.0f, 0.0f, 0.0f);
		}
		else
		{
			//La tecla no es tocada, color normal
			//Fuera del m_teclado_util tiene un color mas oscuro
			if(n < m_teclado_util->tecla_inicial() || n > m_teclado_util->tecla_final())
				m_rectangulo->color(Color(0.7f, 0.7f, 0.7f));
			else
				m_rectangulo->color(Color(1.0f, 1.0f, 1.0f));
			m_tecla_negra->activar();
		}

		//El ancho de la tecla mas el ancho de la sombra
		m_rectangulo->dibujar(desplazamiento, y, m_ancho_tecla_negra + m_ancho_tecla_negra * 0.22f, m_alto_tecla_negra);

		//Dibuja un punto indicando que debe tocar la nota
		if(m_notas_requeridas == NULL)
			continue;

		std::map<unsigned int, Color>::iterator respuesta = m_notas_requeridas->find(n);
		if(respuesta != m_notas_requeridas->end())
		{
			m_rectangulo->color(static_cast<Color>(respuesta->second)-0.2f);
			m_circulo->activar();
			m_rectangulo->dibujar(desplazamiento+m_ancho_tecla_negra/4.0f, y+m_alto_tecla_negra-(m_ancho_tecla_negra/2.0f)-20, (m_ancho_tecla_negra/2.0f)-1, (m_ancho_tecla_negra/2.0f)-1);
		}
	}
}

void Organo::actualizar(unsigned int diferencia_tiempo)
{
	m_tiempo += static_cast<float>(diferencia_tiempo);

	//Generar particulas
	unsigned int particulas = static_cast<unsigned int>(m_tiempo / 16666666.0f);//16ms
	if(particulas > 0)
	{
		if(static_cast<float>(particulas) * 16666666 < m_tiempo)
			m_tiempo -= static_cast<float>(particulas * 16666666);
		m_numero_particulas = particulas*2;
	}
	else
		m_numero_particulas = 0;
	m_generador_particulas->actualizar((static_cast<float>(diferencia_tiempo)/1000000000.0f)*3.0f);
}

void Organo::dibujar()
{
	//Dibuja un fondo negro
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(this->x(), this->y() - this->alto(), this->ancho(), this->alto(), Color(0.0f, 0.0f, 0.0f));

	//Dibuja las notas blancas
	m_tecla_blanca->activar();
	m_rectangulo->textura(true);
	m_rectangulo->color(Color(1.0f, 1.0f, 1.0f));
	this->dibujar_blancas(this->x(), this->y() - this->alto() + 10, m_teclado_visible->tecla_inicial(), m_teclado_visible->numero_teclas());

	//Dibuja las notas negras
	m_tecla_negra->activar();
	this->dibujar_negras(this->x(), this->y() - this->alto() + 10, m_teclado_visible->tecla_inicial(), m_teclado_visible->numero_teclas());

	//Dibuja un borde negro
	m_borde_negro->activar();
	m_rectangulo->color(Color(1.0f, 1.0f, 1.0f));
	m_rectangulo->dibujar(this->x(), this->y() - this->alto(), this->ancho(), 5);

	//Dibuja un borde rojo
	m_borde_rojo->activar();
	m_rectangulo->dibujar(this->x(), this->y() - this->alto() + 5, this->ancho(), 5);

	//Dibuja las particulas
	m_generador_particulas->dibujar();
}

void Organo::evento_raton(Raton *raton)
{

	if(raton->activado(BotonIzquierdo) && raton->esta_sobre(this->x(), this->y() - this->alto() + 10, this->ancho(), this->alto()))
	{
		unsigned int ajuste_teclas = 0;
		//Cuenta el numero de teclas desde el inicio hasta la primera tecla visible sin contar la visible
		if(m_teclado_visible->tecla_inicial() > 0)
			ajuste_teclas = Octava::blancas_desde_inicio(m_teclado_visible->tecla_inicial()-1);

		//Cuenta las teclas blancas que aparecen en pantalla
		unsigned int tecla_presionada = static_cast<unsigned int>((static_cast<float>(raton->x()) - this->x()) / m_ancho_tecla_blanca);

		//Agrega las teclas blancas que faltan con el ajuste_teclas
		unsigned int nota_enviar = Octava::nota_id_desde_blanca(tecla_presionada+ajuste_teclas);

		//Verifica si es posible que se haya tocado una tecla negra
		if(raton->esta_sobre(this->x(), this->y() - this->alto() + 10, this->ancho(), m_alto_tecla_negra))
		{
			float desplazamiento = 0;
			//Revisa si existe una tecla negra a la izquierda
			int negra_encontrada = false;
			if(nota_enviar > 0 && !Octava::es_blanca(nota_enviar-1))
			{
				//Numero de la tecla dentro de la octava
				desplazamiento = this->x() + static_cast<float>(tecla_presionada) * m_ancho_tecla_blanca + m_ancho_tecla_negra * Octava::desplazamiento_negra(nota_enviar-1);
				//Se toco una tecla negra a la izquierda de la blanca
				if(raton->esta_sobre(desplazamiento, this->y() - this->alto() + 10, m_ancho_tecla_negra, m_alto_tecla_negra))
				{
					nota_enviar--;
					negra_encontrada = true;
				}
			}
			//Revisa si existe una tecla negra a la derecha
			if(!negra_encontrada && nota_enviar < 127 && !Octava::es_blanca(nota_enviar+1))
			{
				desplazamiento = this->x() + static_cast<float>(tecla_presionada+1) * m_ancho_tecla_blanca + m_ancho_tecla_negra * Octava::desplazamiento_negra(nota_enviar+1);
				//Se toco una tecla negra a la derecha de la blanca
				if(raton->esta_sobre(desplazamiento, this->y() - this->alto() + 10, m_ancho_tecla_negra, m_alto_tecla_negra))
					nota_enviar++;
			}
		}

		//Revisa que sea una nota diferenta a la enviada anteriormente
		if(m_nota_enviada_anterior != nota_enviar)
		{
			//Apaga la nota anterior si se habia enviado una
			//if(m_nota_enviada_anterior != SIN_NOTA)
			//	sendNote(m_nota_enviada_anterior, false);
			//Envia la nueva nota
			//sendNote(nota_enviar, true);
			m_nota_enviada_anterior = nota_enviar;
		}
	}
	else if(m_nota_enviada_anterior != SIN_NOTA)
	{
		//sendNote(m_nota_enviada_anterior, false);
		m_nota_enviada_anterior = SIN_NOTA;
	}
}

void Organo::dimension(float ancho, float alto)
{
	this->_dimension(ancho, alto);
	this->calcular_tamannos();
}

void Organo::notas_activas(std::array<Color, 128> *notas)
{
	m_notas_activas = notas;
}

void Organo::notas_requeridas(std::map<unsigned int, Color> *notas_requeridas)
{
	m_notas_requeridas = notas_requeridas;
}

void Organo::calcular_tamannos()
{
	unsigned int numero_blancas = Octava::numero_blancas(m_teclado_visible->tecla_inicial(), m_teclado_visible->numero_teclas());
	m_ancho_tecla_blanca = (this->ancho() / static_cast<float>(numero_blancas));
	m_alto_tecla_blanca = m_ancho_tecla_blanca * PROPORCION_BLANCA;
	if(m_alto_tecla_blanca > 250)
		m_alto_tecla_blanca = 250;

	m_ancho_tecla_negra = m_ancho_tecla_blanca * PROPORCION_ANCHO_NEGRA;
	m_alto_tecla_negra = m_alto_tecla_blanca * PROPORCION_NEGRA;

	m_generador_particulas->escala(m_ancho_tecla_blanca);
	this->_dimension(this->ancho(), m_alto_tecla_blanca + 11);
}
