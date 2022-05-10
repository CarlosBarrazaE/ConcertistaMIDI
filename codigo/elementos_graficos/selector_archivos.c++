#include "selector_archivos.h++"

Selector_Archivos::Selector_Archivos(float x, float y, float ancho, float alto, std::string titulo, std::string ruta, bool mostrar_archivos, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), m_titulo(recursos), m_ruta(x+10, y+45, ancho-20, 30, recursos), m_tabla(x+10, y+80, ancho-20, alto-130, 30, recursos)
{
	m_mostrar_archivos = mostrar_archivos;
	m_dialogo = Esperar;
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_titulo.texto(titulo);
	m_titulo.tipografia(recursos->tipografia(LetraMediana));
	m_titulo.posicion(this->x(), this->y());
	m_titulo.dimension(this->ancho(), 40);
	m_titulo.centrado(true);
	m_titulo.color(Color(1.0f, 1.0f, 1.0f));

	m_tabla.agregar_columna("Nombre", false, 3);
	m_tabla.agregar_columna("Tamaño", false, 1);
	m_ruta.carpeta_extra("/", "/");
	this->cargar_tabla(ruta);

	m_cancelar = new Boton(this->x() + ancho - 320, y + alto - 40, 150, 30, "Cancelar", LetraMediana, recursos);
	m_aceptar = new Boton(this->x() + ancho - 160, y + alto - 40, 150, 30, "Aceptar", LetraMediana, recursos);
}

Selector_Archivos::~Selector_Archivos()
{
	delete m_cancelar;
	delete m_aceptar;
}

void Selector_Archivos::cargar_tabla(std::string ruta_abrir)
{
	//Seleccionar archivos
	m_ruta_actual = ruta_abrir;
	Registro::Depurar("Abriendo la carpeta " + ruta_abrir);
	if(!std::ifstream(ruta_abrir))
	{
		Registro::Error("Permiso denegado, no se puede acceder a " + ruta_abrir);
		return;
	}
	m_tabla.vaciar();//Se vacia la tabla
	m_ruta.ruta("/", ruta_abrir);
	m_lista_archivos.clear();
	for(const std::filesystem::directory_entry &elemento : std::filesystem::directory_iterator(ruta_abrir))
	{
		try
		{
			std::string nombre_archivo = Funciones::nombre_archivo(elemento.path(), elemento.is_directory());

			bool oculto = false;
			if(nombre_archivo.length() > 0 && nombre_archivo[0] == '.')
				oculto = true;

			if((elemento.is_directory() || m_mostrar_archivos) && !oculto)
			{
				Datos_Archivos actual;
				actual.nombre = nombre_archivo;
				actual.ruta = elemento.path();
				actual.es_carpeta = elemento.is_directory();
				if(elemento.is_directory())
					actual.tamanno = Funciones::numero_de_archivos(actual.ruta);
				else
					actual.tamanno = elemento.file_size();
				m_lista_archivos.push_back(actual);
			}
		}
		catch(std::filesystem::filesystem_error &e)
		{
			Registro::Error("No se puede leer en: " + std::string(elemento.path()));
			Registro::Error(std::string(e.what()));
		}
	}

	if(m_lista_archivos.size() == 0)
		m_ruta.siguiente_habilitado(false);
	else
		m_ruta.siguiente_habilitado(true);

	//Ordenar Lista
	std::sort(m_lista_archivos.begin(), m_lista_archivos.end());

	//Crear tabla
	for(unsigned int x=0; x<m_lista_archivos.size(); x++)
	{
		std::vector<std::string> fila;
		fila.push_back(m_lista_archivos[x].nombre);
		if(m_lista_archivos[x].es_carpeta)
		{
			if(m_lista_archivos[x].tamanno == 0)
				fila.push_back("Sin Archivos");
			else if(m_lista_archivos[x].tamanno == 1)
				fila.push_back(std::to_string(m_lista_archivos[x].tamanno) + " archivo");//Singular
			else
				fila.push_back(std::to_string(m_lista_archivos[x].tamanno) + " archivos");//Plural
		}
		else
			fila.push_back(Texto::bytes_a_texto(m_lista_archivos[x].tamanno));
		m_tabla.insertar_fila(fila);
	}
}

void Selector_Archivos::actualizar(unsigned int diferencia_tiempo)
{
	m_tabla.actualizar(diferencia_tiempo);
}

void Selector_Archivos::dibujar()
{
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(0, 0, Pantalla::Ancho, Pantalla::Alto, Color(0.8f, 0.8f, 0.8f, 0.5f));
	m_rectangulo->dibujar(this->x(), this->y(), this->ancho(), 40, Color(0.145f, 0.707f, 1.0f));
	m_rectangulo->dibujar(this->x(), this->y()+40, this->ancho(), this->alto()-40, Color(1.0f, 1.0f, 1.0f));

	m_titulo.dibujar();
	m_ruta.dibujar();
	m_tabla.dibujar();

	m_cancelar->dibujar();
	m_aceptar->dibujar();
}

void Selector_Archivos::evento_raton(Raton *raton)
{
	m_ruta.evento_raton(raton);
	m_tabla.evento_raton(raton);

	//Cambiar la carpeta haciendo clic en un boton de la ruta
	if(m_ruta.cambiar_carpeta())
		this->cargar_tabla(m_ruta.nueva_ruta());

	if((raton->activado(BotonIzquierdo) && m_tabla.seleccion_activada()) || m_ruta.siguiente())
	{
		//Seleccion con doble clic
		unsigned long int seleccion_actual = m_tabla.obtener_seleccion();
		if(m_lista_archivos.size() > seleccion_actual)
			this->cargar_tabla(m_lista_archivos[seleccion_actual].ruta);
	}
	if(m_tabla.seleccion())
	{
		//Seleccion sin doble clic usada al apretar aceptar
		unsigned long int seleccion_actual = m_tabla.obtener_seleccion();
		if(m_lista_archivos.size() > seleccion_actual)
			m_ruta_actual = m_lista_archivos[seleccion_actual].ruta;
	}

	m_cancelar->evento_raton(raton);
	m_aceptar->evento_raton(raton);

	if(m_cancelar->esta_activado())
		m_dialogo = Cancelar;
	else if(m_aceptar->esta_activado())
		m_dialogo = Aceptar;
}

void Selector_Archivos::posicion(float x, float y)
{
	this->_posicion(x, y);

	m_titulo.posicion(this->x(), this->y());
	m_ruta.posicion(this->x()+10, this->y()+45);
	m_tabla.posicion(this->x()+10, this->y()+80);

	m_cancelar->posicion(this->x() + this->ancho() - 320, this->y() + this->alto() - 40);
	m_aceptar->posicion(this->x() + this->ancho() - 160, this->y() + this->alto() - 40);
}

Dialogo Selector_Archivos::dialogo()
{
	return m_dialogo;
}

std::string Selector_Archivos::ruta_seleccionada()
{
	return m_ruta_actual;
}
