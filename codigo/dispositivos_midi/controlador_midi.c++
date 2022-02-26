#include "controlador_midi.h++"

Controlador_Midi::Controlador_Midi()
{
	try
	{
		m_secuenciador = new Secuenciador_Alsa();
	}
	catch(int/* error*/)
	{
		m_secuenciador = NULL;
	}

	if(m_secuenciador != NULL)
		m_secuenciador->crear_lista_dispositivos(m_dispositivos);

	m_cambio_dispositivos = false;
	m_desconectado_pendiente = 0;
}

Controlador_Midi::~Controlador_Midi()
{
	if(m_secuenciador != NULL)
		delete m_secuenciador;

	for(unsigned long int x=0; x<m_dispositivos.size(); x++)
		delete m_dispositivos[x];
}

Dispositivo_Midi *Controlador_Midi::dispositivo(unsigned char cliente, unsigned char puerto, bool conectado, Habilitado habilitado, bool exacto)
{
	if(m_secuenciador == NULL)
		return NULL;

	std::string nombre;
	if(!exacto)
		nombre = m_secuenciador->nombre_dispositivo(cliente, puerto);
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
	if(m_secuenciador == NULL)
		return;

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
	if(m_secuenciador == NULL)
		return;

	//Habilita y conecta el dispositivo si esta disponible si no solo se habilita
	if(conexion_fisica)
		dispositivo->conectado(true);
	else
		dispositivo->habilitado(true);

	if(!dispositivo->conectado() || !dispositivo->habilitado())
		return;

	if(dispositivo->entrada_activa())
	{
		if(m_secuenciador->conectar(dispositivo->cliente(), dispositivo->puerto(), Entrada))
			m_entrada.push_back(dispositivo);
	}
	if(dispositivo->salida_activa())
	{
		if(m_secuenciador->conectar(dispositivo->cliente(), dispositivo->puerto(), Salida))
		{
			dispositivo->reenviar_programa(true);
			m_salida.push_back(dispositivo);
		}
	}
}

void Controlador_Midi::desconectar(Dispositivo_Midi *dispositivo, bool desconexion_fisica, bool conexion_perdida)
{
	if(m_secuenciador == NULL)
		return;

	//Deshabilita el dispositivo si no es una desconexion fisica o lo desconecta sin deshabilitar
	if(dispositivo->habilitado())
	{
		if(dispositivo->entrada_activa())
		{
			//Se desconecta el dispositivo de la entrada
			if(!conexion_perdida)
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

		if(dispositivo->salida_activa())
		{
			//Se desconecta el dispositivo de la salida
			if(!conexion_perdida)
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
	if(m_desconectado_pendiente > 0)
		return true;

	if(m_secuenciador != NULL)
		return m_secuenciador->hay_eventos();
	return false;
}

Evento_Midi Controlador_Midi::leer()
{
	//Crea un evento para apagar las notas que dejo encendida al desconectarse
	if(m_desconectado_pendiente > 0)
	{
		for(unsigned long int x=0; x<m_dispositivos.size(); x++)
		{
			if(!m_dispositivos[x]->conectado() && m_dispositivos[x]->entrada_activa())
			{
				std::vector<unsigned char> notas_aun_activas = m_dispositivos[x]->notas_entrada();
				if(notas_aun_activas.size() > 0)
				{
					//Cuando solo queda una nota se puede recien se puede descontar de la descionexion pendiente
					//Porque solo se puede apagar una nota a la vez
					if(notas_aun_activas.size() == 1)
						m_desconectado_pendiente--;

					unsigned char *datos = new unsigned char[3];
					datos[0] = 0;
					datos[1] = notas_aun_activas[0];
					datos[2] = 0;
					Evento_Midi evento_nuevo(EventoMidi_NotaApagada, datos, 3);

					m_dispositivos[x]->nota_entrada(notas_aun_activas[0], false);
					return evento_nuevo;
				}
			}
		}

	}

	if(m_secuenciador == NULL)
		return Evento_Midi();

	Evento_Midi evento = m_secuenciador->leer();
	if(evento.tipo_evento() == EventoMidi_NotaApagada)
	{
		Dispositivo_Midi *origen = dispositivo_activo(evento.cliente(), evento.puerto(), ENTRADA);
		if(origen == NULL)
			return evento;

		origen->nota_entrada(evento.id_nota(), false);

		if(origen->volumen_entrada() < 0.999f || origen->volumen_entrada() > 1.001f)
			evento.velocidad(static_cast<unsigned char>(evento.velocidad() * origen->volumen_entrada()));
	}
	else if(evento.tipo_evento() == EventoMidi_NotaEncendida)
	{
		Dispositivo_Midi *origen = dispositivo_activo(evento.cliente(), evento.puerto(), ENTRADA);
		if(origen == NULL)
			return evento;

		origen->nota_entrada(evento.id_nota(), true);

		unsigned char velocidad = evento.velocidad();
		if(!origen->sensitivo())
			velocidad = VELOCIDAD_NORMAL;

		if(origen->volumen_entrada() < 0.999f || origen->volumen_entrada() > 1.001f || !origen->sensitivo())
			evento.velocidad(static_cast<unsigned char>(velocidad * origen->volumen_entrada()));
	}
	else if(evento.tipo_evento() == EventoMidi_ClienteConectado)
	{
		m_cambio_dispositivos = true;
		m_mensajes.push_back(m_secuenciador->nombre_dispositivo(evento.cliente(), evento.puerto()) + " - conectado");
	}
	else if(evento.tipo_evento() == EventoMidi_ClienteDesconectado)
	{
		//Primero se desconecta el puerto y luego el cliente, cuando llega a este punto ya esta desconectado el puerto
		m_cambio_dispositivos = true;
		Dispositivo_Midi *nuevo = dispositivo(evento.cliente(), evento.puerto(), false, Indeterminado, true);
		if(nuevo != NULL)
			m_mensajes.push_back(nuevo->nombre() + " - desconectado");
		else
			m_mensajes.push_back(" - desconectado");
	}
	else if(evento.tipo_evento() == EventoMidi_PuertoConectado)
	{
		Dispositivo_Midi *nuevo = dispositivo(evento.cliente(), evento.puerto(), false, Activo, false);
		if(nuevo != NULL)
			conectar(nuevo, true);
		else
		{
			Dispositivo_Midi *nuevo_dispositivo = m_secuenciador->crear_nuevo_dispositivo(evento.cliente(), evento.puerto());
			if(nuevo_dispositivo != NULL)
				m_dispositivos.push_back(nuevo_dispositivo);
		}
	}
	else if(evento.tipo_evento() == EventoMidi_PuertoDesconectado)
	{
		//Desactiva un puerto de la lista
		Dispositivo_Midi *origen = dispositivo(evento.cliente(), evento.puerto(), true, Indeterminado, true);
		if(origen != NULL)
		{
			//Revisa si el dispositivo que se desconecto dejo notas encendidas que hay que apagar
			if(origen->notas_entrada().size() > 0)
				m_desconectado_pendiente++;
			desconectar(origen, true, true);
		}
	}
	else if(evento.tipo_evento() == EventoMidi_PuertoSuscrito)
	{
		if(evento.cliente() != m_secuenciador->cliente())
		{
			if(m_ultimo_programa.size() > 0)
			{
				Dispositivo_Midi *dispositivo = dispositivo_activo(evento.cliente(), evento.puerto(), SALIDA);
				if(dispositivo != NULL && dispositivo->reenviar_programa())
				{
					reenviar_programas(dispositivo);
					dispositivo->reenviar_programa(false);
				}
			}
		}
	}
	return evento;
}

void Controlador_Midi::escribir(Evento_Midi &evento_salida)
{
	if(m_secuenciador == NULL)
		return;

	//Guarda el ultimo programa del canal
	if(evento_salida.tipo_evento() == EventoMidi_CambioPrograma)
		m_ultimo_programa[evento_salida.canal()] = evento_salida.programa();

	//Solo contiene la velocidad si es un evento de NotaEncendida o NotaApagada
	unsigned char velocidad = evento_salida.velocidad();

	for(unsigned long x=0; x<m_salida.size(); x++)
	{
		evento_salida.cliente(m_salida[x]->cliente());
		evento_salida.puerto(m_salida[x]->puerto());
		if(	evento_salida.tipo_evento() == EventoMidi_NotaApagada)
		{
			if(m_salida[x]->volumen_salida() < 0.999f || m_salida[x]->volumen_salida() > 1.001f)
				evento_salida.velocidad(static_cast<unsigned char>(velocidad * m_salida[x]->volumen_salida()));
			m_salida[x]->nota_salida(evento_salida.canal(), evento_salida.id_nota(), false);
		}
		else if(evento_salida.tipo_evento() == EventoMidi_NotaEncendida)
		{
			if(m_salida[x]->volumen_salida() < 0.999f || m_salida[x]->volumen_salida() > 1.001f)
				evento_salida.velocidad(static_cast<unsigned char>(velocidad * m_salida[x]->volumen_salida()));
			m_salida[x]->nota_salida(evento_salida.canal(), evento_salida.id_nota(), true);
		}
		m_secuenciador->escribir(evento_salida);
	}
}

void Controlador_Midi::escribir(MidiEvent &evento_salida)
{
	//NOTE Este es un metodo provisorio hasta reemplazar libmidi por completo
	//Reenviar el MidiEvent al nuevo tipo Evento_Midi
	unsigned char *datos;
	if(evento_salida.Type() == MidiEventType_NoteOff)
	{
		datos = new unsigned char[3];
		datos[0] = evento_salida.Channel();
		datos[1] = static_cast<unsigned char>(evento_salida.NoteNumber());
		datos[2] = static_cast<unsigned char>(evento_salida.NoteVelocity());
		Evento_Midi evento(EventoMidi_NotaApagada, datos, 3);
		this->escribir(evento);
	}
	else if(evento_salida.Type() == MidiEventType_NoteOn)
	{
		datos = new unsigned char[3];
		datos[0] = evento_salida.Channel();
		datos[1] = static_cast<unsigned char>(evento_salida.NoteNumber());
		datos[2] = static_cast<unsigned char>(evento_salida.NoteVelocity());
		Evento_Midi evento(EventoMidi_NotaEncendida, datos, 3);
		this->escribir(evento);
	}
	else if(evento_salida.Type() == MidiEventType_ProgramChange)
	{
		datos = new unsigned char[2];
		datos[0] = evento_salida.Channel();
		datos[1] = static_cast<unsigned char>(evento_salida.ProgramNumber());
		Evento_Midi evento(EventoMidi_CambioPrograma, datos, 2);
		this->escribir(evento);
	}
	else
	{/*
		if(evento_salida.Type() == MidiEventType_Meta)
			Registro::Depurar("\tMetaevento: " + GetMidiMetaEventTypeDescription(evento_salida.MetaType()) + "\n");
		else
			Registro::Depurar("\tEvento: " + GetMidiEventTypeDescription(evento_salida.Type()) + "\n");*/
	}
}

void Controlador_Midi::tecla_luninosa(unsigned char id_nota, bool estado)
{
	if(m_secuenciador == NULL)
		return;

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
	if(m_secuenciador != NULL)
		m_secuenciador->enviar_nota(id_nota, estado);
}

void Controlador_Midi::actualizar(unsigned int diferencia_tiempo, bool consumir_eventos)
{
	if(m_secuenciador == NULL)
		return;

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
	if(m_secuenciador == NULL)
		return;

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
	if(m_secuenciador == NULL)
		return;

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
	bool cambios = m_cambio_dispositivos;
	m_cambio_dispositivos = false;
	return cambios;
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
