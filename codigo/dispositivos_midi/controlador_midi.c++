#include "controlador_midi.h++"

unsigned int Controlador_Midi::Bandera_entrada = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ;
unsigned int Controlador_Midi::Bandera_salida = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;

Controlador_Midi::Controlador_Midi()
{
	m_estado = snd_seq_open(&m_secuenciador_alsa, "default", SND_SEQ_OPEN_DUPLEX, 0);
	m_estado = snd_config_update_free_global();//Libera memoria de la configuración

	m_cliente = snd_seq_client_id(m_secuenciador_alsa);

	if(m_estado < 0)
		m_secuenciador_alsa = NULL;

	m_estado = snd_seq_set_client_name(m_secuenciador_alsa, "Concertista MIDI");//Nombre del cliente
	//Crea el puerto de entrada
	//Bandera_salida porque la entrada se conecta a la salida del otro dispositivo
	m_puerto_entrada = snd_seq_create_simple_port(m_secuenciador_alsa, "Concertista MIDI Entrada", Bandera_salida, SND_SEQ_PORT_TYPE_MIDI_GENERIC);

	//Crea el puerto de salida
	//Bandera_entrada porque la salida se conecta a la entrada del otro dispositivo
	m_puerto_salida = snd_seq_create_simple_port(m_secuenciador_alsa, "Concertista MIDI Salida", Bandera_entrada, SND_SEQ_PORT_TYPE_MIDI_GENERIC);

	//Crea el puerto para el teclado y ratón
	//Bandera_entrada porque la salida se conecta a la entrada del otro dispositivo
	m_puerto_virtual = snd_seq_create_simple_port(m_secuenciador_alsa, "Teclado y Ratón", Bandera_entrada, SND_SEQ_PORT_TYPE_MIDI_GENERIC);

	//Crea el puerto de anuncio
	unsigned int bandera_anuncio = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_NO_EXPORT;
	unsigned int bandera_tipo_anuncio = SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION;
	int puerto_anuncio = snd_seq_create_simple_port(m_secuenciador_alsa, "Concertista MIDI Anuncio", bandera_anuncio, bandera_tipo_anuncio);

	this->suscribir_puerto(SND_SEQ_CLIENT_SYSTEM, SND_SEQ_PORT_SYSTEM_ANNOUNCE, m_cliente, puerto_anuncio);
	this->crear_lista_dispositivos();
	m_cambio_dispositivos = false;
}

Controlador_Midi::~Controlador_Midi()
{
	snd_seq_close(m_secuenciador_alsa);

	for(unsigned long int x=0; x<m_dispositivos.size(); x++)
		delete m_dispositivos[x];
}

void Controlador_Midi::suscribir_puerto(int cliente_origen, int puerto_origen, int cliente_destino, int puerto_destino)
{
	//Suscribirse al puerto de anuncio para detectar nuevos dispositivos MIDI
	snd_seq_addr_t origen, destino;
	origen.client = static_cast<unsigned char>(cliente_origen);
	origen.port = static_cast<unsigned char>(puerto_origen);

	destino.client = static_cast<unsigned char>(cliente_destino);
	destino.port = static_cast<unsigned char>(puerto_destino);

	snd_seq_port_subscribe_t *suscripcion;
	m_estado = snd_seq_port_subscribe_malloc(&suscripcion);
	snd_seq_port_subscribe_set_sender(suscripcion, &origen);//Establece Origen
	snd_seq_port_subscribe_set_dest(suscripcion, &destino);//Establece Destino
	m_estado = snd_seq_subscribe_port(m_secuenciador_alsa, suscripcion);//Se suscribe al puerto de anuncio
	free(suscripcion);
}

void Controlador_Midi::crear_lista_dispositivos()
{
	snd_seq_client_info_t* informacion_cliente;
	snd_seq_port_info_t* informacion_puerto;
	int cliente, puerto;
	unsigned int capacidad = 0;
	std::string nombre;

	m_estado = snd_seq_client_info_malloc(&informacion_cliente);
	m_estado = snd_seq_port_info_malloc(&informacion_puerto);

	snd_seq_client_info_set_client(informacion_cliente, -1);
	//Recorre todos los clientes
	while(snd_seq_query_next_client(m_secuenciador_alsa, informacion_cliente) >= 0)
	{
		cliente = snd_seq_client_info_get_client(informacion_cliente);
		snd_seq_port_info_set_client(informacion_puerto, cliente);
		snd_seq_port_info_set_port(informacion_puerto, -1);

		//Se omiten Timer y Announce que son puertos del sistema
		if(cliente == SND_SEQ_CLIENT_SYSTEM)
			continue;

		//Recorre todos los puertos del cliente actual
		while(snd_seq_query_next_port(m_secuenciador_alsa, informacion_puerto) >= 0)
		{
			puerto = snd_seq_port_info_get_port(informacion_puerto);
			//Se omiten los puertos propios de entrada y salida
			if(cliente == m_cliente)
				if(puerto == m_puerto_entrada || puerto == m_puerto_salida)
					continue;

			//Solo dispositivos de entrada o salida
			capacidad = snd_seq_port_info_get_capability(informacion_puerto);

			unsigned char capacidad_dispositivo = 0;
			if((capacidad & Bandera_entrada) == Bandera_entrada)
				capacidad_dispositivo = ENTRADA;
			if((capacidad & Bandera_salida) == Bandera_salida)
				capacidad_dispositivo = capacidad_dispositivo | SALIDA;

			if(capacidad_dispositivo > 0)
			{
				if(cliente != m_cliente)
				{
					nombre = snd_seq_client_info_get_name(informacion_cliente);
					m_dispositivos.push_back(new Dispositivo_Midi(cliente, puerto, capacidad_dispositivo, nombre, true));
				}
				else
				{
					//Caso espcial, para el teclado y raton el nombre esta en el puerto
					nombre = snd_seq_port_info_get_name(informacion_puerto);
					m_dispositivos.push_back(new Dispositivo_Midi(cliente, puerto, capacidad_dispositivo, nombre, true));
				}
			}
		}
	}
	free(informacion_cliente);
	free(informacion_puerto);
}

void Controlador_Midi::agregar_nuevo_dispositivo(int cliente, int puerto)
{
	snd_seq_client_info_t* informacion_cliente;
	snd_seq_port_info_t* informacion_puerto;

	/*int estado = */snd_seq_client_info_malloc(&informacion_cliente);
	/*estado = */snd_seq_port_info_malloc(&informacion_puerto);

	snd_seq_get_any_client_info(m_secuenciador_alsa, cliente, informacion_cliente);
	snd_seq_get_any_port_info(m_secuenciador_alsa, cliente, puerto, informacion_puerto);

	unsigned int capacidad = snd_seq_port_info_get_capability(informacion_puerto);

	unsigned char capacidad_dispositivo = 0;
	if((capacidad & Bandera_entrada) == Bandera_entrada)
		capacidad_dispositivo = ENTRADA;
	if((capacidad & Bandera_salida) == Bandera_salida)
		capacidad_dispositivo = capacidad_dispositivo | SALIDA;

	if(capacidad_dispositivo > 0)
	{
		std::string nombre_cliente = snd_seq_client_info_get_name(informacion_cliente);
		std::string nombre_puerto = snd_seq_port_info_get_name(informacion_puerto);
		m_dispositivos.push_back(new Dispositivo_Midi(cliente, puerto, capacidad_dispositivo, nombre_cliente, true));
	}

	free(informacion_cliente);
	free(informacion_puerto);
}

bool Controlador_Midi::hay_eventos()
{
	if(snd_seq_event_input_pending(m_secuenciador_alsa, 1) > 0)
		return true;
	return false;
}

std::string Controlador_Midi::nombre_dispositivo(int cliente, int puerto) const
{
	std::string nombre;
	if(cliente != m_cliente)
	{
		snd_seq_client_info_t* informacion_cliente;
		/*int estado = */snd_seq_client_info_malloc(&informacion_cliente);
		snd_seq_get_any_client_info(m_secuenciador_alsa, cliente, informacion_cliente);
		nombre = snd_seq_client_info_get_name(informacion_cliente);
		free(informacion_cliente);
	}
	else
	{
		//Nombres de puertos del programa
		snd_seq_port_info_t* informacion_puerto;
		/*int estado = */snd_seq_port_info_malloc(&informacion_puerto);
		snd_seq_get_any_port_info(m_secuenciador_alsa, cliente, puerto, informacion_puerto);
		nombre = snd_seq_port_info_get_name(informacion_puerto);
		free(informacion_puerto);
	}

	return nombre;
}

Dispositivo_Midi *Controlador_Midi::dispositivo_activo(int cliente, int puerto, unsigned char capacidad)
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
Dispositivo_Midi *Controlador_Midi::dispositivo(int cliente, int puerto, bool conectado, Habilitado habilitado, bool exacto)
{
	std::string nombre = nombre_dispositivo(cliente, puerto);
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

void Controlador_Midi::reenviar_programas(Dispositivo_Midi *dispositivo)
{
	unsigned char *datos;
	for(std::pair<unsigned char, unsigned char> programa : m_ultimo_programa)
	{
		datos = new unsigned char[2];
		datos[0] = programa.first;
		datos[1] = programa.second;
		escribir(Evento_Midi(EventoMidi_CambioPrograma, datos, 2), dispositivo);
	}
}

Evento_Midi Controlador_Midi::leer()
{
	if(!this->hay_eventos())
		return Evento_Midi();

	snd_seq_event_t* evento;
	snd_seq_event_input(m_secuenciador_alsa, &evento);
	unsigned char *datos;

	if(evento->type == SND_SEQ_EVENT_NOTEOFF)
	{
		datos = new unsigned char[3];
		datos[0] = evento->data.note.channel;
		datos[1] = evento->data.note.note;

		//Ajusta la velocidad de acurdo a la configuracion de cada dispositivo de entrada
		unsigned char velocidad = evento->data.note.velocity;
		Dispositivo_Midi *origen = dispositivo_activo(evento->source.client, evento->source.port, ENTRADA);
		if(origen != NULL)
			velocidad = static_cast<unsigned char>(velocidad * origen->volumen_entrada());

		datos[2] = velocidad;
		return Evento_Midi(EventoMidi_NotaApagada, datos, 3);
	}
	else if(evento->type == SND_SEQ_EVENT_NOTEON)
	{
		datos = new unsigned char[3];
		datos[0] = evento->data.note.channel;
		datos[1] = evento->data.note.note;

		//Ajusta la velocidad de acurdo a la configuracion de cada dispositivo de entrada
		unsigned char velocidad = evento->data.note.velocity;
		Dispositivo_Midi *origen = dispositivo_activo(evento->source.client, evento->source.port, ENTRADA);
		if(origen != NULL)
		{
			if(!origen->sensitivo())
				velocidad = static_cast<unsigned char>(VELOCIDAD_NORMAL * origen->volumen_entrada());
			else
			{
				velocidad = static_cast<unsigned char>(velocidad * origen->volumen_entrada());
			}
		}

		//Si fuera 0 se transforma en un evento de nota apagada
		if(velocidad == 0)
			velocidad = 1;

		datos[2] = velocidad;
		return Evento_Midi(EventoMidi_NotaEncendida, datos, 3);
	}
	else if(evento->type == SND_SEQ_EVENT_PGMCHANGE)
	{
		datos = new unsigned char[2];
		datos[0] = evento->data.note.channel;
		datos[1] = static_cast<unsigned char>(evento->data.control.value);
		return Evento_Midi(EventoMidi_CambioPrograma, datos, 2);
	}
	else if(evento->type == SND_SEQ_EVENT_CLIENT_START)
	{
		m_cambio_dispositivos = true;
		m_mensajes.push_back(nombre_dispositivo(evento->data.addr.client, evento->data.addr.port) + " - conectado");
	}
	else if(evento->type == SND_SEQ_EVENT_CLIENT_EXIT)
	{
		m_cambio_dispositivos = true;
		Dispositivo_Midi *nuevo = dispositivo(evento->data.addr.client, evento->data.addr.port, false, Indeterminado, true);
		if(nuevo != NULL)
			m_mensajes.push_back(nuevo->nombre() + " - desconectado");
		else
			m_mensajes.push_back(" - desconectado");
	}
	else if(evento->type == SND_SEQ_EVENT_PORT_START)
	{
		//Agrega nuevo puerto a la lista
		Dispositivo_Midi *nuevo = dispositivo(evento->data.addr.client, evento->data.addr.port, false, Activo, false);
		if(nuevo != NULL)
			conectar(nuevo, true);
		else
			agregar_nuevo_dispositivo(evento->data.addr.client, evento->data.addr.port);
	}
	else if(evento->type == SND_SEQ_EVENT_PORT_EXIT)
	{
		//Desactiva un puerto de la lista
		Dispositivo_Midi *origen = dispositivo(evento->data.addr.client, evento->data.addr.port, true, Indeterminado, true);
		if(origen != NULL)
			desconectar(origen, true);
	}
	else if(evento->type == SND_SEQ_EVENT_CONTROLLER)//10
	{
		Registro::Depurar("Evento Controlador: " + nombre_dispositivo(evento->source.client, evento->source.port));
	}
	else if(evento->type == SND_SEQ_EVENT_PORT_SUBSCRIBED)//66
	{
		if(evento->data.addr.client != SND_SEQ_CLIENT_SYSTEM && evento->data.addr.client != m_cliente)
		{
			if(m_ultimo_programa.size() > 0)
			{
				Dispositivo_Midi *dispositivo = dispositivo_activo(evento->data.addr.client, evento->data.addr.port, SALIDA);
				if(dispositivo != NULL && dispositivo->reenviar_programa())
				{
					reenviar_programas(dispositivo);
					dispositivo->reenviar_programa(false);
				}
			}
		}
	}
	else if(evento->type == SND_SEQ_EVENT_SYSEX)//130
	{
		unsigned char *d = static_cast<unsigned char*>(evento->data.ext.ptr);
		for(unsigned long int x=0; x<evento->data.ext.len; x++)
		{
			std::cout << std::hex << static_cast<unsigned int>(d[x]) << "-";
		}
		std::cout << "\n" << std::flush;
	}

	return Evento_Midi();
}

void Controlador_Midi::escribir(const Evento_Midi &evento_entrada, Dispositivo_Midi *dispositivo)
{
	//Termina si recive un evento vacio
	if(evento_entrada.datos() == NULL || evento_entrada.largo_datos() == 0)
		return;

	snd_seq_event_t evento;
	snd_seq_ev_clear(&evento);

	evento.source.port = static_cast<unsigned char>(m_puerto_salida);
	evento.dest.client = static_cast<unsigned char>(dispositivo->cliente());//SND_SEQ_ADDRESS_SUBSCRIBERS
	evento.dest.port = static_cast<unsigned char>(dispositivo->puerto());//SND_SEQ_ADDRESS_UNKNOWN
	evento.queue = SND_SEQ_QUEUE_DIRECT;

	if(evento_entrada.tipo_evento() == EventoMidi_NotaApagada)
	{
		evento.type = SND_SEQ_EVENT_NOTEOFF;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
		evento.data.note.channel = evento_entrada.canal();
		evento.data.note.note = static_cast<unsigned char>(evento_entrada.id_nota());
		evento.data.note.velocity = static_cast<unsigned char>(evento_entrada.velocidad());
		dispositivo->desactivar_tecla(evento_entrada.canal(), evento_entrada.id_nota());
	}
	else if(evento_entrada.tipo_evento() == EventoMidi_NotaEncendida)
	{
		evento.type = SND_SEQ_EVENT_NOTEON;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
		evento.data.note.channel = evento_entrada.canal();
		evento.data.note.note = static_cast<unsigned char>(evento_entrada.id_nota());
		evento.data.note.velocity = static_cast<unsigned char>(evento_entrada.velocidad() * dispositivo->volumen_salida());
		dispositivo->activar_tecla(evento_entrada.canal(), evento_entrada.id_nota());
	}
	else if(evento_entrada.tipo_evento() == EventoMidi_CambioPrograma)
	{
		evento.type = SND_SEQ_EVENT_PGMCHANGE;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
		evento.data.control.channel = evento_entrada.canal();
		evento.data.control.value = evento_entrada.programa();
	}
	else if(evento_entrada.tipo_evento() == EventoMidi_SistemaExclusivo)
	{
		evento.type = SND_SEQ_EVENT_SYSEX;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_VARIABLE;
		evento.data.ext.len = static_cast<unsigned int>(evento_entrada.largo_datos());
		evento.data.ext.ptr = evento_entrada.datos();
	}

	m_estado = snd_seq_event_output(m_secuenciador_alsa, &evento);
	m_estado = snd_seq_drain_output(m_secuenciador_alsa);
	if(m_estado < 0)
	{
		//Se eliminan todos los eventos que no se enviaron correctamente
		snd_seq_drop_output_buffer(m_secuenciador_alsa);
	}
}

void Controlador_Midi::escribir(const MidiEvent &evento_entrada, Dispositivo_Midi *dispositivo)
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
		this->escribir(Evento_Midi(EventoMidi_NotaApagada, datos, 3), dispositivo);
	}
	else if(evento_entrada.Type() == MidiEventType_NoteOn)
	{
		datos = new unsigned char[3];
		datos[0] = evento_entrada.Channel();
		datos[1] = static_cast<unsigned char>(evento_entrada.NoteNumber());
		datos[2] = static_cast<unsigned char>(evento_entrada.NoteVelocity());
		this->escribir(Evento_Midi(EventoMidi_NotaEncendida, datos, 3), dispositivo);
	}
	else if(evento_entrada.Type() == MidiEventType_ProgramChange)
	{
		datos = new unsigned char[2];
		datos[0] = evento_entrada.Channel();
		datos[1] = static_cast<unsigned char>(evento_entrada.ProgramNumber());
		this->escribir(Evento_Midi(EventoMidi_CambioPrograma, datos, 2), dispositivo);
	}
	else
	{/*
		if(evento_entrada.Type() == MidiEventType_Meta)
			Registro::Depurar("\tMetaevento: " + GetMidiMetaEventTypeDescription(evento_entrada.MetaType()) + "\n");
		else
			Registro::Depurar("\tEvento: " + GetMidiEventTypeDescription(evento_entrada.Type()) + "\n");*/
	}
}

void Controlador_Midi::escribir(const Evento_Midi &evento_entrada)
{
	//Guarda el ultimo programa del canal
	if(evento_entrada.tipo_evento() == EventoMidi_CambioPrograma)
		m_ultimo_programa[evento_entrada.canal()] = evento_entrada.programa();

	for(unsigned long x=0; x<m_salida.size(); x++)
		this->escribir(evento_entrada, m_salida[x]);
}

void Controlador_Midi::escribir(const MidiEvent &evento_entrada)
{
	//NOTE Este es un metodo provisorio hasta reemplazar libmidi por completo
	//Guarda el ultimo programa del canal
	if(evento_entrada.Type() == MidiEventType_ProgramChange)
		m_ultimo_programa[evento_entrada.Channel()] = static_cast<unsigned char>(evento_entrada.ProgramNumber());

	for(unsigned long x=0; x<m_salida.size(); x++)
		this->escribir(evento_entrada, m_salida[x]);
}

void Controlador_Midi::tecla_luninosa(unsigned char id_nota, bool estado)
{
	for(unsigned long int x=0; x<m_salida.size(); x++)
	{
		Teclas_Luminosas *teclado_actual = m_salida[x]->teclas_luminosas();
		if(teclado_actual != NULL)
		{
			if(estado)
				this->escribir(teclado_actual->encender(id_nota), m_salida[x]);
			else
				this->escribir(teclado_actual->apagar(id_nota), m_salida[x]);
		}
	}
}

void Controlador_Midi::enviar_nota(unsigned char id_nota, bool estado)
{
	snd_seq_event_t evento;
	snd_seq_ev_clear(&evento);

	evento.source.port = static_cast<unsigned char>(m_puerto_virtual);
	evento.dest.client = SND_SEQ_ADDRESS_SUBSCRIBERS;
	evento.dest.port = SND_SEQ_ADDRESS_UNKNOWN;
	evento.queue = SND_SEQ_QUEUE_DIRECT;

	if(estado)
		evento.type = SND_SEQ_EVENT_NOTEON;
	else
		evento.type = SND_SEQ_EVENT_NOTEOFF;

	evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
	evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
	evento.data.note.channel = 0;
	evento.data.note.note = id_nota;
	evento.data.note.velocity = VELOCIDAD_NORMAL;

	m_estado = snd_seq_event_output(m_secuenciador_alsa, &evento);
	m_estado = snd_seq_drain_output(m_secuenciador_alsa);
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
				this->escribir(teclado_actual->actualizar(diferencia_tiempo), m_salida[x]);
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
			escribir(Evento_Midi(EventoMidi_CambioPrograma, datos, 2));
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
				this->escribir(teclado_actual->apagar_siguiente(), m_salida[x]);
		}

		snd_seq_event_t evento;
		snd_seq_ev_clear(&evento);

		evento.source.port = static_cast<unsigned char>(m_puerto_salida);
		evento.dest.client = static_cast<unsigned char>(m_salida[x]->cliente());
		evento.dest.port = static_cast<unsigned char>(m_salida[x]->puerto());
		evento.queue = SND_SEQ_QUEUE_DIRECT;
		evento.type = SND_SEQ_EVENT_NOTEOFF;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
		evento.data.note.velocity = 0;

		std::map<unsigned char, std::vector<unsigned char>> teclas_activas = m_salida[x]->teclas_activas();
		for(std::pair<unsigned char, std::vector<unsigned char>> canal_actual : teclas_activas)
		{
			evento.data.note.channel = canal_actual.first;
			for(unsigned long int n=0; n<canal_actual.second.size(); n++)
			{
				evento.data.note.note = canal_actual.second[n];
				snd_seq_event_output(m_secuenciador_alsa, &evento);
				snd_seq_drain_output(m_secuenciador_alsa);
				//Se eliminan todos los eventos que no se enviaron correctamente
				if(m_estado < 0)
					snd_seq_drop_output_buffer(m_secuenciador_alsa);
				m_salida[x]->desactivar_tecla(canal_actual.first, canal_actual.second[n]);
			}
		}
		m_salida[x]->limpiar();
	}
}

Dispositivo_Midi *Controlador_Midi::configurar_dispositivo(int cliente, int puerto, unsigned char capacidad, const std::string &nombre)
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

	if((dispositivo->modo() & ENTRADA) == ENTRADA)
	{
		m_estado = snd_seq_connect_from(m_secuenciador_alsa, m_puerto_entrada, dispositivo->cliente(), dispositivo->puerto());
		m_entrada.push_back(dispositivo);
	}
	if((dispositivo->modo() & SALIDA) == SALIDA)
	{
		m_estado = snd_seq_connect_to(m_secuenciador_alsa, m_puerto_salida, dispositivo->cliente(), dispositivo->puerto());
		dispositivo->reenviar_programa(true);
		m_salida.push_back(dispositivo);
	}
}

void Controlador_Midi::desconectar(Dispositivo_Midi *dispositivo, bool desconexion_fisica)
{
	//Deshabilita el dispositivo si no es una desconexion fisica o lo desconecta sin deshabilitar
	if(dispositivo->habilitado())
	{
		if((dispositivo->modo() & ENTRADA) == ENTRADA)
		{
			//Se desconecta el dispositivo de la entrada
			snd_seq_disconnect_from(m_secuenciador_alsa, m_puerto_entrada, dispositivo->cliente(), dispositivo->puerto());
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

		if((dispositivo->modo() & SALIDA) == SALIDA)
		{
			//Se desconecta el dispositivo de la salida
			snd_seq_disconnect_to(m_secuenciador_alsa, m_puerto_salida, dispositivo->cliente(), dispositivo->puerto());
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
