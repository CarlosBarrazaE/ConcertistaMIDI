#include "controlador_midi.h++"

Controlador_Midi::Controlador_Midi()
{
	try
	{
		m_secuenciador = new Secuenciador_Alsa();
	}
	catch(int error)
	{
	}

	m_secuenciador->crear_lista_dispositivos(m_dispositivos);
}

Controlador_Midi::~Controlador_Midi()
{
	delete m_secuenciador;

	for(unsigned long int x=0; x<m_dispositivos.size(); x++)
		delete m_dispositivos[x];
}

Dispositivo_Midi *Controlador_Midi::dispositivo(unsigned char cliente, unsigned char puerto, bool conectado, Habilitado habilitado, bool exacto)
{
	std::string nombre = m_secuenciador->nombre_dispositivo(cliente, puerto);
	Dispositivo_Midi *dispositivo = NULL;
	for(unsigned long int x=0; x<m_dispositivos.size(); x++)
	{
		//El estado de conexion tiene que ser lo pedido
		if(m_dispositivos[x]->conectado() != conectado)
			continue;

		if(	(habilitado == Activo && !m_dispositivos[x]->habilitado()) ||
			(habilitado == Inactivo && m_dispositivos[x]->habilitado()))
			continue;

		//Si ya lo encotro lo retorna, usado al desconectar cuando no se tiene el nombre
		if(exacto && m_dispositivos[x]->cliente() == cliente && m_dispositivos[x]->puerto() == puerto)
			return m_dispositivos[x];

		//Aunque no se pida, se encontro el mismo y se retorna de inmediato
		//Usado al conectar, se tiene el nombre y puede haber otro dispositivo en su lugar
		if(!exacto && m_dispositivos[x]->cliente() == cliente && m_dispositivos[x]->puerto() == puerto && m_dispositivos[x]->nombre() == nombre)
			return m_dispositivos[x];

		if(!exacto && m_dispositivos[x]->puerto() == puerto && m_dispositivos[x]->nombre() == nombre)
			dispositivo = m_dispositivos[x];
	}
	if(dispositivo != NULL)
		return dispositivo;

	return NULL;
}

Dispositivo_Midi *Controlador_Midi::dispositivo_activo(unsigned char cliente, unsigned char puerto, unsigned char capacidad)
{
	//Encuentra un dispositivo que cumpla las condiciones establecidas
	if(capacidad == ENTRADA)
	{
		for(unsigned long int x=0; x<m_entrada.size(); x++)
		{
			if(m_entrada[x]->cliente() == cliente && m_entrada[x]->puerto() == puerto)
				return m_entrada[x];
		}
	}
	else if(capacidad == SALIDA)
	{
		for(unsigned long int x=0; x<m_salida.size(); x++)
		{
			if(m_salida[x]->cliente() == cliente && m_salida[x]->puerto() == puerto)
				return m_salida[x];
		}
	}
	return NULL;
}

void Controlador_Midi::reenviar_programas(Dispositivo_Midi *dispositivo)
{
	unsigned char *datos;
	for(std::pair<unsigned char, unsigned char> programa : m_ultimo_programa)
	{
		datos = new unsigned char[2];
		datos[0] = programa.first;
		datos[1] = programa.second;

		Evento_Midi evento(EventoMidi_CambioPrograma, datos, 2);
		evento.cliente(dispositivo->cliente());
		evento.puerto(dispositivo->puerto());
		m_secuenciador->escribir(evento);
	}
}

Dispositivo_Midi *Controlador_Midi::configurar_dispositivo(unsigned char cliente, unsigned char puerto, unsigned char capacidad, const std::string &nombre)
{
	Dispositivo_Midi *dispositivo = NULL;
	for(unsigned long int x=0; x<m_dispositivos.size(); x++)//Salida
	{
		if(m_dispositivos[x]->nombre() == nombre)
		{
			//Coincide al menos en el nombre
			dispositivo = m_dispositivos[x];

			//Se es exactamente el mismo lo retorna
			if(	m_dispositivos[x]->cliente() == cliente &&
				m_dispositivos[x]->puerto() == puerto)
				return dispositivo;
		}
	}
	//Retorna el dispositivo que coincide por nombre
	//pero esta en otro cliente o puerto
	if(dispositivo != NULL)
		return dispositivo;
	else
	{
		//Crea un dispositivo para guardar los datos
		dispositivo = new Dispositivo_Midi(cliente, puerto, capacidad, nombre, false);
		m_dispositivos.push_back(dispositivo);
		return dispositivo;
	}
}

void Controlador_Midi::conectar(Dispositivo_Midi *dispositivo, bool conexion_fisica)
{
	//Habilita y conecta el dispositivo si esta disponible si no solo se habilita
	if(conexion_fisica)
		dispositivo->conectado(true);
	else
		dispositivo->habilitado(true);

	if(!dispositivo->conectado() || !dispositivo->habilitado())
		return;

	if((dispositivo->capacidad_activa() & ENTRADA) == ENTRADA)
	{
		if(m_secuenciador->conectar(dispositivo->cliente(), dispositivo->puerto(), Entrada))
			m_entrada.push_back(dispositivo);
	}
	if((dispositivo->capacidad_activa() & SALIDA) == SALIDA)
	{
		if(m_secuenciador->conectar(dispositivo->cliente(), dispositivo->puerto(), Salida))
		{
			dispositivo->reenviar_programa(true);
			m_salida.push_back(dispositivo);
		}
	}
}

void Controlador_Midi::desconectar(Dispositivo_Midi *dispositivo, bool desconexion_fisica)
{
	//Deshabilita el dispositivo si no es una desconexion fisica o lo desconecta sin deshabilitar
	if(dispositivo->habilitado())
	{
		if((dispositivo->capacidad_activa() & ENTRADA) == ENTRADA)
		{
			//Se desconecta el dispositivo de la entrada
			m_secuenciador->desconectar(dispositivo->cliente(), dispositivo->puerto(), Entrada);

			bool borrado = false;
			for(unsigned int x=0; x<m_entrada.size() && !borrado; x++)
			{
				if(dispositivo == m_entrada[x])
				{
					m_entrada.erase(m_entrada.begin()+x);
					borrado = true;
				}
			}
		}

		if((dispositivo->capacidad_activa() & SALIDA) == SALIDA)
		{
			//Se desconecta el dispositivo de la salida
			m_secuenciador->desconectar(dispositivo->cliente(), dispositivo->puerto(), Salida);

			bool borrado = false;
			for(unsigned int x=0; x<m_salida.size() && !borrado; x++)
			{
				if(dispositivo == m_salida[x])
				{
					m_salida.erase(m_salida.begin()+x);
					borrado = true;
				}
			}
		}
	}

	if(desconexion_fisica)
		dispositivo->conectado(false);
	else
		dispositivo->habilitado(false);
}

bool Controlador_Midi::hay_eventos()
{
	return m_secuenciador->hay_eventos();
}

Evento_Midi Controlador_Midi::leer()
{
	return m_secuenciador->leer();
}

void Controlador_Midi::escribir(Evento_Midi &evento_entrada)
{
	//Guarda el ultimo programa del canal
	if(evento_entrada.tipo_evento() == EventoMidi_CambioPrograma)
		m_ultimo_programa[evento_entrada.canal()] = evento_entrada.programa();

	for(unsigned long x=0; x<m_salida.size(); x++)
	{
		evento_entrada.cliente(m_salida[x]->cliente());
		evento_entrada.puerto(m_salida[x]->puerto());
		//if()Que tenga velocidad entonces ajustar la velocidad
		m_secuenciador->escribir(evento_entrada);
	}
}

void Controlador_Midi::escribir(MidiEvent &evento_entrada)
{
	//NOTE Este es un metodo provisorio hasta reemplazar libmidi por completo
	//Reenviar el MidiEvent al nuevo tipo Evento_Midi
	unsigned char *datos;
	if(evento_entrada.Type() == MidiEventType_NoteOff)
	{
		datos = new unsigned char[3];
		datos[0] = evento_entrada.Channel();
		datos[1] = static_cast<unsigned char>(evento_entrada.NoteNumber());
		datos[2] = static_cast<unsigned char>(evento_entrada.NoteVelocity());
		Evento_Midi evento(EventoMidi_NotaApagada, datos, 3);
		this->escribir(evento);
	}
	else if(evento_entrada.Type() == MidiEventType_NoteOn)
	{
		datos = new unsigned char[3];
		datos[0] = evento_entrada.Channel();
		datos[1] = static_cast<unsigned char>(evento_entrada.NoteNumber());
		datos[2] = static_cast<unsigned char>(evento_entrada.NoteVelocity());
		Evento_Midi evento(EventoMidi_NotaEncendida, datos, 3);
		this->escribir(evento);
	}
	else if(evento_entrada.Type() == MidiEventType_ProgramChange)
	{
		datos = new unsigned char[2];
		datos[0] = evento_entrada.Channel();
		datos[1] = static_cast<unsigned char>(evento_entrada.ProgramNumber());
		Evento_Midi evento(EventoMidi_CambioPrograma, datos, 2);
		this->escribir(evento);
	}
	else
	{/*
		if(evento_entrada.Type() == MidiEventType_Meta)
			Registro::Depurar("\tMetaevento: " + GetMidiMetaEventTypeDescription(evento_entrada.MetaType()) + "\n");
		else
			Registro::Depurar("\tEvento: " + GetMidiEventTypeDescription(evento_entrada.Type()) + "\n");*/
	}
}

void Controlador_Midi::tecla_luninosa(unsigned char id_nota, bool estado)
{
	for(unsigned long int x=0; x<m_salida.size(); x++)
	{
		Teclas_Luminosas *teclado_actual = m_salida[x]->teclas_luminosas();
		if(teclado_actual != NULL)
		{
			if(estado)
			{
				Evento_Midi evento = teclado_actual->encender(id_nota);
				evento.cliente(m_salida[x]->cliente());
				evento.puerto(m_salida[x]->puerto());
				m_secuenciador->escribir(evento);
			}
			else
			{
				Evento_Midi evento = teclado_actual->apagar(id_nota);
				evento.cliente(m_salida[x]->cliente());
				evento.puerto(m_salida[x]->puerto());
				m_secuenciador->escribir(evento);
			}
		}
	}
}

void Controlador_Midi::enviar_nota(unsigned char id_nota, bool estado)
{
	m_secuenciador->enviar_nota(id_nota, estado);
}

void Controlador_Midi::actualizar(unsigned int diferencia_tiempo, bool consumir_eventos)
{
	if(consumir_eventos)
	{
		//Esta en una ventana que no requiere leer los eventos
		//pero igualmente necesito revisarlos para detectar nuevos
		//dispositivos o perdida de conexion
		while(this->hay_eventos())
		{
			this->leer();
		}
	}
	else
	{
		for(unsigned long int x=0; x<m_salida.size(); x++)
		{
			Teclas_Luminosas *teclado_actual = m_salida[x]->teclas_luminosas();
			if(teclado_actual != NULL)
			{
				Evento_Midi evento = teclado_actual->actualizar(diferencia_tiempo);
				evento.cliente(m_salida[x]->cliente());
				evento.puerto(m_salida[x]->puerto());
				m_secuenciador->escribir(evento);
			}
		}
	}
}

void Controlador_Midi::reiniciar()
{
	detener_eventos();

	//Restablecer programas predeterminado
	for(std::pair<unsigned char, unsigned char> programa : m_ultimo_programa)
	{
		if(programa.second != 0)
		{
			unsigned char *datos = new unsigned char[2];
			datos[0] = programa.first;
			datos[1] = 0;
			Evento_Midi evento(EventoMidi_CambioPrograma, datos, 2);
			evento.cliente(DIRECCIONES_SUSCRITAS);
			evento.puerto(DIRECCIONES_SUSCRITAS);
			m_secuenciador->escribir(evento);
		}
	}
	m_ultimo_programa.clear();
}

void Controlador_Midi::detener_eventos()
{
	//Detiene solo las notas sonando
	for(unsigned long int x=0; x<m_salida.size(); x++)
	{
		//Apaga las luces
		Teclas_Luminosas *teclado_actual = m_salida[x]->teclas_luminosas();
		if(teclado_actual != NULL)
		{
			while(teclado_actual->quedan_luces_activas())
			{
				Evento_Midi evento = teclado_actual->apagar_siguiente();
				evento.cliente(m_salida[x]->cliente());
				evento.puerto(m_salida[x]->puerto());
				m_secuenciador->escribir(evento);
			}
		}

		std::map<unsigned char, std::vector<unsigned char>> teclas_activas = m_salida[x]->notas_salida();
		unsigned char *datos = NULL;
		for(std::pair<unsigned char, std::vector<unsigned char>> canal_actual : teclas_activas)
		{
			for(unsigned long int n=0; n<canal_actual.second.size(); n++)
			{
				datos = new unsigned char[3];
				datos[0] = canal_actual.first;
				datos[1] = canal_actual.second[n];
				datos[2] = 0;

				Evento_Midi evento(EventoMidi_NotaApagada, datos, 3);
				evento.cliente(m_salida[x]->cliente());
				evento.puerto(m_salida[x]->puerto());
				m_secuenciador->escribir(evento);
			}
		}
		m_salida[x]->limpiar();
	}
}

bool Controlador_Midi::hay_mensajes()
{
	if(m_mensajes.size() > 0)
		return true;
	return false;
}

std::string Controlador_Midi::siguiente_mensaje()
{
	if(m_mensajes.size() > 0)
	{
		std::string texto = m_mensajes[m_mensajes.size()-1];
		m_mensajes.pop_back();
		return texto;
	}
	return "Sin notificacion";
}

bool Controlador_Midi::hay_cambios_de_dispositivos()
{
	return false;//NOTE Terminar
}

std::string Controlador_Midi::dispositivos_conectados(unsigned char capacidad)
{
	std::string nombres = "";
	if(capacidad == ENTRADA)
	{
		for(unsigned long int x=0; x<m_entrada.size(); x++)
		{
			if(x>0)
				nombres += ", ";
			nombres += m_entrada[x]->nombre();
		}
	}
	else if(capacidad == SALIDA)
	{
		for(unsigned long int x=0; x<m_salida.size(); x++)
		{
			if(x>0)
				nombres += ", ";
			nombres += m_salida[x]->nombre();
		}
	}
	return nombres;
}
