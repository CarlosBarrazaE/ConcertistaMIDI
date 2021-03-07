#include "ruta_exploracion.h++"

Ruta_Exploracion::Ruta_Exploracion(float x, float y, float ancho, float alto, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto)
{
	m_recursos = recursos;
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_boton_atras = new Boton(x, y, 30, 30, "<", recursos);
	m_boton_atras->color_boton(Color(1.0f, 1.0f, 1.0f));

	m_boton_adelante = new Boton(x+30, y, 30, 30, ">", recursos);
	m_boton_adelante->color_boton(Color(1.0f, 1.0f, 1.0f));

	m_ancho_actual = 65;
	m_primera_carpeta_dibujar = 0;
	m_numero_carpeta_extra = 0;
	m_agregar_carpeta_extra = true;
	m_cambiar_carpeta = false;
}

Ruta_Exploracion::~Ruta_Exploracion()
{
	delete m_boton_atras;
	delete m_boton_adelante;

	for(Boton *b : m_carpetas)
		delete b;
}

void Ruta_Exploracion::eliminar(unsigned long int inicio)
{
	if(m_carpetas.size() > 1)
	{
		if(inicio+1 == m_carpetas.size())
		{
			//Elimina la ultima
			m_ancho_actual -= m_carpetas.back()->ancho();
			delete m_carpetas.back();
			m_carpetas.pop_back();
			m_rutas_botones.pop_back();
		}
		else
		{
			//Elimina multiples
			if(inicio > 0 && inicio < m_carpetas.size())
			{
				for(unsigned long int x = inicio; x<m_carpetas.size(); x++)
				{
					m_ancho_actual -= m_carpetas[x]->ancho();
					delete m_carpetas[x];
				}
				m_carpetas.erase(m_carpetas.begin() + static_cast<long int>(inicio), m_carpetas.end());
				m_rutas_botones.erase(m_rutas_botones.begin() + static_cast<long int>(inicio), m_rutas_botones.end());
			}
		}

		//Si quedan solo carpetas extras se pueden agregar mas
		if(m_carpetas.size() <= m_numero_carpeta_extra)
		{
			m_agregar_carpeta_extra = true;
			m_numero_carpeta_extra = m_carpetas.size();
		}
		this->calcular_carpetas_visibles();
	}
}

void Ruta_Exploracion::calcular_carpetas_visibles()
{
	unsigned long int carpeta_visible_actual = m_primera_carpeta_dibujar;
	if(this->ancho() >= m_ancho_actual && m_primera_carpeta_dibujar > 0)
		m_primera_carpeta_dibujar = 0;
	else if(this->ancho() < m_ancho_actual)
	{
		//Calcular cuantos botones se pueden dibujar sin salirse de la ventana
		//almacena el primer boton en m_primera_carpeta_dibujar
		float ancho_nuevo = 65;
		for(unsigned long int x=m_carpetas.size(); x>0; x--)
		{
			ancho_nuevo += m_carpetas[x-1]->ancho();
			if(ancho_nuevo < this->ancho())
				m_primera_carpeta_dibujar = x-1;
			else
				x = 1;//Fin del ciclo
		}
	}
	if(carpeta_visible_actual != m_primera_carpeta_dibujar)
	{
		//Se recalculan las posiciones X
		float posicion_x = 65;
		for(unsigned long int x=m_primera_carpeta_dibujar; x<m_carpetas.size(); x++)
		{
			m_carpetas[x]->posicion(this->x() + posicion_x , m_carpetas[x]->y());
			posicion_x += m_carpetas[x]->ancho();
		}
	}
}

void Ruta_Exploracion::actualizar(unsigned int /*diferencia_tiempo*/)
{
}

void Ruta_Exploracion::dibujar()
{
	m_boton_atras->dibujar();
	m_boton_adelante->dibujar();

	for(unsigned long int x = m_primera_carpeta_dibujar; x < m_carpetas.size(); x++)
		m_carpetas[x]->dibujar();
}

void Ruta_Exploracion::evento_raton(Raton *raton)
{
	m_boton_atras->evento_raton(raton);
	m_boton_adelante->evento_raton(raton);

	if(m_boton_atras->esta_activado())
	{
		m_cambiar_carpeta = true;
		this->eliminar(m_carpetas.size()-1);
	}

	unsigned long int eliminar = 0;
	for(unsigned long int x = m_primera_carpeta_dibujar; x<m_carpetas.size() && eliminar == 0; x++)
	{
		m_carpetas[x]->evento_raton(raton);
		if(m_carpetas[x]->esta_activado())
		{
			m_cambiar_carpeta = true;
			eliminar = x+1;
		}
	}
	if(eliminar > 0)
		this->eliminar(eliminar);
}

void Ruta_Exploracion::posicion(float x, float y)
{
	float diferencia_x = this->x() - x;
	this->_posicion(x, y);

	m_boton_atras->posicion(this->x(), this->y());
	m_boton_adelante->posicion(this->x()+30, this->y());

	for(Boton *b : m_carpetas)
		b->posicion(b->x() - diferencia_x, this->y());
}

void Ruta_Exploracion::dimension(float ancho, float alto)
{
	this->_dimension(ancho, alto);
	this->calcular_carpetas_visibles();
}

void Ruta_Exploracion::carpeta_extra(const std::string &nombre, const std::string &ruta)
{
	//Solo se permiten carpetas extras al inicio
	if(m_agregar_carpeta_extra)
	{
		m_carpetas.push_back(new Boton(this->x() + m_ancho_actual, this->y(), 30, 30, nombre, LetraChica, m_recursos));
		m_rutas_botones.push_back(ruta);

		m_ancho_actual += m_carpetas.back()->ancho();
		m_numero_carpeta_extra++;
	}
}

void Ruta_Exploracion::ruta(const std::string &inicio, const std::string &fin)
{
	if(fin.find(inicio) != std::string::npos)//La ruta final debe contener la ruta inicial
	{
		if(inicio.size() < fin.size())
		{
			std::string ruta = fin.substr(inicio.size());//Se quita la parte de la ruta que no se usara
			std::vector<std::string> carpetas_anidadas = Texto::dividir_texto(ruta, '/');//Se dividen las carpetas anidadas

			std::string ruta_actual = inicio;
			//Agrega un '/' al final si no lo tiene
			if(ruta_actual[ruta_actual.length()-1] != '/')
				ruta_actual += "/";

			//Se crea un boton por cada carpeta anidada
			for(unsigned long int x=0; x<carpetas_anidadas.size(); x++)
			{
				//Guarda la ruta del boton actual
				ruta_actual += carpetas_anidadas[x] + "/";
				if(x >= m_carpetas.size() - m_numero_carpeta_extra)//Se salta los que ya existe
				{
					//Agrega el boton actual
					m_carpetas.push_back(new Boton(this->x()+m_ancho_actual, this->y(), 30, 30, carpetas_anidadas[x], LetraChica, m_recursos));
					m_ancho_actual += m_carpetas.back()->ancho();
					m_rutas_botones.push_back(ruta_actual);
					m_agregar_carpeta_extra = false;//Ya no se permiten mas carpetas extras una vez que entra aqui
				}
			}
		}
	}
	this->calcular_carpetas_visibles();
	if(m_carpetas.size() <= 1)
		m_boton_atras->habilitado(false);
	else
		m_boton_atras->habilitado(true);
}

void Ruta_Exploracion::atras()
{
	this->eliminar(m_carpetas.size()-1);
}

bool Ruta_Exploracion::siguiente()
{
	return m_boton_adelante->esta_activado();
}

void Ruta_Exploracion::siguiente_habilitado(bool estado)
{
	m_boton_adelante->habilitado(estado);
}

bool Ruta_Exploracion::cambiar_carpeta()
{
	bool cambiar = m_cambiar_carpeta;
	m_cambiar_carpeta = false;
	return cambiar;
}

std::string Ruta_Exploracion::nueva_ruta()
{
	if(m_rutas_botones.size() > 0)
		return m_rutas_botones.back();//Retorna la ultima carpeta
	return "";
}
