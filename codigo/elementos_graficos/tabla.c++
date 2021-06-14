#include "tabla.h++"

Tabla::Tabla(float x, float y, float ancho, float alto, float alto_fila, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), m_color_fondo(0.9f, 0.9f, 0.9f)
{
	m_recursos = recursos;
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_alto_fila = alto_fila;
	m_fila_seleccionada = 0;//Primera fila predeterminada
	m_seleccion = false;
	m_seleccion_activada = false;

	m_espacio_total_columnas = 0;

	m_panel_desplazamiento = new Panel_Desplazamiento(x, y+m_alto_fila, ancho, alto-m_alto_fila, m_alto_fila, 0, recursos);
}

Tabla::~Tabla()
{
	for(Celda c : m_fila_titulo)
		delete c.texto;

	for(Fila *f : m_filas)
		delete f;

	delete m_panel_desplazamiento;
}

void Tabla::actualizar_ancho_columnas()
{
	float posicion_actual = this->x();
	float ancho_actual = 0;
	for(Celda c : m_fila_titulo)
	{
		c.texto->posicion(posicion_actual, this->y());
		ancho_actual = (static_cast<float>(c.numero_espacio) * this->ancho()) / static_cast<float>(m_espacio_total_columnas);
		c.texto->dimension(ancho_actual, m_alto_fila);
		posicion_actual += ancho_actual;
	}

	for(Fila *f : m_filas)
	{
		f->dimension(this->ancho(), m_alto_fila);

		std::vector<Etiqueta *> *celdas = f->celdas();
		for(unsigned int x = 0; x<m_fila_titulo.size(); x++)
		{
			celdas->at(x)->posicion(m_fila_titulo[x].texto->x(), m_fila_titulo[x].texto->y());
			celdas->at(x)->dimension(m_fila_titulo[x].texto->ancho(), m_fila_titulo[x].texto->alto());
		}
	}
}

void Tabla::actualizar(unsigned int diferencia_tiempo)
{
	m_panel_desplazamiento->actualizar(diferencia_tiempo);
}

void Tabla::dibujar()
{
	//Dibuja el color de fondo
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(this->x(), this->y(), this->ancho(), m_alto_fila, m_color_fondo);

	//Dibuja las filas
	for(unsigned int x=0; x<m_fila_titulo.size(); x++)
		m_fila_titulo[x].texto->dibujar();

	//Dibuja los bordes laterales
	m_panel_desplazamiento->dibujar();
	m_rectangulo->dibujar(this->x(), this->y(), 1, this->alto(), m_color_fondo);
	m_rectangulo->dibujar(this->x()+this->ancho()-1, this->y(), 1, this->alto(), m_color_fondo);
	m_rectangulo->dibujar(this->x(), this->y()+this->alto()-1, this->ancho(), 1, m_color_fondo);
}

void Tabla::evento_raton(Raton *raton)
{
	if(!raton->esta_sobre(this->x(), this->y(), this->ancho(), this->alto()))
		return;//Ignora eventos fuera de la tabla
	m_panel_desplazamiento->evento_raton(raton);
	for(unsigned int x=0; x<m_filas.size(); x++)
	{
		if(m_filas[x]->esta_seleccionado() && x != m_fila_seleccionada)
		{
			m_filas[m_fila_seleccionada]->deseleccionar();
			m_fila_seleccionada = x;
			m_seleccion_activada = false;
		}
		if(m_filas[x]->esta_seleccionado())
		{
			m_seleccion = true;
			if(raton->activado(BotonIzquierdo) && raton->numero_clics() == 2)
			{
				m_seleccion_activada = true;
			}
		}
	}
}

void Tabla::posicion(float x, float y)
{
	this->_posicion(x, y);

	float posicion_actual = this->x();
	float ancho_actual = 0;
	for(Celda c : m_fila_titulo)
	{
		c.texto->posicion(posicion_actual, this->y());
		ancho_actual = (static_cast<float>(c.numero_espacio) * this->ancho()) / static_cast<float>(m_espacio_total_columnas);
		posicion_actual += ancho_actual;
	}
	m_panel_desplazamiento->posicion(this->x(), this->y() + m_alto_fila);
}

void Tabla::dimension(float ancho, float alto)
{
	this->_dimension(ancho, alto);
	this->actualizar_ancho_columnas();
	//La barra de desplazamiento esta despues de la fila de titulo
	m_panel_desplazamiento->dimension(ancho, alto-m_alto_fila);

	for(Fila *m : m_filas)
		m->dimension(this->ancho(), m_alto_fila);
}

void Tabla::agregar_columna(std::string texto, bool centrado, unsigned int numero_espacio)
{
	//numero_espacio representa el espacio que ocupa la columna actual en relacion con el espacio total de todas las columnas
	if(numero_espacio == 0)
		numero_espacio = 1;
	m_espacio_total_columnas += numero_espacio;

	Etiqueta *titulo_celda = new Etiqueta(this->x(), this->y(), centrado, texto, LetraChica, m_recursos);
	titulo_celda->centrado_vertical(true);

	//Si no esta centrado se le agrega un margen
	if(!centrado)
		titulo_celda->margen(10);
	Celda celda_actual = {titulo_celda, centrado, numero_espacio};

	m_fila_titulo.push_back(celda_actual);

	this->actualizar_ancho_columnas();
}

void Tabla::insertar_fila(std::vector<std::string> texto)
{
	if(texto.size() != m_fila_titulo.size())
	{
		Registro::Error("No coincide el numero de columnas.");
		return;
	}

	float posicion_fila_y = this->y();
	if(m_filas.size() > 0)
		posicion_fila_y = m_filas[m_filas.size()-1]->y()+m_alto_fila;//Obtiene la posicion de la fila anterior

	Fila *f = new Fila(this->x(), posicion_fila_y, this->ancho(), m_alto_fila, m_recursos);
	Etiqueta *nueva_etiqueta;
	for(unsigned int x=0; x<texto.size(); x++)
	{
		//Se crean las celdas de la fila
		nueva_etiqueta = new Etiqueta(m_fila_titulo[x].texto->x(), posicion_fila_y, m_fila_titulo[x].centrado, texto[x], LetraChica, m_recursos);
		nueva_etiqueta->centrado_vertical(true);
		if(!m_fila_titulo[x].centrado)
			nueva_etiqueta->margen(10);
		nueva_etiqueta->dimension(m_fila_titulo[x].texto->ancho(), m_alto_fila);
		f->agregar_celda(nueva_etiqueta);
	}

	m_seleccion = false;
	m_fila_seleccionada = 0;
	m_filas.push_back(f);
	m_panel_desplazamiento->agregar_elemento(f);
}

void Tabla::vaciar()
{
	//Se eliminan las filas al cambiar de carpeta
	m_panel_desplazamiento->vaciar();
	for(Fila *m : m_filas)
		delete m;

	m_filas.clear();
	m_fila_seleccionada = 0;
	m_seleccion = false;
}

void Tabla::cambiar_seleccion(int cambio)
{
	if(m_filas.size() == 0)
		return;
	m_seleccion = true;
	m_seleccion_activada = false;
	//Se deselecciona la fila anterior y se marca la siguiente
	//Si llega arriba comienza abajo de nuevo
	if(!m_filas[m_fila_seleccionada]->esta_seleccionado())
	{
		//La fila no esta seleccionada visiblemente
		//hasta que se cambia la seleccion
		m_filas[m_fila_seleccionada]->seleccionar();
	}
	else
	{
		m_filas[m_fila_seleccionada]->deseleccionar();

		if(cambio > 0)
		{
			m_fila_seleccionada++;
			if(m_fila_seleccionada >= m_filas.size())
				m_fila_seleccionada = 0;
		}
		else
		{
			if(m_fila_seleccionada == 0)
				m_fila_seleccionada = m_filas.size() - 1;
			else
				m_fila_seleccionada--;
		}
		m_filas[m_fila_seleccionada]->seleccionar();
	}

	if(m_filas[m_fila_seleccionada]->y() < this->y()+m_alto_fila)
	{
		//Desplaza la barra cuando se sale por arriba
		float inicio_tabla = this->y()+m_alto_fila;
		float inicio_fila = m_filas[m_fila_seleccionada]->y();
		m_panel_desplazamiento->desplazar_y(static_cast<int>(inicio_tabla - inicio_fila));
	}
	else if(m_filas[m_fila_seleccionada]->y() + m_filas[m_fila_seleccionada]->alto() > this->y()+this->alto())
	{
		//Desplaza la barra cuando se sale por abajo
		float final_tabla = this->y()+this->alto();
		float final_fila = m_filas[m_fila_seleccionada]->y() + m_filas[m_fila_seleccionada]->alto();
		m_panel_desplazamiento->desplazar_y(static_cast<int>(final_tabla - final_fila));
	}
}

void Tabla::seleccionar(unsigned long int seleccion)
{
	//Actualiza las posiciones de los elementos antes de moverlos, normalmente esto se hace
	//el panel de desplazamiento una vez al actualizar, pero la llamada a esta función
	//puede ser realizada antes de actualizar
	m_panel_desplazamiento->actualizar_dimension();
	if(seleccion < m_filas.size())
	{
		//Deselecciona la anterior y selecciona la nueva
		m_filas[m_fila_seleccionada]->deseleccionar();
		m_fila_seleccionada = seleccion;
		m_filas[m_fila_seleccionada]->seleccionar();

		//Mueve la barra de desplazamiento para mostrar la file seleccionada
		if(m_filas[m_fila_seleccionada]->y() < this->y()+m_alto_fila)
		{
			//Desplaza la barra cuando se sale por arriba
			float inicio_tabla = this->y()+m_alto_fila;
			float inicio_fila = m_filas[m_fila_seleccionada]->y();
			m_panel_desplazamiento->desplazar_y(static_cast<int>(inicio_tabla - inicio_fila));
		}
		else if(m_filas[m_fila_seleccionada]->y() + m_filas[m_fila_seleccionada]->alto() > this->y()+this->alto())
		{
			//Desplaza la barra cuando se sale por abajo
			float final_tabla = this->y()+this->alto();
			float final_fila = m_filas[m_fila_seleccionada]->y() + m_filas[m_fila_seleccionada]->alto();
			m_panel_desplazamiento->desplazar_y(static_cast<int>(final_tabla - final_fila));
		}
	}
}

unsigned long int Tabla::obtener_seleccion()
{
	m_seleccion_activada = false;
	return m_fila_seleccionada;
}

bool Tabla::seleccion_activada()
{
	return m_seleccion_activada;
}

bool Tabla::seleccion()
{
	return m_seleccion;
}
