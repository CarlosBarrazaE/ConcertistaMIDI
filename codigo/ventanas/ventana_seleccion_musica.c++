#include "ventana_seleccion_musica.h++"

VentanaSeleccionMusica::VentanaSeleccionMusica(Configuracion *configuracion, Datos_Musica *musica, Administrador_Recursos *recursos) : Ventana(), m_texto_titulo(recursos), m_tabla_archivos(10, 90, Pantalla::Ancho-20, Pantalla::Alto-140, 40, recursos), m_ruta_exploracion(10, 50, Pantalla::Ancho-20, 30, recursos)
{
	m_configuracion = configuracion;
	m_musica = musica;
	m_datos = configuracion->base_de_datos();

	m_rectangulo = recursos->figura(F_Rectangulo);

	m_boton_atras = new Boton(10, Pantalla::Alto - 32, 120, 25, "Atrás", LetraChica, recursos);
	m_boton_atras->color_boton(Color(0.9f, 0.9f, 0.9f));

	m_boton_continuar = new Boton(Pantalla::Ancho - 130, Pantalla::Alto - 32, 120, 25, "Continuar", LetraChica, recursos);
	m_boton_continuar->color_boton(Color(0.9f, 0.9f, 0.9f));

	m_texto_titulo.texto("Seleccione una canción para tocar");
	m_texto_titulo.tipografia(recursos->tipografia(LetraTitulo));
	m_texto_titulo.color(Color(1.0f, 1.0f, 1.0f));
	m_texto_titulo.posicion(0, 0);
	m_texto_titulo.dimension(Pantalla::Ancho, 40);
	m_texto_titulo.centrado(true);

	//Fila titulo
	m_tabla_archivos.agregar_columna("Nombre Archivo", false, 5);
	m_tabla_archivos.agregar_columna("Duracion", true, 1);
	m_tabla_archivos.agregar_columna("Tamaño", true, 1);
	m_tabla_archivos.agregar_columna("Visitas", true, 1);
	m_tabla_archivos.agregar_columna("Fecha", true, 2);

	m_carpeta_inicial = m_configuracion->leer("carpeta_inicial");
	m_carpeta_activa = m_configuracion->leer("carpeta_activa");

	//Cargar el nombre de la carpeta inicial
	std::vector<std::vector<std::string>> ruta_carpetas = m_datos->ruta_carpetas();
	for(unsigned int i=0; i<ruta_carpetas.size(); i++)
	{
		if(m_carpeta_inicial == ruta_carpetas[i][1])
			m_nombre_carpeta_inicial = ruta_carpetas[i][0];
	}

	m_es_carpeta_inicial = false;
	if(m_carpeta_inicial == "-")
		m_carpeta_activa = "-";
	if(m_carpeta_activa == "-")
		this->crear_tabla("");
	else
		this->crear_tabla(m_carpeta_activa);
}

VentanaSeleccionMusica::~VentanaSeleccionMusica()
{
	delete m_boton_atras;
	delete m_boton_continuar;
}

void VentanaSeleccionMusica::actualizar(unsigned int diferencia_tiempo)
{
	m_tabla_archivos.actualizar(diferencia_tiempo);
	m_boton_atras->actualizar(diferencia_tiempo);
	m_boton_continuar->actualizar(diferencia_tiempo);
}

void VentanaSeleccionMusica::dibujar()
{
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(0, 0, Pantalla::Ancho, 40, Color(0.141f, 0.624f, 0.933f));
	m_rectangulo->dibujar(0, Pantalla::Alto - 40, Pantalla::Ancho, 40, Color(0.761f, 0.887f, 0.985f));
	m_texto_titulo.dibujar();

	m_ruta_exploracion.dibujar();
	m_tabla_archivos.dibujar();

	m_boton_atras->dibujar();
	m_boton_continuar->dibujar();
}

void VentanaSeleccionMusica::cargar_lista_carpetas()
{
	std::vector<std::vector<std::string>> ruta_carpetas = m_datos->ruta_carpetas();
	for(unsigned int i=0; i<ruta_carpetas.size(); i++)
	{
		Datos_Archivos actual;

		actual.nombre = ruta_carpetas[i][0];
		actual.ruta = ruta_carpetas[i][1];
		actual.tamanno = Funciones::numero_de_archivos(actual.ruta);
		actual.es_carpeta = true;

		//Todo Filtrar archivos Midi
		m_lista_archivos.push_back(actual);
	}
}

void VentanaSeleccionMusica::cargar_contenido_carpeta(std::string ruta_abrir)
{
	m_datos->iniciar_transaccion();
	for(const std::filesystem::directory_entry elemento : std::filesystem::directory_iterator(ruta_abrir))
	{
		std::string ruta = std::string(elemento.path());
		std::string nombre_archivo = Funciones::nombre_archivo(ruta, elemento.is_directory());
		std::string extencion_archivo = Funciones::extencion_archivo(ruta);

		//Reemplaza el guion bajo por espacio
		std::replace(nombre_archivo.begin(), nombre_archivo.end(), '_', ' ');

		bool oculto = false;
		if(nombre_archivo.length() > 0 && nombre_archivo[0] == '.')
			oculto = true;

		if((elemento.is_directory() && !oculto) || (!elemento.is_directory() && Funciones::es_midi(extencion_archivo)))
		{
			Datos_Archivos actual;
			actual.ruta = elemento.path();
			actual.nombre = nombre_archivo;
			actual.es_carpeta = elemento.is_directory();

			if(!elemento.is_directory())
			{
				std::vector<std::string> datos_midi = m_datos->datos_archivo(actual.ruta);
				actual.tamanno = elemento.file_size();
				actual.fecha_acceso = "-";
				if(datos_midi.size() > 0)
				{
					actual.visitas = static_cast<unsigned int>(std::stoi(datos_midi[0]));
					actual.duracion = static_cast<microseconds_t>(std::stol(datos_midi[1]));
					if(datos_midi[2] != "")
						actual.fecha_acceso = datos_midi[2];
				}
				else
				{
					actual.duracion = Funciones::duracion_midi(actual.ruta);//Tiempo en microsegundos
					m_datos->agregar_archivo(actual.ruta, actual.duracion);
				}
			}
			else
				actual.tamanno = Funciones::numero_de_archivos(actual.ruta);//Numero de archivos

			m_lista_archivos.push_back(actual);
		}
	}
	m_datos->finalizar_transaccion();
}

void VentanaSeleccionMusica::crear_tabla(std::string ruta_abrir)
{
	m_tabla_archivos.vaciar();//Se vacia la tabla
	if(ruta_abrir != "" && std::ifstream(ruta_abrir))
	{
		Registro::Nota("Abriendo la carpeta: " + ruta_abrir);

		//Almacena la carpeta raiz para retroceder
		if(m_es_carpeta_inicial)
		{
			m_carpeta_inicial = ruta_abrir;
			m_configuracion->escribir("carpeta_inicial", ruta_abrir);
			m_es_carpeta_inicial = false;

			for(unsigned long int m = 0; m<m_lista_archivos.size(); m++)
			{
				if(ruta_abrir == m_lista_archivos[m].ruta)
					m_nombre_carpeta_inicial = m_lista_archivos[m].nombre;
			}

		}
		m_carpeta_activa = ruta_abrir;
		m_configuracion->escribir("carpeta_activa", ruta_abrir);

		//Limpia la lista de archivos
		m_lista_archivos.clear();

		this->cargar_contenido_carpeta(ruta_abrir);
	}
	else if(ruta_abrir != "")
		Notificacion::Aviso("La carpeta: \"" + ruta_abrir + "\" no existe", 5);
	else
	{
		Registro::Nota("Mostrando la lista de carpetas");
		//Limpia la lista de archivos
		m_lista_archivos.clear();

		m_es_carpeta_inicial = true;
		m_carpeta_inicial = "-";
		m_carpeta_activa = "-";
		m_configuracion->escribir("carpeta_inicial", "-");
		m_configuracion->escribir("carpeta_activa", "-");

		this->cargar_lista_carpetas();
	}
	m_ruta_exploracion.ruta_carpeta(m_carpeta_inicial, m_carpeta_activa, m_nombre_carpeta_inicial);


	if(m_lista_archivos.size() == 0)
	{
		Notificacion::Aviso("Esta carpeta no contiene archivos MIDI", 5);
		m_ruta_exploracion.boton_siguiente_habilitado(false);
		return;
	}
	else
		m_ruta_exploracion.boton_siguiente_habilitado(true);
	//Ordenar Lista
	std::sort(m_lista_archivos.begin(), m_lista_archivos.end());

	//Crear Tabla
	for(unsigned int i=0; i<m_lista_archivos.size(); i++)
	{
		std::vector<std::string> fila_nueva;

		fila_nueva.push_back(m_lista_archivos[i].nombre);

		if(m_lista_archivos[i].es_carpeta)
		{
			fila_nueva.push_back("-");
			if(m_lista_archivos[i].tamanno == 0)
				fila_nueva.push_back("Sin Archivos");
			else if(m_lista_archivos[i].tamanno == 1)
				fila_nueva.push_back(std::to_string(m_lista_archivos[i].tamanno) + " archivo");//Singular
			else
				fila_nueva.push_back(std::to_string(m_lista_archivos[i].tamanno) + " archivos");//Plural
		}
		else
		{
			fila_nueva.push_back(Funciones::microsegundo_a_texto(m_lista_archivos[i].duracion, false));
			fila_nueva.push_back(Texto::bytes_a_texto(m_lista_archivos[i].tamanno));
		}

		fila_nueva.push_back(std::to_string(m_lista_archivos[i].visitas));
		fila_nueva.push_back(m_lista_archivos[i].fecha_acceso);
		m_tabla_archivos.insertar_fila(fila_nueva);
	}
}

bool VentanaSeleccionMusica::abrir_archivo_seleccionado()
{
	unsigned long int seleccion_actual = m_tabla_archivos.obtener_seleccion();
	if(seleccion_actual < m_lista_archivos.size())
	{
		if(m_lista_archivos[seleccion_actual].es_carpeta)
		{
			std::string ruta_nueva = m_lista_archivos[seleccion_actual].ruta;

			//Carga la lista de archivos de la carpeta seleccionada
			this->crear_tabla(ruta_nueva);
			return false;
		}
		else
		{
			//Abre el archivo seleccionado
			Datos_Archivos &archivo_abierto = m_lista_archivos[seleccion_actual];
			Registro::Nota("Abriendo archivo: " + archivo_abierto.ruta);
			if(m_musica->cargar_midi(archivo_abierto.ruta))
			{
				//Se suma la visita y se actualiza la fecha
				m_datos->sumar_visita_archivo(archivo_abierto.ruta);

				//Se actualiza la duracion si el archivo cambia
				if(m_musica->musica()->GetSongLengthInMicroseconds() != archivo_abierto.duracion)
					m_datos->actualizar_archivo(archivo_abierto.ruta, m_musica->musica()->GetSongLengthInMicroseconds());

				m_musica->nombre_musica(archivo_abierto.nombre);
				m_musica->autor("Autor");
				return true;
			}
			return false;
		}
	}
	return false;
}

void VentanaSeleccionMusica::evento_raton(Raton *raton)
{
	m_ruta_exploracion.evento_raton(raton);
	if(m_ruta_exploracion.boton_siguiente())
	{
		if(this->abrir_archivo_seleccionado())
			m_accion = CambiarASeleccionPista;
	}

	if(m_ruta_exploracion.cambiar_carpeta())
		this->crear_tabla(m_ruta_exploracion.nueva_ruta());

	m_tabla_archivos.evento_raton(raton);

	//Abrir el archivo con doble clic
	if(raton->activado(BotonIzquierdo) && m_tabla_archivos.seleccion_activada())
	{
		if(this->abrir_archivo_seleccionado())
			m_accion = CambiarASeleccionPista;
	}

	m_boton_atras->evento_raton(raton);
	m_boton_continuar->evento_raton(raton);

	if(m_boton_atras->esta_activado())
		m_accion = CambiarATitulo;
	else if(m_boton_continuar->esta_activado())
	{
		if(this->abrir_archivo_seleccionado())
			m_accion = CambiarASeleccionPista;
	}
}

void VentanaSeleccionMusica::evento_teclado(Tecla tecla, bool estado)
{
	if(tecla == TECLA_ESCAPE && !estado)
		m_accion = CambiarATitulo;
	else if((tecla == TECLA_ENTRAR || tecla == TECLA_FLECHA_DERECHA) && !estado)
	{
		if(this->abrir_archivo_seleccionado())
			m_accion = CambiarASeleccionPista;
	}
	else if((tecla == TECLA_BORRAR || tecla == TECLA_FLECHA_IZQUIERDA) && !estado)
	{
		m_ruta_exploracion.ir_atraz();
		this->crear_tabla(m_ruta_exploracion.nueva_ruta());
	}
	else if(tecla == TECLA_FLECHA_ABAJO && !estado)
		m_tabla_archivos.cambiar_seleccion(1);
	else if(tecla == TECLA_FLECHA_ARRIBA && !estado)
		m_tabla_archivos.cambiar_seleccion(-1);
}

void VentanaSeleccionMusica::evento_pantalla(float ancho, float alto)
{
	m_texto_titulo.dimension(ancho, 40);
	m_ruta_exploracion.dimension(ancho-20, 30);
	m_tabla_archivos.dimension(ancho-20, alto-140);

	m_boton_atras->posicion(m_boton_atras->x(), alto - 32);
	m_boton_continuar->posicion(ancho - 130, alto - 32);
}
