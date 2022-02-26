#include "dispositivo_midi.h++"

Dispositivo_Midi::Dispositivo_Midi(unsigned char cliente, unsigned char puerto, unsigned char capacidad, const std::string &nombre, bool conectado)
{
	m_cliente = cliente;
	m_puerto = puerto;
	m_capacidad = capacidad;
	m_capacidad_activa = capacidad;
	m_nombre = nombre;

	m_conectado = conectado;
	m_habilitado = false;
	m_reenviar_programa = false;

	//Entrada
	m_sensitivo = true;
	m_volumen_entrada = 1.0f;
	m_rango_teclado.cambiar(21, 88);

	//Salida
	m_volumen_salida = 1.0f;
	m_teclas_luminosas = NULL;
}

Dispositivo_Midi::~Dispositivo_Midi()
{
	if(m_teclas_luminosas != NULL)
		delete m_teclas_luminosas;
}

void Dispositivo_Midi::cliente(unsigned char cliente)
{
	m_cliente = cliente;
}

unsigned char Dispositivo_Midi::cliente()
{
	return m_cliente;
}

unsigned char Dispositivo_Midi::puerto()
{
	return m_puerto;
}

void Dispositivo_Midi::capacidad_activa(unsigned char capacidad)
{
	if((m_capacidad & capacidad) == capacidad)
		m_capacidad_activa = capacidad;
}

std::string Dispositivo_Midi::nombre()
{
	return m_nombre;
}

void Dispositivo_Midi::conectado(bool estado)
{
	m_conectado = estado;
}

bool Dispositivo_Midi::conectado()
{
	return m_conectado;
}

void Dispositivo_Midi::habilitado(bool estado)
{
	m_habilitado = estado;
}

bool Dispositivo_Midi::habilitado()
{
	return m_habilitado;
}

void Dispositivo_Midi::reenviar_programa(bool valor)
{
	m_reenviar_programa = valor;
}

bool Dispositivo_Midi::reenviar_programa()
{
	return m_reenviar_programa;
}

bool Dispositivo_Midi::es_entrada()
{
	if((m_capacidad & ENTRADA) == ENTRADA)
		return true;
	return false;
}

bool Dispositivo_Midi::entrada_activa()
{
	if((m_capacidad_activa & ENTRADA) == ENTRADA)
		return true;
	return false;
}

void Dispositivo_Midi::sensitivo(bool estado)
{
	m_sensitivo = estado;
}

bool Dispositivo_Midi::sensitivo()
{
	return m_sensitivo;
}

void Dispositivo_Midi::volumen_entrada(float valor)
{
	m_volumen_entrada = valor;
}

float Dispositivo_Midi::volumen_entrada()
{
	return m_volumen_entrada;
}

void Dispositivo_Midi::rango_teclado(const std::string &rango)
{
	m_rango_teclado.cargar(rango);
}

Teclado_Organo Dispositivo_Midi::rango_teclado()
{
	return m_rango_teclado;
}

void Dispositivo_Midi::nota_entrada(unsigned char id_nota, bool encendida)
{
	if(encendida)
	{
		//Nueva nota activada
		m_notas_entrada.push_back(id_nota);
	}
	else
	{
		//Nota desactivada
		for(std::vector<unsigned char>::iterator i = m_notas_entrada.begin(); i != m_notas_entrada.end(); i++)
		{
			if(*i == id_nota)
			{
				//Al eliminarlo termina
				m_notas_entrada.erase(i);
				return;
			}
		}
	}
}

std::vector<unsigned char> Dispositivo_Midi::notas_entrada() const
{
	return m_notas_entrada;
}

bool Dispositivo_Midi::es_salida()
{
	if((m_capacidad & SALIDA) == ENTRADA)
		return true;
	return false;
}

bool Dispositivo_Midi::salida_activa()
{
	if((m_capacidad_activa & SALIDA) == SALIDA)
		return true;
	return false;
}

void Dispositivo_Midi::volumen_salida(float valor)
{
	m_volumen_salida = valor;
}

float Dispositivo_Midi::volumen_salida()
{
	return m_volumen_salida;
}

void Dispositivo_Midi::teclas_luminosas(unsigned int identificador)
{
	m_teclas_luminosas = Teclas_Luminosas::Cargar_tecla_luminosa(identificador);
}

Teclas_Luminosas *Dispositivo_Midi::teclas_luminosas()
{
	return m_teclas_luminosas;
}

void Dispositivo_Midi::nota_salida(unsigned char canal, unsigned char id_nota, bool encendida)
{
	if(encendida)
	{
		//Nueva nota activada
		m_notas_salida[canal].push_back(id_nota);
	}
	else
	{
		//Nota desactivada
		std::vector<unsigned char> &notas_canal = m_notas_salida[canal];
		for(std::vector<unsigned char>::iterator i = notas_canal.begin(); i != notas_canal.end(); i++)
		{
			if(*i == id_nota)
			{
				//Al eliminarlo termina
				notas_canal.erase(i);
				return;
			}
		}
	}
}

std::map<unsigned char, std::vector<unsigned char>> Dispositivo_Midi::notas_salida() const
{
	return m_notas_salida;
}

void Dispositivo_Midi::limpiar()
{
	m_notas_salida.clear();
}
