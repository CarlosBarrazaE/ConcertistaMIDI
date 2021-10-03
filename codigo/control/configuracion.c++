#include "configuracion.h++"

Configuracion::Configuracion() : m_entrada(NULL), m_salida(NULL)
{
	unsigned int id_dispositivo_entrada = 0;
	unsigned int id_dispositivo_salida = 0;

	std::string ruta_base_de_datos = Usuario::carpeta_personal() + ".concertista.db";
	if(std::ifstream(ruta_base_de_datos))
	{
		//Se abre la base de datos existente
		Registro::Depurar("Abriendo la base de datos en: " + ruta_base_de_datos);
		m_datos.abrir(ruta_base_de_datos);
		m_datos.actualizar();
		std::string dispositivo_entrada_texto = m_datos.leer_configuracion("dispositivo_entrada");
		if(dispositivo_entrada_texto != "")
			id_dispositivo_entrada = static_cast<unsigned int>(std::stoi(dispositivo_entrada_texto));

		std::string dispositivo_salida_texto = m_datos.leer_configuracion("dispositivo_salida");
		if(dispositivo_entrada_texto != "")
			id_dispositivo_salida = static_cast<unsigned int>(std::stoi(dispositivo_entrada_texto));
	}
	else
	{
		//Se crea la base de datos si no existe
		Registro::Depurar("Creando la base de datos en: " + ruta_base_de_datos);
		m_datos.abrir(ruta_base_de_datos);
		m_datos.crear();

		//Configuracion Inicial
		m_datos.escribir_configuracion("dispositivo_entrada", "0");
		m_datos.escribir_configuracion("dispositivo_salida", "0");
		id_dispositivo_entrada = 0;
		id_dispositivo_salida = 0;
	}

	this->dispositivo_entrada(id_dispositivo_entrada);
	this->dispositivo_salida(id_dispositivo_salida);
}

Configuracion::~Configuracion()
{
	if(m_entrada != NULL)
		delete m_entrada;
	if(m_salida != NULL)
		delete m_salida;
	midiStop();
}

Base_de_Datos* Configuracion::base_de_datos()
{
	return &m_datos;
}

std::string Configuracion::leer(std::string atributo)
{
	return m_datos.leer_configuracion(atributo);
}

bool Configuracion::escribir(std::string atributo, std::string valor)
{
	return m_datos.escribir_configuracion(atributo, valor);
}

void Configuracion::dispositivo_entrada(unsigned int id_entrada)
{
	MidiCommDescriptionList dispositivos_entrada = MidiCommIn::GetDeviceList();
	if(dispositivos_entrada.size() > 0)
	{
		unsigned int id = 0;
		if(id_entrada < dispositivos_entrada.size())
			id = id_entrada;

		Registro::Aviso("Conectado al dispositivo de entrada: " + dispositivos_entrada[id].name);
		if(m_entrada != NULL)
			delete m_entrada;
		m_entrada = new MidiCommIn(id);
	}
	else
		Notificacion::Error("No se pudo conectar al dispositivo MIDI de entrada", 10);
}

void Configuracion::dispositivo_salida(unsigned int id_salida)
{
	MidiCommDescriptionList dispositivos_salida = MidiCommOut::GetDeviceList();
	if(dispositivos_salida.size() > 0)
	{
		unsigned int id = 0;
		if(id_salida < dispositivos_salida.size())
			id = id_salida;

		Registro::Aviso("Conectado al dispositivo de salida: " + dispositivos_salida[id].name);
		if(m_salida != NULL)
			delete m_salida;
		m_salida = new MidiCommOut(id);
	}
	else
		Notificacion::Error("No se pudo conectar al dispositivo MIDI de salida", 10);
}

MidiCommIn *Configuracion::dispositivo_entrada()
{
	return m_entrada;
}

MidiCommOut *Configuracion::dispositivo_salida()
{
	return m_salida;
}

void Configuracion::reconectar()
{
	//Reconecta los dispositivos midis si es necesario
	if(m_entrada != NULL)
	{
		if(m_entrada->ShouldReconnect())
		{
			m_entrada->Reconnect();
			if(m_salida != NULL)
				m_salida->Reconnect();
		}
	}
}
