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
	m_teclado_util_original.cambiar(21, 88);

	std::string ruta_base_de_datos = Usuario::carpeta_juego() + "concertista.db";
	if(std::ifstream(ruta_base_de_datos))
	{
		//Se abre la base de datos existente
		Registro::Depurar("Abriendo la base de datos en: " + ruta_base_de_datos);
		m_datos.abrir(ruta_base_de_datos);
		m_datos.actualizar();//Actualiza la base de datos

		//Configuracion MIDI
		Dispositivo_Midi *dispositivo1 = m_controlador_midi.configurar_dispositivo(129, 2, ENTRADA, "Teclado y Ratón");
		dispositivo1->capacidad_activa(ENTRADA);
		dispositivo1->habilitado(true);
		m_controlador_midi.conectar(dispositivo1, false);//Habilita el dispositivo

		Dispositivo_Midi *dispositivo2 = m_controlador_midi.configurar_dispositivo(128, 0, SALIDA, "TiMidity");
		dispositivo2->capacidad_activa(SALIDA);
		dispositivo2->habilitado(true);
		m_controlador_midi.conectar(dispositivo2, false);//Habilita el dispositivo

		Dispositivo_Midi *dispositivo3 = m_controlador_midi.configurar_dispositivo(24, 0, ENTRADA | SALIDA, "CASIO USB-MIDI");
		dispositivo3->teclas_luminosas(17);
		dispositivo3->rango_teclado("21,88");
		dispositivo3->volumen_entrada(1.0f);
		dispositivo3->volumen_salida(1.0f);
		dispositivo3->sensitivo(false);
		dispositivo3->habilitado(true);
		m_controlador_midi.conectar(dispositivo3, false);//Habilita el dispositivo

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
	m_teclado_util = m_teclado_util_original;
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
	if(m_teclado_util_original != m_teclado_util)
		m_datos.escribir_configuracion("teclado_util", m_teclado_util.texto());
	m_datos.finalizar_transaccion();
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
