#include "ventana_configuracion.h++"

VentanaConfiguracion::VentanaConfiguracion(Configuracion *configuracion, Administrador_Recursos *recursos) : Ventana(), m_texto_titulo(recursos)
{
	m_configuracion = configuracion;

	m_recursos = recursos;
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_texto_titulo.texto("Configuración");
	m_texto_titulo.tipografia(recursos->tipografia(LetraTitulo));
	m_texto_titulo.color(Color(1.0f, 1.0f, 1.0f));
	m_texto_titulo.posicion(0, 0);
	m_texto_titulo.dimension(Pantalla::Ancho, 40);
	m_texto_titulo.centrado(true);

	m_boton_atras = new Boton(10, Pantalla::Alto - 32, 120, 25, "Atrás", LetraChica, recursos);
	m_boton_atras->color_boton(Color(0.9f, 0.9f, 0.9f));
	m_selector_archivos = NULL;

	//Pestaña de configuracion general
	m_solapa = new Panel_Solapa(0, 40, 250, Pantalla::Alto, recursos);
	m_solapa->agregar_solapa("General");
	m_solapa1_titulo = new Etiqueta(260, 50, Pantalla::Ancho-270, 40, true, "General", LetraTitulo, recursos);
	m_solapa1_texto_restablecer = new Etiqueta(260, 100, Pantalla::Ancho-270, 30, false, "Volver a la configuración predeterminada", LetraMediana, recursos);
	m_solapa1_texto_limpiar = new Etiqueta(260, 140, Pantalla::Ancho-270, 30, false, "Limpiar la base de datos", LetraMediana, recursos);
	m_solapa1_texto_borrar = new Etiqueta(260, 180, Pantalla::Ancho-270, 30, false, "Borrar la base de datos", LetraMediana, recursos);
	m_solapa1_desarrollo = new Etiqueta(260, 230, Pantalla::Ancho-270, 30, true, "Desarrollo", LetraTitulo, recursos);
	m_solapa1_restablecer = new Boton(Pantalla::Ancho - 160, 100, 150, 30, "Restablecer", LetraMediana, recursos);
	m_solapa1_limpiar_bd = new Boton(Pantalla::Ancho - 160, 140, 150, 30, "Limpiar", LetraMediana, recursos);
	m_solapa1_borrar_db = new Boton(Pantalla::Ancho - 160, 180, 150, 30, "Borrar", LetraMediana, recursos);
	m_solapa1_casilla_desarrollo = new Casilla_Verificacion(260, 280, Pantalla::Ancho-270, 30, "Modo Desarrollo (F10)", recursos);
	m_solapa1_casilla_modo_alambre = new Casilla_Verificacion(260, 320, Pantalla::Ancho-270, 30, "Modo Alambre (F12)", recursos);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_titulo);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_texto_restablecer);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_texto_limpiar);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_texto_borrar);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_restablecer);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_limpiar_bd);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_borrar_db);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_desarrollo);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_casilla_desarrollo);
	m_solapa->agregar_elemento_solapa(0, m_solapa1_casilla_modo_alambre);

	//Pestaña de configuracion de carpetas midi
	m_solapa->agregar_solapa("Carpetas MIDI");
	m_solapa2_titulo = new Etiqueta(260, 50, Pantalla::Ancho-270, 40, true, "Carpetas MIDI", LetraTitulo, recursos);
	m_solapa2_tabla = new Tabla(260, 100, Pantalla::Ancho-270, Pantalla::Alto-190, 30, recursos);
	m_solapa2_tabla->agregar_columna("Nombre", false, 1);
	m_solapa2_tabla->agregar_columna("Ruta", false, 3);
	this->cargar_tabla_carpetas();
	m_solapa2_agregar = new Boton(Pantalla::Ancho-320, Pantalla::Alto-80, 150, 30, "Agregar", LetraMediana, recursos);
	m_solapa2_eliminar = new Boton(Pantalla::Ancho-160, Pantalla::Alto-80, 150, 30, "Eliminar", LetraMediana, recursos);
	m_solapa2_eliminar->habilitado(false);//Se habilita al seleccionar una fila de la tabla
	m_solapa->agregar_elemento_solapa(1, m_solapa2_titulo);
	m_solapa->agregar_elemento_solapa(1, m_solapa2_tabla);
	m_solapa->agregar_elemento_solapa(1, m_solapa2_agregar);
	m_solapa->agregar_elemento_solapa(1, m_solapa2_eliminar);

	//Pestaña de configuracion de dispositivos midis
	m_solapa->agregar_solapa("Dispositivos");
	m_solapa3_titulo = new Etiqueta(260, 50, Pantalla::Ancho-270, 40, true, "Dispositivos", LetraTitulo, recursos);
	m_solapa3_panel = new Panel_Desplazamiento(260, 100, Pantalla::Ancho-270, Pantalla::Alto-150, 10, recursos);

	Controlador_Midi *controlador = m_configuracion->controlador_midi();
	for(unsigned long int x=0; x<controlador->lista_dispositivos().size(); x++)
	{
		m_solapa3_lista_dispositivos.push_back(new Configuracion_Dispositivo(0, 0, Pantalla::Ancho-270, *controlador->lista_dispositivos()[x], recursos));
		m_solapa3_panel->agregar_elemento(m_solapa3_lista_dispositivos[x]);
	}

	m_solapa->agregar_elemento_solapa(2, m_solapa3_titulo);
	m_solapa->agregar_elemento_solapa(2, m_solapa3_panel);

	//Pestaña de configuracion de videos
	m_solapa->agregar_solapa("Video");
	m_solapa4_titulo = new Etiqueta(260, 50, Pantalla::Ancho-270, 40, true, "Video", LetraTitulo, recursos);
	m_solapa4_casilla_pantalla_completa = new Casilla_Verificacion(260, 100, Pantalla::Ancho-270, 30, "Pantalla Completa (F11)", recursos);
	m_solapa->agregar_elemento_solapa(3, m_solapa4_titulo);
	m_solapa->agregar_elemento_solapa(3, m_solapa4_casilla_pantalla_completa);

	//Actualiza segun el estado de la pantalla
	if(m_solapa4_casilla_pantalla_completa->activado() != Pantalla::PantallaCompleta)
		m_solapa4_casilla_pantalla_completa->estado(Pantalla::PantallaCompleta);
	if(m_solapa1_casilla_desarrollo->activado() != Pantalla::ModoDesarrollo)
		m_solapa1_casilla_desarrollo->estado(Pantalla::ModoDesarrollo);
	if(m_solapa1_casilla_modo_alambre->activado() != Pantalla::ModoAlambre)
		m_solapa1_casilla_modo_alambre->estado(Pantalla::ModoAlambre);
}

VentanaConfiguracion::~VentanaConfiguracion()
{
	delete m_boton_atras;
	if(m_selector_archivos != NULL)
		delete m_selector_archivos;

	delete m_solapa1_titulo;
	delete m_solapa1_texto_restablecer;
	delete m_solapa1_texto_limpiar;
	delete m_solapa1_texto_borrar;
	delete m_solapa1_restablecer;
	delete m_solapa1_limpiar_bd;
	delete m_solapa1_borrar_db;
	delete m_solapa1_desarrollo;
	delete m_solapa1_casilla_desarrollo;
	delete m_solapa1_casilla_modo_alambre;

	delete m_solapa2_titulo;
	delete m_solapa2_tabla;
	delete m_solapa2_agregar;
	delete m_solapa2_eliminar;

	delete m_solapa3_titulo;
	//Elimina todos los elementos de configuracion de dispositivos
	for(unsigned long int x=0; x<m_solapa3_lista_dispositivos.size(); x++)
		delete m_solapa3_lista_dispositivos[x];
	delete m_solapa3_panel;

	delete m_solapa4_titulo;
	delete m_solapa4_casilla_pantalla_completa;

	delete m_solapa;
}

void VentanaConfiguracion::cargar_tabla_carpetas()
{
	std::vector<std::vector<std::string>> carpetas = m_configuracion->base_de_datos()->carpetas();
	for(unsigned long int c=0; c < carpetas.size(); c++)
	{
		std::vector<std::string> fila;
		for(unsigned long int f=0; f < carpetas[c].size(); f++)
		{
			fila.push_back(carpetas[c][f]);
		}
		m_solapa2_tabla->insertar_fila(fila);
	}
}

unsigned int VentanaConfiguracion::limpiar_base_de_datos()
{
	unsigned int registros_eliminados = 0;
	m_configuracion->base_de_datos()->iniciar_transaccion();
	std::vector<std::string> archivos = m_configuracion->base_de_datos()->lista_archivos();
	for(unsigned int x=0; x<archivos.size(); x++)
	{
		//Archivos que ya no existen, movidos o renombrados
		if(!std::ifstream(archivos[x]))
		{
			//NOTE Agregar las demas tablas una vez que esten implementadas
			m_configuracion->base_de_datos()->borrar_archivo(archivos[x]);
			Registro::Nota("El archivo no existe: " + archivos[x]);
			registros_eliminados++;
		}
	}
	std::vector<std::string> carpetas = m_configuracion->base_de_datos()->lista_seleccion();
	for(unsigned int x=0; x<carpetas.size(); x++)
	{
		if(!std::ifstream(carpetas[x]) && carpetas[x] != "-")
		{
			m_configuracion->base_de_datos()->borrar_seleccion(carpetas[x]);
			Registro::Nota("La carpeta no existe: " + carpetas[x]);
			registros_eliminados++;
		}
	}

	std::vector<std::vector<std::string>> carpetas_de_busqueda = m_configuracion->base_de_datos()->carpetas();
	bool cambio_lista = false;
	for(unsigned int x=0; x<carpetas_de_busqueda.size(); x++)
	{
		if(!std::ifstream(carpetas_de_busqueda[x][1]))
		{
			m_configuracion->base_de_datos()->eliminar_carpeta(carpetas_de_busqueda[x][1]);
			Registro::Nota("La carpeta de busqueda no existe: " + carpetas_de_busqueda[x][1]);
			registros_eliminados++;
			cambio_lista = true;
		}
	}
	if(cambio_lista)
	{
		//Se vuelve a cargar la tabla de carpetas de busqueda
		this->m_solapa2_tabla->vaciar();
		this->cargar_tabla_carpetas();
	}
	m_configuracion->base_de_datos()->finalizar_transaccion();
	return registros_eliminados;
}

void VentanaConfiguracion::guardar_configuracion_dispositivos()
{
	m_configuracion->base_de_datos()->iniciar_transaccion();
	bool hay_cambios = false;
	for(unsigned long int x=0; x<m_solapa3_lista_dispositivos.size(); x++)
	{

		Configuracion_Dispositivo *actual = m_solapa3_lista_dispositivos[x];
		if(actual->dispositivo_cambiado())
		{
			hay_cambios = true;
			Dispositivo_Midi datos = actual->configuracion();
			Dispositivo_Midi *dispositivo = m_configuracion->controlador_midi()->obtener_dispositivo(datos.cliente(), datos.puerto(), datos.nombre());

			if(dispositivo != NULL)
			{
				if(dispositivo->conectado())
					m_configuracion->controlador_midi()->desconectar(dispositivo);
				else if(!datos.habilitado())//Si el dispositivo no estaba conectado y se deshabilita se elimina del controlador MIDI
					m_configuracion->controlador_midi()->eliminar_dispositivo(dispositivo);
				dispositivo->copiar_configuracion(datos);//Carga la nueva configuracion
			}
			if(datos.habilitado())
			{
				//Si existe, se borra la configuración anterior para guardar la nueva configuracion
				if(m_configuracion->base_de_datos()->existe_dispositivo(datos))
					m_configuracion->base_de_datos()->eliminar_dispositivo(datos);
				m_configuracion->base_de_datos()->agregar_dispositivo(datos);
				if(dispositivo != NULL)
					m_configuracion->controlador_midi()->conectar(dispositivo);
			}
			else
				m_configuracion->base_de_datos()->eliminar_dispositivo(datos);
		}
	}
	m_configuracion->base_de_datos()->finalizar_transaccion();
	if(hay_cambios)
		m_configuracion->actualizar_rango_util_organo();
}

void VentanaConfiguracion::actualizar(unsigned int diferencia_tiempo)
{
	if(m_selector_archivos != NULL)
		m_selector_archivos->actualizar(diferencia_tiempo);
	m_solapa->actualizar(diferencia_tiempo);

	if(m_solapa->solapa_activa() == 2)
	{
		if(m_configuracion->controlador_midi()->hay_cambios_de_dispositivos())
		{
			//Guarda los cambios de la configuracion, porque se borrara todo y se cargaran nuevamente
			this->guardar_configuracion_dispositivos();

			//Elimina todos los elementos de configuracion de dispositivos
			for(unsigned long int x=0; x<m_solapa3_lista_dispositivos.size(); x++)
				delete m_solapa3_lista_dispositivos[x];
			m_solapa3_lista_dispositivos.clear();
			m_solapa3_panel->vaciar();

			//Crea nuevamente la lista de dispositivos
			Controlador_Midi *controlador = m_configuracion->controlador_midi();
			for(unsigned long int x=0; x<controlador->lista_dispositivos().size(); x++)
			{
				m_solapa3_lista_dispositivos.push_back(new Configuracion_Dispositivo(0, 0, Pantalla::Ancho-270, *controlador->lista_dispositivos()[x], m_recursos));
				m_solapa3_panel->agregar_elemento(m_solapa3_lista_dispositivos[x]);
			}

			//Actualiza las posiciones, reenvia el evento porque se creeo todo de nuevo, normalmente esto lo hace
			//el panel m_solapa
			m_solapa3_panel->actualizar(diferencia_tiempo);
		}
		else
		{
			bool cambiar_altura = false;
			for(unsigned long int x=0; x<m_solapa3_lista_dispositivos.size() && !cambiar_altura; x++)
			{
				if(m_solapa3_lista_dispositivos[x]->cambio_altura())
					cambiar_altura = true;
			}
			if(cambiar_altura)
				m_solapa3_panel->actualizar_dimension();
		}
	}
}

void VentanaConfiguracion::dibujar()
{
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(0, 0, Pantalla::Ancho, 40, Color(0.141f, 0.624f, 0.933f));//Borde arriba
	m_rectangulo->dibujar(0, Pantalla::Alto - 40, Pantalla::Ancho, 40, Color(0.761f, 0.887f, 0.985f));//Borde abajo
	m_texto_titulo.dibujar();

	m_solapa->dibujar();
	m_boton_atras->dibujar();

	if(m_selector_archivos != NULL)
		m_selector_archivos->dibujar();
}

void VentanaConfiguracion::evento_raton(Raton *raton)
{
	if(m_selector_archivos != NULL)//Cancela los demas eventos
	{
		m_selector_archivos->evento_raton(raton);
		if(m_selector_archivos->dialogo() == Cancelar)
		{
			delete m_selector_archivos;
			m_selector_archivos = NULL;
		}
		else if(m_selector_archivos->dialogo() == Aceptar)
		{
			std::string ruta_nueva = m_selector_archivos->ruta_seleccionada();
			std::string nombre_carpeta = Funciones::nombre_archivo(ruta_nueva, true);
			if(ruta_nueva == "/")
				nombre_carpeta = "/";
			Registro::Depurar("Agregando la Carpeta: '" + nombre_carpeta + "' Ruta: '" + ruta_nueva + "'");
			m_configuracion->base_de_datos()->agregar_carpeta(nombre_carpeta, ruta_nueva);
			delete m_selector_archivos;
			m_selector_archivos = NULL;

			//Se recarga la tabla de carpetas
			m_solapa2_eliminar->habilitado(false);
			this->m_solapa2_tabla->vaciar();
			this->cargar_tabla_carpetas();
		}
		else
			return;
	}
	m_solapa->evento_raton(raton);
	m_boton_atras->evento_raton(raton);
	if(m_boton_atras->esta_activado())
	{
		//Se guardan los cambios en los dispositivos
		this->guardar_configuracion_dispositivos();
		m_accion = CambiarATitulo;
	}

	if(m_solapa->solapa_activa() == 0)
	{
		if(m_solapa1_restablecer->esta_activado())
		{
			m_configuracion->velocidad(1.0);
			m_configuracion->duracion_nota(6500);
			m_configuracion->teclado_visible(21,88);
			m_configuracion->subtitulos(true);
			Notificacion::Nota("Configuración restablecida", 5);
		}
		if(m_solapa1_limpiar_bd->esta_activado())
		{
			Notificacion::Nota("Limpiando base de datos...", 1);
			unsigned int registros_eliminados = this->limpiar_base_de_datos();
			if(registros_eliminados == 1)
				Notificacion::Nota("Se borro 1 registro huerfano", 5);
			else if(registros_eliminados > 1)
				Notificacion::Nota("Se borraron "+std::to_string(registros_eliminados)+" registros huerfanos", 5);
			else
				Notificacion::Nota("Base de datos limpia", 5);
		}
		if(m_solapa1_borrar_db->esta_activado())
		{
			m_configuracion->base_de_datos()->borrar_archivos();
			m_configuracion->base_de_datos()->borrar_selecciones();
			Notificacion::Nota("Base de datos borrada", 5);
		}
		if(m_solapa1_casilla_desarrollo->cambio_estado())
		{
			if(m_solapa1_casilla_desarrollo->activado())
				m_accion = EntrarModoDesarrollo;
			else
				m_accion = SalirModoDesarrollo;
		}
		if(m_solapa1_casilla_modo_alambre->cambio_estado())
		{
			if(m_solapa1_casilla_modo_alambre->activado())
				m_accion = EntrarModoAlambre;
			else
				m_accion = SalirModoAlambre;
		}
	}
	else if(m_solapa->solapa_activa() == 1)
	{
		if(m_solapa2_tabla->seleccion())
			m_solapa2_eliminar->habilitado(true);
		if(m_solapa2_agregar->esta_activado())
			m_selector_archivos = new Selector_Archivos(Pantalla::Ancho/2 - 600/2, Pantalla::Alto/2 - 450/2, 600, 450, "Seleccione una carpeta", Usuario::carpeta_personal(), false, m_recursos);
		else if(m_solapa2_eliminar->esta_activado())
		{
			//Borra seleccion actual y recarga la tabla
			std::vector<std::vector<std::string>> carpetas = m_configuracion->base_de_datos()->carpetas();
			unsigned long int seleccion = m_solapa2_tabla->obtener_seleccion();
			if(seleccion < carpetas.size())
			{
				Registro::Depurar("Eliminando de la lista la Carpeta: '" + carpetas[seleccion][0] + "' Ruta: '" + carpetas[seleccion][1] + "'");
				m_configuracion->base_de_datos()->eliminar_carpeta(carpetas[seleccion][1]);

				//Se recarga la tabla de carpetas
				m_solapa2_eliminar->habilitado(false);
				this->m_solapa2_tabla->vaciar();
				this->cargar_tabla_carpetas();
			}
		}
	}
	else if(m_solapa->solapa_activa() == 2)
	{
		if(raton->ultimo_evento_fue(EventoClic) && raton->numero_clics() >= 1 && !raton->activado(BotonIzquierdo))
		{
			//Cuando suelta el clic se revisa si cambio el estado del dispositivo a habilitado
			for(unsigned long int x=0; x<m_solapa3_lista_dispositivos.size(); x++)
			{
				Configuracion_Dispositivo *actual = m_solapa3_lista_dispositivos[x];
				if(actual->cambio_estado_conexion())
				{
					//Si se habilita un dispositivo, se guarda todo en caso que sea desconectado
					//Esto se hace de forma inmediata para no perder informacion, si justo despues de habilitarlo
					//es desconectado, el controlador midi eliminaria el dispositivo y no se podrian guardar los cambios
					//NO se guarda por cada cambio porque seria muy lento.
					if(actual->configuracion().habilitado())
						this->guardar_configuracion_dispositivos();
				}
			}
		}
	}
	else if(m_solapa->solapa_activa() == 3)
	{
		if(m_solapa4_casilla_pantalla_completa->cambio_estado())
		{
			if(m_solapa4_casilla_pantalla_completa->activado())
				m_accion = EntrarPantallaCompleta;
			else
				m_accion = SalirPantallaCompleta;
		}
	}
}

void VentanaConfiguracion::evento_teclado(Tecla tecla, bool estado)
{
	if(tecla == TECLA_ESCAPE && !estado)
	{
		//Se guardan los cambios en los dispositivos
		this->guardar_configuracion_dispositivos();
		m_accion = CambiarATitulo;
	}
	//Modo desarrollo activado desde teclado
	if(m_solapa1_casilla_desarrollo->activado() != Pantalla::ModoDesarrollo)
		m_solapa1_casilla_desarrollo->estado(Pantalla::ModoDesarrollo);
	if(m_solapa1_casilla_modo_alambre->activado() != Pantalla::ModoAlambre)
		m_solapa1_casilla_modo_alambre->estado(Pantalla::ModoAlambre);
	//Actualiza cuando se activa desde el teclado
	if(m_solapa4_casilla_pantalla_completa->activado() != Pantalla::PantallaCompleta)
		m_solapa4_casilla_pantalla_completa->estado(Pantalla::PantallaCompleta);

	if(m_selector_archivos != NULL)//Cancela los demas eventos
		return;

	if(m_solapa->solapa_activa() == 1)
	{
		if(tecla == TECLA_FLECHA_ABAJO && !estado)
			m_solapa2_tabla->cambiar_seleccion(1);
		else if(tecla == TECLA_FLECHA_ARRIBA && !estado)
			m_solapa2_tabla->cambiar_seleccion(-1);

		if(m_solapa2_tabla->seleccion())
			m_solapa2_eliminar->habilitado(true);
	}
}

void VentanaConfiguracion::evento_pantalla(float ancho, float alto)
{
	m_texto_titulo.dimension(ancho, 40);
	m_boton_atras->posicion(m_boton_atras->x(), alto - 32);
	if(m_selector_archivos != NULL)
		m_selector_archivos->posicion(ancho/2 - 600/2, alto/2 - 450/2);

	m_solapa->dimension(250, alto);

	m_solapa1_titulo->dimension(Pantalla::Ancho-250, 40);
	m_solapa1_restablecer->posicion(Pantalla::Ancho - 160, 100);
	m_solapa1_limpiar_bd->posicion(Pantalla::Ancho - 160, 140);
	m_solapa1_borrar_db->posicion(Pantalla::Ancho - 160, 180);
	m_solapa1_desarrollo->dimension(Pantalla::Ancho-250, 40);
	m_solapa1_casilla_desarrollo->dimension(Pantalla::Ancho-270, 30);
	m_solapa1_casilla_modo_alambre->dimension(Pantalla::Ancho-270, 30);

	m_solapa2_titulo->dimension(Pantalla::Ancho-250, 40);
	m_solapa2_tabla->dimension(Pantalla::Ancho-270, Pantalla::Alto-190);
	m_solapa2_agregar->posicion(Pantalla::Ancho-320, Pantalla::Alto-80);
	m_solapa2_eliminar->posicion(Pantalla::Ancho-160, Pantalla::Alto-80);

	m_solapa3_titulo->dimension(Pantalla::Ancho-250, 40);
	m_solapa3_panel->dimension(Pantalla::Ancho-270, Pantalla::Alto-150);
	for(Configuracion_Dispositivo *d : m_solapa3_lista_dispositivos)
		d->dimension(Pantalla::Ancho-270, 0);

	m_solapa4_titulo->dimension(Pantalla::Ancho-250, 40);
	m_solapa4_casilla_pantalla_completa->dimension(Pantalla::Ancho-270, 30);
}
