#include "tablero_notas.h++"

Tablero_Notas::Tablero_Notas(float x, float y, float ancho, float alto, Rango_Organo *teclado_visible, Rango_Organo *teclado_util, Evaluacion *evaluacion, Administrador_Recursos *recursos)
: Elemento(x, y, ancho, alto)
{
	m_duracion_nota = 6500;
	m_teclado_visible = teclado_visible;
	m_teclado_util = teclado_util;
	m_evaluacion = evaluacion;
	this->calcular_tamannos();

	m_textura_nota = recursos->textura(T_Nota);
	m_textura_nota_resaltada = recursos->textura(T_NotaResaltada);

	m_rectangulo = recursos->figura(F_Rectangulo);
	m_tipografia = recursos->tipografia(LetraMuyChica);
	m_recursos = recursos;

	m_desplazamiento = Bajar;
}

Tablero_Notas::~Tablero_Notas()
{
	for(std::map<int, Etiqueta*>::iterator i = m_texto_numeros.begin(); i != m_texto_numeros.end(); i++)
		delete i->second;
	m_texto_numeros.clear();
}

void Tablero_Notas::calcular_tamannos()
{
	m_ancho_blanca = (this->ancho() / static_cast<float>(Octava::numero_blancas(m_teclado_visible->tecla_inicial(), m_teclado_visible->numero_teclas())));
	m_ancho_negra = m_ancho_blanca * PROPORCION_ANCHO_NEGRA;
}

void Tablero_Notas::dibujar_lineas_horizontales_bajar()
{
	int numero_linea = 0;
	float posicion_y = 0;
	Etiqueta *numero_temporal;

	for(unsigned int i=0; i<m_lineas.size(); i++)
	{
		numero_linea++;

		//Dibuja solo las lineas que seran visibles
		posicion_y = (static_cast<float>(m_tiempo_actual_midi) - static_cast<float>(m_lineas[i])) / static_cast<float>(m_duracion_nota);

		if(posicion_y < -this->alto())
			break;
		else if(posicion_y > 0)
			continue;

		//Se carga el numero de linea a mostrar
		numero_temporal = m_texto_numeros[numero_linea];
		if(!numero_temporal)
		{
			//Si no existe se crea
			numero_temporal = new Etiqueta(this->x()+10, this->y()+posicion_y + this->alto(), false, std::to_string(numero_linea), m_tipografia, m_recursos);
			m_texto_numeros[numero_linea] = numero_temporal;
		}

		//Dibuja la linea horizontal
		m_rectangulo->dibujar(this->x(), this->y()+posicion_y + this->alto(), this->ancho(), 1);

		//Dibuja el numero de linea
		numero_temporal->posicion(this->x()+10, this->y()+posicion_y + this->alto()-static_cast<float>(m_tipografia->alto_texto()));
		numero_temporal->dibujar();
	}
}

void Tablero_Notas::dibujar_lineas_horizontales_subir()
{
	int numero_linea = 0;
	float posicion_y = 0;
	Etiqueta *numero_temporal;

	for(unsigned int i=0; i<m_lineas.size(); i++)
	{
		numero_linea++;

		//Dibuja solo las lineas que seran visibles
		posicion_y = (static_cast<float>(m_lineas[i]) - static_cast<float>(m_tiempo_actual_midi)) / static_cast<float>(m_duracion_nota);

		if(posicion_y > 0)
			break;
		else if(posicion_y < -this->alto())
			continue;

		//Se carga el numero de linea a mostrar
		numero_temporal = m_texto_numeros[numero_linea];
		if(!numero_temporal)
		{
			//Si no existe se crea
			numero_temporal = new Etiqueta(this->x()+10, this->y()+posicion_y + this->alto(), false, std::to_string(numero_linea), m_tipografia, m_recursos);
			m_texto_numeros[numero_linea] = numero_temporal;
		}

		//Dibuja la linea horizontal
		m_rectangulo->dibujar(this->x(), this->y()+posicion_y + this->alto(), this->ancho(), 1);

		//Dibuja el numero de linea
		numero_temporal->posicion(this->x()+10, this->y()+posicion_y + this->alto() - static_cast<float>(m_tipografia->alto_texto()));
		numero_temporal->dibujar();
	}
}

void Tablero_Notas::dibujar_lineas_verticales()
{
	float posicion_x = 0;
	bool primera_parte = false;//La primera parte corresponde a la linea de DO
	unsigned char posicion_inicial = m_teclado_visible->tecla_inicial() % 12;
	if(posicion_inicial == 0 || posicion_inicial > 5)
	{
		//Dibuja linea de DO
		primera_parte = true;
		//Ajusta la posicion_x si no empieza en DO
		if(posicion_inicial != 0)
			posicion_x = m_ancho_blanca * static_cast<float>(8 - Octava::blancas_desde_inicio(posicion_inicial-1) - 1);
	}
	else
	{
		//Dibuja linea de FA
		//Ajusta la posicion_x si no empieza en FA
		if(posicion_inicial != 5)
			posicion_x = m_ancho_blanca * static_cast<float>(4 - Octava::blancas_desde_inicio(posicion_inicial-1) - 1);
	}
	for(int i=0; i<22 && posicion_x < this->ancho(); i++)
	{
		//Se dibuja la linea vertical
		m_rectangulo->dibujar(this->x()+posicion_x, this->y(), 1, this->alto());
		if(primera_parte)
			posicion_x += m_ancho_blanca * 3;
		else
			posicion_x += m_ancho_blanca * 4;
		primera_parte = !primera_parte;
	}
}

void Tablero_Notas::dibujar_notas_bajar(unsigned int pista)
{
	float largo_nota = 0;
	float posicion_y = 0;//Es negativo hasta que la tota sale de la pantalla

	//Datos para el dibujo final
	unsigned char numero_nota = 0;//Id de la nota desde 0 hasta 127
	unsigned char numero_notas_omitir = 0;
	if(m_teclado_visible->tecla_inicial() > 0)
		numero_notas_omitir = Octava::blancas_desde_inicio(m_teclado_visible->tecla_inicial()-1);
	float ancho_tecla = 0;//El ancho puede cambiar si es blanca o es negra
	float ajuste_negra = 0;//Permite desplazar la nota negra un poco en relacion a la blanca
	for(unsigned int n=m_ultima_nota[pista]; n<m_notas[pista].size(); n++)
	{
		//Numero_nota incluye blancas y negras
		numero_nota = static_cast<unsigned char>(m_notas[pista][n].note_id);

		//Se salta las notas fuera de la pantalla hacia los lados
		if(numero_nota < m_teclado_visible->tecla_inicial() || numero_nota >= m_teclado_visible->tecla_inicial() + m_teclado_visible->numero_teclas())
		{
			//Se actualiza si es la ultima nota que salio de la pantalla aunque no se vea
			if(n == m_ultima_nota[pista])
				m_ultima_nota[pista] = n+1;
			continue;
		}

		posicion_y = static_cast<float>(m_tiempo_actual_midi - m_notas[pista][n].start) / static_cast<float>(m_duracion_nota);
		//No se dibujan las notas que aun no entran en la pantalla
		if(posicion_y < -this->alto())
			break;

		largo_nota = static_cast<float>(m_notas[pista][n].end - m_notas[pista][n].start) / static_cast<float>(m_duracion_nota);

		//Largo minimo de nota a 20, si es 0 no es visible
		if(largo_nota > 0 && largo_nota < 20)
			largo_nota = 20;

		//El alto minimo de la nota a mostrar es de 20 pixeles
		if(posicion_y-largo_nota > 0 || largo_nota <= 0)//La nota n salio de la pantalla
		{
			if(n == m_ultima_nota[pista])
				m_ultima_nota[pista] = n+1;
			//No se dibujan las notas que ya salieron de la pantalla o son invisibles (largo igual a cero)
			continue;
		}

		//Notas tocadas por el jugador
		Color color_nota_actual = m_pistas->at(pista).color();
		if(Octava::es_blanca(static_cast<unsigned char>(m_notas[pista][n].note_id)))
		{
			//Dibuja las notas blancas
			ancho_tecla = m_ancho_blanca;
			ajuste_negra = 0;

			//Se establece el color de la nota
			m_rectangulo->color(color_nota_actual);
		}
		else
		{
			//Dibuja las notas negras
			ancho_tecla = m_ancho_negra;

			//Mueve la tecla un poco dependiendo de su posicion
			ajuste_negra = m_ancho_blanca + m_ancho_negra * Octava::desplazamiento_negra(numero_nota);

			//La nota negra es un poco mas oscura
			m_rectangulo->color(color_nota_actual-0.2f);
		}

		if(m_pistas->at(pista).modo() != Fondo && numero_nota >= m_teclado_util->tecla_inicial() && numero_nota <= m_teclado_util->tecla_final())
		{
			//Cambia de color a la nota tocada correctamente
			Evaluacion::iterator pista_buscada = m_evaluacion->find(pista);
			if(pista_buscada != m_evaluacion->end() && pista_buscada->second.size() > n)
			{
				std::vector<Tiempos_Nota> &pista_actual = pista_buscada->second;
				if(pista_actual[n].tocada && pista_actual[n].fin_tocado == LLONG_MIN)
					m_rectangulo->color(Color(color_nota_actual.rojo()+0.1f, color_nota_actual.verde()+0.1f, color_nota_actual.azul()+0.1f));
				//Nota ploma al no tocarla cuando se termina el tiempo
				else if (!pista_actual[n].tocada && pista_actual[n].inicio + TIEMPO_DETECCION_FINAL < m_tiempo_actual_midi)
					m_rectangulo->color(Pista::Colores_pista[0]);
			}
		}

		unsigned char numero_blancas = Octava::blancas_desde_inicio(numero_nota) - numero_notas_omitir;
		if(numero_blancas > 0)
			numero_blancas--;
		else
			ajuste_negra -= m_ancho_blanca;//Esto ocurre cuando comienza con una negra, se le quita el ancho de la blanca

		m_textura_nota->activar();
		m_rectangulo->dibujar_estirable(this->x() + static_cast<float>(numero_blancas) * m_ancho_blanca + ajuste_negra, this->y()+this->alto()+posicion_y-largo_nota, ancho_tecla, largo_nota, 0, 10);
/*
		//Muestra el rango de deteccion
		m_rectangulo->color(Color(0.0f, 0.7f, 0.2f, 0.5f));
		float posicion_y_d = static_cast<float>(m_tiempo_actual_midi - (m_notas[pista][n].start-TIEMPO_TOLERANCIA)) / static_cast<float>(m_duracion_nota);
		float largo_nota_d = static_cast<float>(TIEMPO_TOLERANCIA) / static_cast<float>(m_duracion_nota);
		m_rectangulo->dibujar_estirable(this->x() + static_cast<float>(numero_blancas) * m_ancho_blanca + ajuste_negra, this->y()+this->alto()+posicion_y_d-largo_nota_d, ancho_tecla, largo_nota_d, 0, 10);
*/
/*
		//Muestra lo que realmente toco el jugador
		if(m_pistas->at(pista).modo() != Fondo)
		{
			//AVISO no se esta verificando si (*m_evaluacion)[pista][n] existe
			microseconds_t inicio_tocado = (*m_evaluacion)[pista][n].inicio_tocado;
			microseconds_t fin_tocado = (*m_evaluacion)[pista][n].fin_tocado;

			if(inicio_tocado > 0 || fin_tocado > 0)
			{
				//Dibuja de otro color lo tocado por el jugador
				m_rectangulo->color(Color(color_nota_actual.rojo()+0.4f, color_nota_actual.verde(), color_nota_actual.azul(), 0.5f));
				float posicion_y_tocado = static_cast<float>(m_tiempo_actual_midi - inicio_tocado) / static_cast<float>(m_duracion_nota);
				float largo_nota_tocado = static_cast<float>(fin_tocado - inicio_tocado) / static_cast<float>(m_duracion_nota);
				m_rectangulo->dibujar_estirable(this->x() + static_cast<float>(numero_blancas) * m_ancho_blanca + ajuste_negra, this->y()+this->alto()+posicion_y_tocado-largo_nota_tocado, ancho_tecla, largo_nota_tocado, 0, 10);
			}
		}
*/
	}
}

void Tablero_Notas::dibujar_notas_subir(unsigned int pista)
{
	float largo_nota = 0;
	float posicion_y = 0;//Es negativo hasta que la tota sale de la pantalla

	//Datos para el dibujo final
	unsigned char numero_nota = 0;//Id de la nota desde 0 hasta 127
	unsigned char numero_notas_omitir = 0;
	if(m_teclado_visible->tecla_inicial() > 0)
		numero_notas_omitir = Octava::blancas_desde_inicio(m_teclado_visible->tecla_inicial()-1);
	float ancho_tecla = 0;//El ancho puede cambiar si es blanca o es negra
	float ajuste_negra = 0;//Permite desplazar la nota negra un poco en relacion a la blanca
	for(unsigned int n=m_ultima_nota[pista]; n<m_notas[pista].size(); n++)
	{
		//Numero_nota incluye blancas y negras
		numero_nota = static_cast<unsigned char>(m_notas[pista][n].note_id);

		//Se salta las notas fuera de la pantalla hacia los lados
		if(numero_nota < m_teclado_visible->tecla_inicial() || numero_nota >= m_teclado_visible->tecla_inicial() + m_teclado_visible->numero_teclas())
		{
			//Se actualiza si es la ultima nota que salio de la pantalla aunque no se vea
			if(n == m_ultima_nota[pista])
				m_ultima_nota[pista] = n+1;
			continue;
		}

		posicion_y = static_cast<float>(m_notas[pista][n].start - m_tiempo_actual_midi) / static_cast<float>(m_duracion_nota);

		//No se dibujan las notas que aun no entran en pantalla
		if(posicion_y > 0)
			break;

		largo_nota = static_cast<float>(m_notas[pista][n].end - m_notas[pista][n].start) / static_cast<float>(m_duracion_nota);

		//Largo minimo de nota a 20, si es 0 no es visible
		if(largo_nota > 0 && largo_nota < 20)
			largo_nota = 20;

		//No se dibujan las notas que ya salieron de la pantalla
		if(posicion_y+largo_nota < -this->alto() || largo_nota <= 0)
		{
			//Se actualiza si es la ultima nota que salio de la pantalla
			if(n == m_ultima_nota[pista])
				m_ultima_nota[pista] = n+1;
			continue;
		}

		//Notas tocadas por el jugador
		Color color_nota_actual = m_pistas->at(pista).color();
		if(Octava::es_blanca(static_cast<unsigned char>(m_notas[pista][n].note_id)))
		{
			//Dibuja las notas blancas
			ancho_tecla = m_ancho_blanca;
			ajuste_negra = 0;

			//Se establece el color de la nota
			m_rectangulo->color(color_nota_actual);
		}
		else
		{
			//Dibuja las notas negras
			ancho_tecla = m_ancho_negra;

			//Mueve la tecla un poco dependiendo de su posicion
			ajuste_negra = m_ancho_blanca + m_ancho_negra * Octava::desplazamiento_negra(numero_nota);

			//La nota negra es un poco mas oscura
			m_rectangulo->color(color_nota_actual-0.2f);
		}

		unsigned int numero_blancas = Octava::blancas_desde_inicio(numero_nota) - numero_notas_omitir;
		if(numero_blancas > 0)
			numero_blancas--;
		else
			ajuste_negra -= m_ancho_blanca;//Esto ocurre cuando comienza con una negra, se le quita el ancho de la blanca

		m_textura_nota->activar();
		m_rectangulo->dibujar_estirable(this->x() + static_cast<float>(numero_blancas) * m_ancho_blanca + ajuste_negra, this->y()+this->alto()+posicion_y, ancho_tecla, largo_nota, 0, 10);
	}
}

void Tablero_Notas::actualizar(unsigned int /*diferencia_tiempo*/)
{
}

void Tablero_Notas::dibujar()
{
	//Dibuja el fondo
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(this->x(), this->y(), this->ancho(), this->alto(), Color(0.95f, 0.95f, 0.95f));

	//Dibuja las lineas
	m_rectangulo->color(Color(0.7f, 0.7f, 0.7f));
	if(m_desplazamiento == Bajar)
		this->dibujar_lineas_horizontales_bajar();
	else if(m_desplazamiento == Subir)
		this->dibujar_lineas_horizontales_subir();
	this->dibujar_lineas_verticales();

	//Dibuja las notas por pistas
	m_rectangulo->textura(true);
	m_rectangulo->extremos_fijos(false, true);
	for(unsigned int pista=0; pista<m_notas.size(); pista++)
	{
		//Dibuja solo las pistas que tienen notas, hay pistas vacias
		if(m_notas[pista].size() > 0)
		{
			//Dibuja solo las pistas visibles
			if(m_pistas->at(pista).visible())
			{
				if(m_desplazamiento == Bajar)
					this->dibujar_notas_bajar(pista);
				else if(m_desplazamiento == Subir)
					this->dibujar_notas_subir(pista);
			}
		}
	}
	m_rectangulo->extremos_fijos(false, false);
}

void Tablero_Notas::evento_raton(Raton */*raton*/)
{
}

void Tablero_Notas::dimension(float ancho, float alto)
{
	this->_dimension(ancho, alto);
	this->calcular_tamannos();
}

void Tablero_Notas::tiempo(microseconds_t tiempo)
{
	m_tiempo_actual_midi = tiempo;
}

void Tablero_Notas::notas(NotasPistas notas)
{
	m_notas = notas;
}

void Tablero_Notas::lineas(MidiEventMicrosecondList lineas)
{
	m_lineas = lineas;
}

void Tablero_Notas::pistas(std::vector<Pista> *pistas)
{
	m_pistas = pistas;
	for(unsigned int i=0; i<pistas->size(); i++)
		m_ultima_nota.push_back(0);//Se inician todas las pistas en 0
}

unsigned int Tablero_Notas::duracion_nota()
{
	return m_duracion_nota;
}

void Tablero_Notas::duracion_nota(int valor)
{
	if(valor < 0)
		m_duracion_nota = m_duracion_nota - 100;
	else
		m_duracion_nota = m_duracion_nota + 100;

	if(m_duracion_nota < 1500)
		m_duracion_nota = 1500;
	else if(m_duracion_nota > 14000)
		m_duracion_nota = 14000;
}

void Tablero_Notas::modificar_duracion_nota(unsigned int valor)
{
	m_duracion_nota = valor;

	if(m_duracion_nota < 1500)
		m_duracion_nota = 1500;
	else if(m_duracion_nota > 14000)
		m_duracion_nota = 14000;
}

void Tablero_Notas::reiniciar()
{
	for(unsigned int i=0; i<m_ultima_nota.size(); i++)
		m_ultima_nota[i] = 0;
}

void Tablero_Notas::desplazamiento(Direccion direccion)
{
	m_desplazamiento = direccion;
}
