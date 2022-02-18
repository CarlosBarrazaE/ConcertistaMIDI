#include "datos_musica.h++"

Datos_Musica::Datos_Musica()
{
	m_musica = NULL;
	m_nombre_musica = "Sin Titulo";
	m_autor = "Desconocido";
}

Datos_Musica::~Datos_Musica()
{
	if(m_musica != NULL)
		delete m_musica;
}

bool Datos_Musica::cargar_midi(std::string direccion)
{
	try
	{
		if(m_musica != NULL)
			delete m_musica;
		m_musica = new Midi(Midi::ReadFromFile(direccion));
		m_evaluacion.clear();
	}
	catch(const MidiError &e)
	{
		Registro::Error(e.GetErrorDescription());
		m_musica = NULL;
		return false;
	}
	this->reiniciar();
	m_pistas.clear();
	return true;
}

void Datos_Musica::pistas(std::vector<Pista> pistas)
{
	m_pistas = pistas;
}

std::map<unsigned long int, std::vector<Tiempos_Nota>> *Datos_Musica::evaluacion()
{
	return &m_evaluacion;
}

void Datos_Musica::nombre_musica(std::string nombre)
{
	m_nombre_musica = nombre;
}

void Datos_Musica::autor(std::string autor)
{
	m_autor = autor;
}

void Datos_Musica::reiniciar()
{
	if(m_musica != NULL)
		m_musica->Reset(7000000, 1000000);
}

void Datos_Musica::reiniciar(microseconds_t espera_inicio)
{
	if(m_musica != NULL)
		m_musica->Reset(espera_inicio, 1000000);
}

Midi *Datos_Musica::musica()
{
	return m_musica;
}

std::vector<Pista> *Datos_Musica::pistas()
{
	return &m_pistas;
}

std::string Datos_Musica::nombre_musica()
{
	return m_nombre_musica;
}

std::string Datos_Musica::autor()
{
	return m_autor;
}
