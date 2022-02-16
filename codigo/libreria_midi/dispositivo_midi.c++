#include "dispositivo_midi.h++"

Dispositivo_Midi::Dispositivo_Midi(int cliente, int puerto, unsigned char capacidad, const std::string &nombre, bool conectado)
{
	m_cliente = cliente;
	m_puerto = puerto;
	m_capacidad = capacidad;
	m_capacidad_habilitada = capacidad;
	m_nombre = nombre;
	m_conectado = conectado;
	m_habilitado = false;

	//Entrada
	m_volumen_entrada = 1.0f;
	m_sensitivo = true;
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

void Dispositivo_Midi::cliente(int cliente)
{
	m_cliente = cliente;
}

int Dispositivo_Midi::cliente()
{
	return m_cliente;
}

int Dispositivo_Midi::puerto()
{
	return m_puerto;
}

unsigned char Dispositivo_Midi::capacidad()
{
	return m_capacidad;
}

void Dispositivo_Midi::modo(unsigned char modo)
{
	if((m_capacidad & modo) == modo)
		m_capacidad_habilitada = modo;
}

unsigned char Dispositivo_Midi::modo()
{
	return m_capacidad_habilitada;
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

void Dispositivo_Midi::teclas_luminosas(unsigned int identificador)
{
	m_teclas_luminosas = Teclas_Luminosas::Cargar_tecla_luminosa(identificador);
}

Teclas_Luminosas *Dispositivo_Midi::teclas_luminosas()
{
	return m_teclas_luminosas;
}

void Dispositivo_Midi::rango_teclado(const std::string &rango)
{
	m_rango_teclado.cargar(rango);
}

Teclado_Organo Dispositivo_Midi::rango_teclado()
{
	return m_rango_teclado;
}

void Dispositivo_Midi::volumen_entrada(float valor)
{
	m_volumen_entrada = valor;
}

float Dispositivo_Midi::volumen_entrada()
{
	return m_volumen_entrada;
}

void Dispositivo_Midi::volumen_salida(float valor)
{
	m_volumen_salida = valor;
}

float Dispositivo_Midi::volumen_salida()
{
	return m_volumen_salida;
}

void Dispositivo_Midi::sensitivo(bool estado)
{
	m_sensitivo = estado;
}

bool Dispositivo_Midi::sensitivo()
{
	return m_sensitivo;
}

void Dispositivo_Midi::activar_tecla(unsigned char canal, unsigned char id_nota)
{
	m_notas_activas[canal].push_back(id_nota);
}

void Dispositivo_Midi::desactivar_tecla(unsigned char canal, unsigned char id_nota)
{
	std::vector<unsigned char> &notas_canal = m_notas_activas[canal];
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

std::map<unsigned char, std::vector<unsigned char>> Dispositivo_Midi::teclas_activas() const
{
	return m_notas_activas;
}

void Dispositivo_Midi::limpiar()
{
	m_notas_activas.clear();
}
