#include "configuracion.h++"

Configuracion::Configuracion()
{
	//Configuracion General
	m_pantalla_completa_original = false;

	//Configuracion Archivo
	m_carpeta_inicial_original = "-";
	m_carpeta_activa_original = "-";

	//Configuracion Reproduccion
	m_volumen_original = 1.0;
	m_velocidad_original = 1.0;
	m_duracion_nota_original = 6500;
	m_subtitulos_original = true;
	m_teclado_visible_original.cambiar(21, 88);

	std::string ruta_base_de_datos = Usuario::carpeta_juego() + "concertista.db";
	if(std::ifstream(ruta_base_de_datos))
	{
		//Se abre la base de datos existente
		Registro::Depurar("Abriendo la base de datos en: " + ruta_base_de_datos);
		m_datos.abrir(ruta_base_de_datos);
		m_datos.actualizar();//Actualiza la base de datos

		//Configuracion General
		m_pantalla_completa_original = this->leer("pantalla_completa", m_pantalla_completa_original);

		//Configuracion Archivo
		m_carpeta_inicial_original = this->leer("carpeta_inicial", m_carpeta_inicial_original);
		m_carpeta_activa_original = this->leer("carpeta_activa", m_carpeta_activa_original);

		//Configuracion Reproduccion
		m_volumen_original = this->leer("volumen", m_volumen_original);
		m_velocidad_original = this->leer("velocidad", m_velocidad_original);
		m_duracion_nota_original = this->leer("duracion_nota", m_duracion_nota_original);
		m_subtitulos_original = this->leer("subtitulos", m_subtitulos_original);
		m_teclado_visible_original.cargar(this->leer("teclado_visible", std::string("21,88")));
	}
	else
	{
		//Se crea la base de datos si no existe
		Registro::Depurar("Creando la base de datos en: " + ruta_base_de_datos);
		m_datos.abrir(ruta_base_de_datos);
		m_datos.crear();
	}

	//Se cargan los dispositivos disponibles
	std::vector<Dispositivo_Midi> lista_dispositivos = m_datos.lista_dispositivos();
	for(Dispositivo_Midi &datos : lista_dispositivos)
	{
		if(datos.habilitado())
		{
			Dispositivo_Midi *dispositivo = m_controlador_midi.configurar_dispositivo(datos.cliente(), datos.puerto(), datos.capacidad(), datos.nombre());

			//Si cambio el id del cliente se actualiza la base de datos
			if(dispositivo->cambio_cliente())
				m_datos.actualizar_cliente_dispositivo(datos.cliente(), *dispositivo);

			//Carga la configuracion de la base de datos
			dispositivo->copiar_configuracion(datos);
			m_controlador_midi.conectar(dispositivo);
		}
	}
	this->actualizar_rango_util_organo();

	//Se guarda la configuracion original para poder ver cual se ha modificado
	//Configuracion General
	m_pantalla_completa = m_pantalla_completa_original;

	//Configuracion Archivo
	m_carpeta_inicial = m_carpeta_inicial_original;
	m_carpeta_activa = m_carpeta_activa_original;

	//Configuracion Reproduccion
	this->volumen(m_volumen_original);
	this->velocidad(m_velocidad_original);
	this->duracion_nota(m_duracion_nota_original);
	m_subtitulos = m_subtitulos_original;
	m_teclado_visible = m_teclado_visible_original;
}

Configuracion::~Configuracion()
{
}

Base_de_Datos* Configuracion::base_de_datos()
{
	return &m_datos;
}
std::string Configuracion::leer(std::string atributo, std::string predeterminado)
{
	std::string respuesta = m_datos.leer_configuracion(atributo);
	if(respuesta != "")
		return respuesta;
	return predeterminado;
}

unsigned int Configuracion::leer(std::string atributo, unsigned int predeterminado)
{
	std::string respuesta = m_datos.leer_configuracion(atributo);
	if(respuesta != "")
		return static_cast<unsigned int>(std::stoi(respuesta));
	return predeterminado;
}

double Configuracion::leer(std::string atributo, double predeterminado)
{
	std::string respuesta = m_datos.leer_configuracion(atributo);
	if(respuesta != "")
		return static_cast<double>(std::stod(respuesta));
	return predeterminado;
}

bool Configuracion::leer(std::string atributo, bool predeterminado)
{
	std::string respuesta = m_datos.leer_configuracion(atributo);
	if(respuesta == "verdadero")
		return true;
	else if (respuesta == "falso")
		return false;
	return predeterminado;
}

void Configuracion::guardar_configuracion()
{
	//Escribe la configuracion cambiada, usar al salir
	//Configuracion MIDI
	m_datos.iniciar_transaccion();

	//Configuracion General
	if(m_pantalla_completa_original != m_pantalla_completa)
	{
		if(m_pantalla_completa)
			m_datos.escribir_configuracion("pantalla_completa", "verdadero");
		else
			m_datos.escribir_configuracion("pantalla_completa", "falso");
	}

	//Configuracion Archivo
	if(m_carpeta_inicial_original != m_carpeta_inicial)
		m_datos.escribir_configuracion("carpeta_inicial", m_carpeta_inicial);
	if(m_carpeta_activa_original != m_carpeta_activa)
		m_datos.escribir_configuracion("carpeta_activa", m_carpeta_activa);

	//Configuracion Reproduccion
	if(!Funciones::comparar_double(m_volumen_original, m_volumen, 0.001))
		m_datos.escribir_configuracion("volumen", std::to_string(m_volumen));
	if(!Funciones::comparar_double(m_velocidad_original, m_velocidad, 0.001))
		m_datos.escribir_configuracion("velocidad", std::to_string(m_velocidad));
	if(m_duracion_nota_original != m_duracion_nota)
		m_datos.escribir_configuracion("duracion_nota", std::to_string(m_duracion_nota));
	if(m_subtitulos_original != m_subtitulos)
	{
		if(m_subtitulos)
			m_datos.escribir_configuracion("subtitulos", "verdadero");
		else
			m_datos.escribir_configuracion("subtitulos", "falso");
	}
	if(m_teclado_visible_original != m_teclado_visible)
		m_datos.escribir_configuracion("teclado_visible", m_teclado_visible.texto());
	m_datos.finalizar_transaccion();
}

void Configuracion::actualizar_rango_util_organo()
{
	std::vector<Dispositivo_Midi*> dispositivos = m_controlador_midi.lista_dispositivos();

	//Tamaño minimo para quedarme con el mayor de todos los rangos
	if(dispositivos.size() > 0)
		m_teclado_util.cambiar(0, 24);//El menor de todos

	bool nuevo_rango = false;
	for(Dispositivo_Midi *d : dispositivos)
	{
		if(d->habilitado())
		{
			//Se queda con el teclado mas grande
			if(d->conectado() && d->es_entrada() && m_teclado_util <= d->rango_teclado())
			{
				m_teclado_util.cambiar(d->rango_teclado().tecla_inicial(), d->rango_teclado().numero_teclas());
				nuevo_rango = true;
			}
		}
	}
	//Si no hay dispositivo de entrada o no estan habilitados se muestra una rango predeterminado
	if(!nuevo_rango)
		m_teclado_util.cambiar(21, 88);
}

Controlador_Midi *Configuracion::controlador_midi()
{
	return &m_controlador_midi;
}

//Configuracion General
void Configuracion::pantalla_completa(bool estado)
{
	m_pantalla_completa = estado;
}

bool Configuracion::pantalla_completa()
{
	return m_pantalla_completa;
}

//Configuracion Archivo
void Configuracion::carpeta_inicial(const std::string& carpeta)
{
	m_carpeta_inicial = carpeta;
}

void Configuracion::carpeta_activa(const std::string& carpeta)
{
	m_carpeta_activa = carpeta;
}

std::string Configuracion::carpeta_inicial()
{
	return m_carpeta_inicial;
}

std::string Configuracion::carpeta_activa()
{
	return m_carpeta_activa;
}

//Configuracion Reproduccion
void Configuracion::volumen(double valor)
{
	if(valor < 0)
		m_volumen = 0;
	else if(valor > 2)
		m_volumen = 2;
	else
		m_volumen = valor;
}

void Configuracion::velocidad(double valor)
{
	if(valor <= 0)
		m_velocidad = 0.01;
	else if(valor > 2)
		m_velocidad = 2;
	else
		m_velocidad = valor;
}

void Configuracion::duracion_nota(unsigned int duracion)
{
	if(duracion < 1500)
		m_duracion_nota = 1500;
	else if(duracion > 14000)
		m_duracion_nota = 14000;
	else
		m_duracion_nota = duracion;
}

void Configuracion::subtitulos(bool estado)
{
	m_subtitulos = estado;
}

void Configuracion::teclado_visible(unsigned char inicial, unsigned char largo)
{
	m_teclado_visible.cambiar(inicial, largo);
}

double Configuracion::volumen()
{
	return m_volumen;
}

double Configuracion::velocidad()
{
	return m_velocidad;
}

unsigned int Configuracion::duracion_nota()
{
	return m_duracion_nota;
}

bool Configuracion::subtitulos()
{
	return m_subtitulos;
}

Rango_Organo Configuracion::teclado_visible()
{
	return m_teclado_visible;
}

Rango_Organo Configuracion::teclado_util()
{
	return m_teclado_util;
}

void Configuracion::posicion_ventana(int *x, int *y)
{
	std::string posicion = m_datos.leer_configuracion("posicion_ventana");
	bool correcto = false;
	if(posicion != "")
	{
		std::vector<std::string> datos = Texto::dividir_texto(posicion, ',');
		if(datos.size() > 1)
		{
			*x = std::stoi(datos[0]);
			*y = std::stoi(datos[1]);
			correcto = true;
		}
	}
	if(!correcto)
	{
		*x = 0;
		*y = 0;
	}
}

void Configuracion::dimension_ventana(int *ancho, int *alto)
{
	std::string dimension = m_datos.leer_configuracion("dimension_ventana");
	bool correcto = false;
	if(dimension != "")
	{
		std::vector<std::string> datos = Texto::dividir_texto(dimension, 'x');
		if(datos.size() > 1)
		{
			*ancho = std::stoi(datos[0]);
			*alto = std::stoi(datos[1]);
			correcto = true;
		}
	}
	if(!correcto)
	{
		*ancho = ANCHO;
		*alto = ALTO;
	}
}

void Configuracion::guardar_posicion_ventana(int x, int y)
{
	m_datos.escribir_configuracion("posicion_ventana", std::to_string(x) + "," + std::to_string(y));
}

void Configuracion::guardar_dimension_ventana(int ancho, int alto)
{
	m_datos.escribir_configuracion("dimension_ventana", std::to_string(ancho) + "x" + std::to_string(alto));
}
