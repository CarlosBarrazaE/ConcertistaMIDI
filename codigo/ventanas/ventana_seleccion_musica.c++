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
	m_tabla_archivos.agregar_columna("Visitas", true, 1);
	m_tabla_archivos.agregar_columna("Tamaño", true, 1);
	m_tabla_archivos.agregar_columna("Fecha", true, 2);

	m_es_carpeta_inicial = false;
	m_carpeta_inicial = m_configuracion->carpeta_inicial();
	m_carpeta_activa = m_configuracion->carpeta_activa();

	m_ruta_exploracion.carpeta_extra("Inicio", "-");

	if(m_carpeta_inicial == "-")
		m_carpeta_activa = "-";
	else
	{
		//Cargar el nombre de la carpeta inicial
		std::vector<std::vector<std::string>> ruta_carpetas = m_datos->carpetas();
		for(unsigned int i=0; i<ruta_carpetas.size(); i++)
		{
			if(m_carpeta_inicial == ruta_carpetas[i][1])
				m_ruta_exploracion.carpeta_extra(ruta_carpetas[i][0], ruta_carpetas[i][1]);
		}
	}

	if(m_carpeta_activa == "-")
		this->crear_tabla("-");
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
	std::vector<std::vector<std::string>> ruta_carpetas = m_datos->carpetas();
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
	for(const std::filesystem::directory_entry &elemento : std::filesystem::directory_iterator(ruta_abrir))
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
			actual.es_carpeta = elemento.is_directory();
			actual.fecha_acceso = "-";

			if(!elemento.is_directory())
			{
				actual.nombre = Texto::primera_letra_mayuscula(nombre_archivo);
				std::vector<std::string> datos_midi = m_datos->datos_archivo(actual.ruta);
				actual.tamanno = elemento.file_size();

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
			{
				actual.nombre = nombre_archivo;
				actual.tamanno = Funciones::numero_de_archivos(actual.ruta);//Numero de archivos
			}

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
		Registro::Depurar("Abriendo la carpeta: " + ruta_abrir);

		//Almacena la carpeta raiz para retroceder
		if(m_es_carpeta_inicial)
		{
			m_carpeta_inicial = ruta_abrir;
			m_configuracion->carpeta_inicial(ruta_abrir);
			m_es_carpeta_inicial = false;

			for(unsigned long int m = 0; m<m_lista_archivos.size(); m++)
			{
				if(ruta_abrir == m_lista_archivos[m].ruta)
					m_ruta_exploracion.carpeta_extra(m_lista_archivos[m].nombre, ruta_abrir);
			}

		}
		m_carpeta_activa = ruta_abrir;
		m_configuracion->carpeta_activa(ruta_abrir);

		//Limpia la lista de archivos
		m_lista_archivos.clear();

		this->cargar_contenido_carpeta(ruta_abrir);
	}
	else if(ruta_abrir != "-")
		Notificacion::Aviso("La carpeta no existe", 5);
	else
	{
		//Limpia la lista de archivos
		m_lista_archivos.clear();

		m_es_carpeta_inicial = true;
		m_carpeta_inicial = "-";
		m_carpeta_activa = "-";
		m_configuracion->carpeta_inicial(m_carpeta_inicial);
		m_configuracion->carpeta_activa(m_carpeta_activa);

		this->cargar_lista_carpetas();
	}
	m_ruta_exploracion.ruta(m_carpeta_inicial, m_carpeta_activa);


	if(m_lista_archivos.size() == 0)
	{
		if(m_es_carpeta_inicial)
		{
			Notificacion::Aviso("No hay carpetas donde buscar archivos MIDI", 5);
			Notificacion::Nota("Agrega carpetas en la configuración", 15);
		}
		else
			Notificacion::Aviso("Esta carpeta no contiene archivos MIDI", 5);
		m_ruta_exploracion.siguiente_habilitado(false);
		return;
	}
	else
		m_ruta_exploracion.siguiente_habilitado(true);

	//Ordenar Lista
	std::sort(m_lista_archivos.begin(), m_lista_archivos.end());

	//Crear Tabla
	for(unsigned int i=0; i<m_lista_archivos.size(); i++)
	{
		std::vector<std::string> fila_nueva;

		fila_nueva.push_back(m_lista_archivos[i].nombre);

		if(m_lista_archivos[i].es_carpeta)
		{
			fila_nueva.push_back("-");//Las carpetas no tienen duracion
			fila_nueva.push_back("-");//No se cuentan las visitas a las carpetas

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
			fila_nueva.push_back(std::to_string(m_lista_archivos[i].visitas));
			fila_nueva.push_back(Texto::bytes_a_texto(m_lista_archivos[i].tamanno));
		}
		fila_nueva.push_back(m_lista_archivos[i].fecha_acceso);

		m_tabla_archivos.insertar_fila(fila_nueva);
	}
	//Recupera la fila seleccionada anteriormente si fue guardada
	std::vector<std::string> seleccionado_anterior = m_datos->leer_ultima_seleccion(ruta_abrir);
	if(seleccionado_anterior.size() == 2)
	{
		unsigned long int seleccion_guardada = std::stoul(seleccionado_anterior[0]);
		if(seleccion_guardada < m_lista_archivos.size() && m_lista_archivos[seleccion_guardada].ruta == seleccionado_anterior[1])
		{
			//El archivo seleccionado no cambio de posicion en la tabla
			m_tabla_archivos.seleccionar(seleccion_guardada);
		}
		else
		{
			//El archivo seleccionado cambio de posicion en la tabla por lo que se busca donde quedo
			//si aun existe
			bool posicion_encontrada = false;
			unsigned long int x=0;
			while(x<m_lista_archivos.size() && !posicion_encontrada)
			{
				if(m_lista_archivos[x].ruta == seleccionado_anterior[1])
				{
					//Archivo encontrado
					m_tabla_archivos.seleccionar(x);
					posicion_encontrada = true;
				}
				x++;
			}
		}
	}
}

void VentanaSeleccionMusica::guardar_carpeta_actual()
{
	unsigned long int seleccion_actual = m_tabla_archivos.obtener_seleccion();
	if(seleccion_actual < m_lista_archivos.size())
	{
		//Guarda el archivo seleccionado en esta carpeta
		m_datos->guardar_ultima_seleccion(m_carpeta_activa, seleccion_actual, m_lista_archivos[seleccion_actual].ruta);
	}
}

bool VentanaSeleccionMusica::abrir_archivo_seleccionado()
{
	unsigned long int seleccion_actual = m_tabla_archivos.obtener_seleccion();
	if(seleccion_actual < m_lista_archivos.size())
	{
		//Guarda el archivo seleccionado en esta carpeta
		m_datos->guardar_ultima_seleccion(m_carpeta_activa, seleccion_actual, m_lista_archivos[seleccion_actual].ruta);
		if(m_lista_archivos[seleccion_actual].es_carpeta)
		{
			//Abre la carpeta seleccionada
			std::string ruta_nueva = m_lista_archivos[seleccion_actual].ruta;

			//Carga la lista de archivos de la carpeta seleccionada
			this->crear_tabla(ruta_nueva);
			return false;
		}
		else
		{
			//Abre el archivo seleccionado
			Datos_Archivos &archivo_abierto = m_lista_archivos[seleccion_actual];
			Registro::Depurar("Abriendo archivo: " + archivo_abierto.ruta);
			if(m_musica->cargar_midi(archivo_abierto.ruta))
			{
				//Se suma la visita y se actualiza la fecha
				m_datos->sumar_visita_archivo(archivo_abierto.ruta);

				//Se actualiza la duracion si el archivo cambia
				if(m_musica->musica()->GetSongLengthInMicroseconds() != archivo_abierto.duracion)
					m_datos->actualizar_archivo(archivo_abierto.ruta, m_musica->musica()->GetSongLengthInMicroseconds());

				//El formato del nombre se considera: Nombre autor - Nombre musica
				std::vector<std::string> nombres = Texto::dividir_texto(archivo_abierto.nombre, '-');
				if(nombres.size() == 0)
				{
					//Archivo sin nombre, solo espacios
					m_musica->nombre_musica("Sin Nombre");
					m_musica->autor("");
				}
				if(nombres.size() == 1)
				{
					//Archivo con solo el nombre de la musica
					m_musica->nombre_musica(Texto::quitar_espacios_en_extremos(nombres[0]));
					m_musica->autor("");
				}
				else if(nombres.size() > 1)
				{
					//Archivo con el nombre de autor y nombre de la musica
					m_musica->autor(Texto::quitar_espacios_en_extremos(nombres[0]));
					//Unir todos los demas textos
					std::string nombre_final;
					for(unsigned long int x=1; x<nombres.size(); x++)
					{
						if(x==1)
							nombre_final = nombres[x];
						else
							nombre_final += "-"+nombres[x];
					}
					m_musica->nombre_musica(Texto::quitar_espacios_en_extremos(nombre_final));
				}
				return true;
			}
			else
				Notificacion::Error("No se puede abrir el archivo MIDI", 5);
			return false;
		}
	}
	return false;
}

void VentanaSeleccionMusica::evento_raton(Raton *raton)
{
	m_ruta_exploracion.evento_raton(raton);
	if(m_ruta_exploracion.siguiente())
	{
		if(this->abrir_archivo_seleccionado())
			m_accion = CambiarASeleccionPista;
	}

	if(m_ruta_exploracion.cambiar_carpeta())
	{
		this->guardar_carpeta_actual();
		this->crear_tabla(m_ruta_exploracion.nueva_ruta());
	}

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
	{
		this->guardar_carpeta_actual();
		m_accion = CambiarATitulo;
	}
	else if(m_boton_continuar->esta_activado())
	{
		if(this->abrir_archivo_seleccionado())
			m_accion = CambiarASeleccionPista;
	}
}

void VentanaSeleccionMusica::evento_teclado(Tecla tecla, bool estado)
{
	if(tecla == TECLA_ESCAPE && !estado)
	{
		this->guardar_carpeta_actual();
		m_accion = CambiarATitulo;
	}
	else if((tecla == TECLA_ENTRAR || tecla == TECLA_FLECHA_DERECHA) && !estado)
	{
		if(this->abrir_archivo_seleccionado())
			m_accion = CambiarASeleccionPista;
	}
	else if((tecla == TECLA_BORRAR || tecla == TECLA_FLECHA_IZQUIERDA) && !estado)
	{
		this->guardar_carpeta_actual();
		m_ruta_exploracion.atras();
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

	//Actualiza la seleccion en caso que al achicar el tamaño de la ventana
	//vaya a quedar fuera de la tabla.
	m_tabla_archivos.seleccionar(m_tabla_archivos.obtener_seleccion());
}
