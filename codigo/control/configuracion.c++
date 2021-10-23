#include "configuracion.h++"

Configuracion::Configuracion() : m_entrada(NULL), m_salida(NULL), m_teclas_luminosas(NULL)
{
	//Configuracion MIDI
	m_id_entrada_original = 0;
	m_id_salida_original = 0;
	m_id_teclas_luminosas_original = 0;

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
	m_teclado_util_original.cambiar(21, 88);

	std::string ruta_base_de_datos = Usuario::carpeta_personal() + ".concertista.db";
	if(std::ifstream(ruta_base_de_datos))
	{
		//Se abre la base de datos existente
		Registro::Depurar("Abriendo la base de datos en: " + ruta_base_de_datos);
		m_datos.abrir(ruta_base_de_datos);
		m_datos.actualizar();//Actualiza la base de datos

		//Configuracion MIDI
		m_id_entrada_original = this->leer("dispositivo_entrada", m_id_entrada_original);
		m_id_salida_original = this->leer("dispositivo_salida", m_id_salida_original);
		m_id_teclas_luminosas_original = this->leer("teclas_luminosas", m_id_teclas_luminosas_original);

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
		m_teclado_util_original.cargar(this->leer("teclado_util", std::string("21,88")));
	}
	else
	{
		//Se crea la base de datos si no existe
		Registro::Depurar("Creando la base de datos en: " + ruta_base_de_datos);
		m_datos.abrir(ruta_base_de_datos);
		m_datos.crear();
	}
	//Configuracion Midi
	this->dispositivo_entrada(m_id_entrada_original);
	this->dispositivo_salida(m_id_salida_original);
	m_teclas_luminosas = TeclasLuminosas::Cargar_tecla_luminosa(m_id_teclas_luminosas_original);

	//Se guarda la configuracion original para poder ver cual se ha modificado
	//Configuracion General
	m_pantalla_completa = m_pantalla_completa_original;

	//Configuracion Archivo
	m_carpeta_inicial = m_carpeta_inicial_original;
	m_carpeta_activa = m_carpeta_activa_original;

	//Configuracion Reproduccion
	m_volumen = m_volumen_original;
	m_velocidad = m_velocidad_original;
	m_duracion_nota = m_duracion_nota_original;
	m_subtitulos_original = m_subtitulos;
	m_teclado_visible = m_teclado_visible_original;
	m_teclado_util = m_teclado_util_original;
}

Configuracion::~Configuracion()
{
	if(m_entrada != NULL)
		delete m_entrada;
	if(m_salida != NULL)
		delete m_salida;
	midiStop();
	if(m_teclas_luminosas != NULL)
		delete m_teclas_luminosas;
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
	if(m_id_entrada_original != m_id_entrada)
		m_datos.escribir_configuracion("dispositivo_entrada", std::to_string(m_id_entrada));
	if(m_id_salida_original != m_id_salida)
		m_datos.escribir_configuracion("dispositivo_salida", std::to_string(m_id_salida));
	if(m_id_teclas_luminosas_original != m_teclas_luminosas->identificador())
		m_datos.escribir_configuracion("teclas_luminosas", std::to_string(m_teclas_luminosas->identificador()));

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
			m_datos.escribir_configuracion("pantalla_completa", "verdadero");
		else
			m_datos.escribir_configuracion("pantalla_completa", "falso");
	}
	if(m_teclado_visible_original != m_teclado_visible)
		m_datos.escribir_configuracion("teclado_visible", m_teclado_visible.texto());
	if(m_teclado_util_original != m_teclado_util)
		m_datos.escribir_configuracion("teclado_util", m_teclado_util.texto());
	m_datos.finalizar_transaccion();
}

//Configuracion MIDI
void Configuracion::dispositivo_entrada(unsigned int id_entrada)
{
	MidiCommDescriptionList dispositivos_entrada = MidiCommIn::GetDeviceList();
	if(dispositivos_entrada.size() > 0)
	{
		m_id_entrada = 0;
		if(id_entrada < dispositivos_entrada.size())
			m_id_entrada = id_entrada;

		Registro::Aviso("Conectado al dispositivo de entrada: " + dispositivos_entrada[m_id_entrada].name);
		if(m_entrada != NULL)
			delete m_entrada;
		m_entrada = new MidiCommIn(m_id_entrada);
	}
	else
		Notificacion::Error("No se pudo conectar al dispositivo MIDI de entrada", 10);
}

void Configuracion::dispositivo_salida(unsigned int id_salida)
{
	MidiCommDescriptionList dispositivos_salida = MidiCommOut::GetDeviceList();
	if(dispositivos_salida.size() > 0)
	{
		m_id_salida = 0;
		if(id_salida < dispositivos_salida.size())
			m_id_salida = id_salida;

		Registro::Aviso("Conectado al dispositivo de salida: " + dispositivos_salida[m_id_salida].name);
		if(m_salida != NULL)
			delete m_salida;
		m_salida = new MidiCommOut(m_id_salida);
	}
	else
		Notificacion::Error("No se pudo conectar al dispositivo MIDI de salida", 10);
}

void Configuracion::teclas_luminosas(unsigned int identificador)
{
	delete m_teclas_luminosas;
	m_teclas_luminosas = TeclasLuminosas::Cargar_tecla_luminosa(identificador);;
}

unsigned int Configuracion::id_dispositivo_entrada()
{
	return m_id_entrada;
}

unsigned int Configuracion::id_dispositivo_salida()
{
	return m_id_salida;
}

MidiCommIn *Configuracion::dispositivo_entrada()
{
	return m_entrada;
}

MidiCommOut *Configuracion::dispositivo_salida()
{
	return m_salida;
}

TeclasLuminosas *Configuracion::teclas_luminosas()
{
	return m_teclas_luminosas;
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
	else if(m_volumen > 2)
		m_volumen = 2;
	else
		m_volumen = valor;
}

void Configuracion::velocidad(double valor)
{
	if(valor <= 0)
		m_velocidad = 0.01;
	else if(m_velocidad > 2)
		m_velocidad = 2;
	else
		m_velocidad = valor;
}

void Configuracion::duracion_nota(unsigned int duracion)
{
	m_duracion_nota = duracion;
}

void Configuracion::subtitulos(bool estado)
{
	m_subtitulos = estado;
}

void Configuracion::teclado_visible(unsigned int inicial, unsigned int largo)
{
	m_teclado_visible.cambiar(inicial, largo);
}

void Configuracion::teclado_util(unsigned int inicial, unsigned int largo)
{
	m_teclado_util.cambiar(inicial, largo);
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

Teclado_Organo Configuracion::teclado_visible()
{
	return m_teclado_visible;
}

Teclado_Organo Configuracion::teclado_util()
{
	return m_teclado_util;
}
