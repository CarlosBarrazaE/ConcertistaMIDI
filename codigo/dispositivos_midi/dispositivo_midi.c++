#include "dispositivo_midi.h++"

Dispositivo_Midi::Dispositivo_Midi()
{
	this->inicializar();
}

Dispositivo_Midi::Dispositivo_Midi(const Dispositivo_Midi &dispositivo)
{
	m_cliente = dispositivo.m_cliente;
	m_puerto = dispositivo.m_puerto;
	m_capacidad = dispositivo.m_capacidad;
	m_capacidad_activa = dispositivo.m_capacidad_activa;
	m_nombre = dispositivo.m_nombre;

	m_conectado = dispositivo.m_conectado;
	m_habilitado = dispositivo.m_habilitado;
	m_reenviar_programa = dispositivo.m_reenviar_programa;

	//Entrada
	m_sensitivo = dispositivo.m_sensitivo;
	m_volumen_entrada = dispositivo.m_volumen_entrada;
	m_rango_teclado.cambiar(dispositivo.m_rango_teclado.tecla_inicial(), dispositivo.m_rango_teclado.numero_teclas());

	//Salida
	m_volumen_salida = dispositivo.m_volumen_salida;
	m_teclas_luminosas = Teclas_Luminosas::Cargar_tecla_luminosa(dispositivo.id_teclas_luminosas());
}

Dispositivo_Midi::Dispositivo_Midi(unsigned char cliente, unsigned char puerto, unsigned char capacidad, const std::string &nombre, bool conectado)
{
	this->inicializar();

	m_cliente = cliente;
	m_puerto = puerto;
	m_capacidad = capacidad;
	m_capacidad_activa = capacidad;
	m_nombre = nombre;
	m_conectado = conectado;
}

Dispositivo_Midi::~Dispositivo_Midi()
{
	if(m_teclas_luminosas != NULL)
		delete m_teclas_luminosas;
}

void Dispositivo_Midi::inicializar()
{
	m_cliente = 0;
	m_puerto = 0;
	m_capacidad = 0;
	m_capacidad_activa = 0;
	m_nombre = "Sin Nombre";

	m_conectado = false;
	m_habilitado = false;
	m_reenviar_programa = false;

	//Entrada
	m_sensitivo = true;
	m_volumen_entrada = 1.0;
	m_rango_teclado.cambiar(21, 88);

	//Salida
	m_volumen_salida = 1.0;
	m_teclas_luminosas = NULL;
}

void Dispositivo_Midi::cliente(unsigned char cliente)
{
	m_cliente = cliente;
}

unsigned char Dispositivo_Midi::cliente() const
{
	return m_cliente;
}

void Dispositivo_Midi::puerto(unsigned char puerto)
{
	m_puerto = puerto;
}

unsigned char Dispositivo_Midi::puerto() const
{
	return m_puerto;
}

void Dispositivo_Midi::capacidad(unsigned char capacidad)
{
	m_capacidad = capacidad & 0x03;
}

unsigned char Dispositivo_Midi::capacidad() const
{
	return m_capacidad;
}

void Dispositivo_Midi::capacidad_activa(unsigned char capacidad)
{
	if((m_capacidad & capacidad) == capacidad)
		m_capacidad_activa = capacidad;
}

unsigned char Dispositivo_Midi::capacidad_activa() const
{
	return m_capacidad_activa & 0x03;
}

void Dispositivo_Midi::nombre(const std::string &nombre)
{
	m_nombre = nombre;
}

std::string Dispositivo_Midi::nombre() const
{
	return m_nombre;
}

void Dispositivo_Midi::conectado(bool estado)
{
	m_conectado = estado;
}

bool Dispositivo_Midi::conectado() const
{
	return m_conectado;
}

void Dispositivo_Midi::habilitado(bool estado)
{
	m_habilitado = estado;
}

bool Dispositivo_Midi::habilitado() const
{
	return m_habilitado;
}

void Dispositivo_Midi::reenviar_programa(bool valor)
{
	m_reenviar_programa = valor;
}

bool Dispositivo_Midi::reenviar_programa() const
{
	return m_reenviar_programa;
}

bool Dispositivo_Midi::es_entrada() const
{
	if((m_capacidad & ENTRADA) == ENTRADA)
		return true;
	return false;
}

bool Dispositivo_Midi::entrada_activa() const
{
	if((m_capacidad_activa & ENTRADA) == ENTRADA)
		return true;
	return false;
}

void Dispositivo_Midi::sensitivo(bool estado)
{
	m_sensitivo = estado;
}

bool Dispositivo_Midi::sensitivo() const
{
	return m_sensitivo;
}

void Dispositivo_Midi::volumen_entrada(double valor)
{
	if(valor < 0)
		m_volumen_entrada = 0;
	else if(valor > 1.5)
		m_volumen_entrada = 1.5;
	else
		m_volumen_entrada = valor;
}

double Dispositivo_Midi::volumen_entrada() const
{
	return m_volumen_entrada;
}

void Dispositivo_Midi::rango_teclado(const std::string &rango)
{
	m_rango_teclado.cargar(rango);
}

Teclado_Organo Dispositivo_Midi::rango_teclado() const
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

bool Dispositivo_Midi::es_salida() const
{
	if((m_capacidad & SALIDA) == SALIDA)
		return true;
	return false;
}

bool Dispositivo_Midi::salida_activa() const
{
	if((m_capacidad_activa & SALIDA) == SALIDA)
		return true;
	return false;
}

void Dispositivo_Midi::volumen_salida(double valor)
{
	if(valor < 0)
		m_volumen_salida = 0;
	else if(valor > 1.5)
		m_volumen_salida = 1.5;
	else
		m_volumen_salida = valor;
}

double Dispositivo_Midi::volumen_salida() const
{
	return m_volumen_salida;
}

void Dispositivo_Midi::teclas_luminosas(unsigned int identificador)
{
	m_teclas_luminosas = Teclas_Luminosas::Cargar_tecla_luminosa(identificador);
}

Teclas_Luminosas *Dispositivo_Midi::teclas_luminosas() const
{
	return m_teclas_luminosas;
}

unsigned int Dispositivo_Midi::id_teclas_luminosas() const
{
	if(m_teclas_luminosas != NULL)
		return m_teclas_luminosas->identificador();
	return 0;
}

void Dispositivo_Midi::nota_salida(unsigned char canal, bool encendida)
{
	if(encendida)//Nueva nota activada
		m_notas_salida[canal]++;
	else if(m_notas_salida[canal] > 0)//Nota desactivada
		m_notas_salida[canal]--;
}

std::map<unsigned char, unsigned int> Dispositivo_Midi::notas_salida() const
{
	return m_notas_salida;
}

void Dispositivo_Midi::copiar_configuracion(const Dispositivo_Midi &datos)
{
	this->capacidad_activa(datos.capacidad_activa());
	this->habilitado(datos.habilitado());
	this->sensitivo(datos.sensitivo());
	this->volumen_entrada(datos.volumen_entrada());
	this->rango_teclado(datos.rango_teclado().texto());
	this->volumen_salida(datos.volumen_salida());
	this->teclas_luminosas(datos.id_teclas_luminosas());
}

void Dispositivo_Midi::limpiar()
{
	m_notas_salida.clear();
}

Dispositivo_Midi& Dispositivo_Midi::operator = (const Dispositivo_Midi &dispositivo)
{
	if(this != &dispositivo)
	{
		m_cliente = dispositivo.m_cliente;
		m_puerto = dispositivo.m_puerto;
		m_capacidad = dispositivo.m_capacidad;
		m_capacidad_activa = dispositivo.m_capacidad_activa;
		m_nombre = dispositivo.m_nombre;

		m_conectado = dispositivo.conectado();;
		m_habilitado = dispositivo.m_habilitado;
		m_reenviar_programa = dispositivo.m_reenviar_programa;

		//Entrada
		m_sensitivo = dispositivo.m_sensitivo;
		m_volumen_entrada = dispositivo.m_volumen_entrada;
		m_rango_teclado.cambiar(dispositivo.m_rango_teclado.tecla_inicial(), dispositivo.m_rango_teclado.numero_teclas());

		//Salida
		m_volumen_salida = dispositivo.m_volumen_salida;
		m_teclas_luminosas = Teclas_Luminosas::Cargar_tecla_luminosa(dispositivo.id_teclas_luminosas());
	}
	return *this;
}
