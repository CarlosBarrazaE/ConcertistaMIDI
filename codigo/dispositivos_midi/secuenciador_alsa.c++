#include "secuenciador_alsa.h++"

unsigned int Secuenciador_Alsa::Bandera_entrada = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ;
unsigned int Secuenciador_Alsa::Bandera_salida = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;

Secuenciador_Alsa::Secuenciador_Alsa()
{
	m_secuenciador_alsa = NULL;

	int estado = snd_seq_open(&m_secuenciador_alsa, "default", SND_SEQ_OPEN_DUPLEX, 0);
	if(estado < 0)
	{
		this->mostrar_estado_alsa(estado, "No se ha podido iniciar el secuenciador alsa.");
		throw 1;
	}

	snd_config_update_free_global();//Libera memoria de la configuración

	m_cliente = static_cast<unsigned char>(snd_seq_client_id(m_secuenciador_alsa));

	estado = snd_seq_set_client_name(m_secuenciador_alsa, "Concertista MIDI");//Nombre del cliente
	this->mostrar_estado_alsa(estado, "No se ha podido asignar el nombre");
	//Crea el puerto para el teclado y ratón
	//Bandera_entrada porque la salida se conecta a la entrada del otro dispositivo
	m_puerto_virtual = static_cast<unsigned char>(snd_seq_create_simple_port(m_secuenciador_alsa, "Teclado y Ratón", Bandera_entrada, SND_SEQ_PORT_TYPE_MIDI_GENERIC));

	//Crea el puerto de entrada
	//Bandera_salida porque la entrada se conecta a la salida del otro dispositivo
	m_puerto_entrada = static_cast<unsigned char>(snd_seq_create_simple_port(m_secuenciador_alsa, "Concertista MIDI Entrada", Bandera_salida, SND_SEQ_PORT_TYPE_MIDI_GENERIC));

	//Crea el puerto de salida
	//Bandera_entrada porque la salida se conecta a la entrada del otro dispositivo
	m_puerto_salida = static_cast<unsigned char>(snd_seq_create_simple_port(m_secuenciador_alsa, "Concertista MIDI Salida", Bandera_entrada, SND_SEQ_PORT_TYPE_MIDI_GENERIC));

	//Crea el puerto de anuncio
	unsigned int bandera_anuncio = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_NO_EXPORT;
	unsigned int bandera_tipo_anuncio = SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION;
	int puerto_anuncio = snd_seq_create_simple_port(m_secuenciador_alsa, "Concertista MIDI Anuncio", bandera_anuncio, bandera_tipo_anuncio);

	this->suscribir_puerto(SND_SEQ_CLIENT_SYSTEM, SND_SEQ_PORT_SYSTEM_ANNOUNCE, m_cliente, puerto_anuncio);
	m_cambio_dispositivos = false;
}

Secuenciador_Alsa::~Secuenciador_Alsa()
{
	int estado = snd_seq_close(m_secuenciador_alsa);
	this->mostrar_estado_alsa(estado, "No se ha podido detener el secuenciador alsa");
}

void Secuenciador_Alsa::mostrar_estado_alsa(int estado, const std::string &mensaje) const
{
	if(estado < 0)
		Registro::Error(mensaje + " " + strerror(estado));
}

void Secuenciador_Alsa::suscribir_puerto(int cliente_origen, int puerto_origen, int cliente_destino, int puerto_destino) const
{
	//Suscribirse al puerto de anuncio para detectar nuevos dispositivos MIDI
	snd_seq_addr_t origen, destino;
	origen.client = static_cast<unsigned char>(cliente_origen);
	origen.port = static_cast<unsigned char>(puerto_origen);

	destino.client = static_cast<unsigned char>(cliente_destino);
	destino.port = static_cast<unsigned char>(puerto_destino);

	snd_seq_port_subscribe_t *suscripcion;
	int estado = snd_seq_port_subscribe_malloc(&suscripcion);
	this->mostrar_estado_alsa(estado, "No se ha podido reservar memoria para la suscripcion");
	if(estado == 0)
	{
		snd_seq_port_subscribe_set_sender(suscripcion, &origen);//Establece Origen
		snd_seq_port_subscribe_set_dest(suscripcion, &destino);//Establece Destino
		estado = snd_seq_subscribe_port(m_secuenciador_alsa, suscripcion);//Se suscribe al puerto de anuncio
		this->mostrar_estado_alsa(estado, "No se ha podido suscibir al puerto de anuncio");

		free(suscripcion);
	}
}

std::string Secuenciador_Alsa::nombre_dispositivo(unsigned char cliente, unsigned char puerto) const
{
	std::string nombre;
	if(cliente != m_cliente)
	{
		snd_seq_client_info_t* informacion_cliente;
		int estado = snd_seq_client_info_malloc(&informacion_cliente);
		this->mostrar_estado_alsa(estado, "No se ha podido reservar memoria para la informacion del cliente - nombre dispositivo");
		if(estado == 0)
		{
			estado = snd_seq_get_any_client_info(m_secuenciador_alsa, cliente, informacion_cliente);
			this->mostrar_estado_alsa(estado, "No se ha podido obtener la informacion del cliente - nombre dispositivo");
			if(estado == 0)
				nombre = snd_seq_client_info_get_name(informacion_cliente);
			free(informacion_cliente);
		}
	}
	else
	{
		//Nombres de puertos del programa
		snd_seq_port_info_t* informacion_puerto;
		int estado = snd_seq_port_info_malloc(&informacion_puerto);
		this->mostrar_estado_alsa(estado, "No se ha podido reservar memoria para la informacion del puerto - nombre dispositivo");
		if(estado == 0)
		{
			snd_seq_get_any_port_info(m_secuenciador_alsa, cliente, puerto, informacion_puerto);
			this->mostrar_estado_alsa(estado, "No se ha podido obtener la informacion del puerto - nombre dispositivo");

			if(estado == 0)
				nombre = snd_seq_port_info_get_name(informacion_puerto);
			free(informacion_puerto);
		}
	}

	return nombre;
}

void Secuenciador_Alsa::crear_lista_dispositivos(std::vector<Dispositivo_Midi*> &dispositivos) const
{
	snd_seq_client_info_t* informacion_cliente;
	snd_seq_port_info_t* informacion_puerto;
	unsigned char cliente, puerto;
	unsigned int capacidad = 0;
	std::string nombre;

	int estado1 = snd_seq_client_info_malloc(&informacion_cliente);
	int estado2 = snd_seq_port_info_malloc(&informacion_puerto);

	this->mostrar_estado_alsa(estado1, "No se ha podido reservar memoria para la informacion del cliente - lista dispositivos");
	this->mostrar_estado_alsa(estado2, "No se ha podido reservar memoria para la informacion del puerto - lista dispositivos");
	if(estado1 < 0 || estado2 < 0)
		return;

	snd_seq_client_info_set_client(informacion_cliente, -1);
	//Recorre todos los clientes
	while(snd_seq_query_next_client(m_secuenciador_alsa, informacion_cliente) >= 0)
	{
		cliente = static_cast<unsigned char>(snd_seq_client_info_get_client(informacion_cliente));
		snd_seq_port_info_set_client(informacion_puerto, cliente);
		snd_seq_port_info_set_port(informacion_puerto, -1);

		//Se omiten Timer y Announce que son puertos del sistema
		if(cliente == SND_SEQ_CLIENT_SYSTEM)
			continue;

		//Recorre todos los puertos del cliente actual
		while(snd_seq_query_next_port(m_secuenciador_alsa, informacion_puerto) >= 0)
		{
			puerto = static_cast<unsigned char>(snd_seq_port_info_get_port(informacion_puerto));
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
					dispositivos.push_back(new Dispositivo_Midi(cliente, puerto, capacidad_dispositivo, nombre, true));
				}
				else
				{
					//Caso espcial, para el teclado y raton el nombre esta en el puerto
					nombre = snd_seq_port_info_get_name(informacion_puerto);
					dispositivos.push_back(new Dispositivo_Midi(cliente, puerto, capacidad_dispositivo, nombre, true));
				}
			}
		}
	}
	free(informacion_cliente);
	free(informacion_puerto);
}

Dispositivo_Midi* Secuenciador_Alsa::crear_nuevo_dispositivo(unsigned char cliente, unsigned char puerto) const
{
	snd_seq_client_info_t* informacion_cliente;
	snd_seq_port_info_t* informacion_puerto;

	int estado1 = snd_seq_client_info_malloc(&informacion_cliente);
	int estado2 = snd_seq_port_info_malloc(&informacion_puerto);

	this->mostrar_estado_alsa(estado1, "No se ha podido reservar memoria para la informacion del cliente - nuevo dispositivo");
	this->mostrar_estado_alsa(estado2, "No se ha podido reservar memoria para la informacion del puerto - nuevo dispositivo");
	if(estado1 < 0 || estado2 < 0)
		return NULL;

	estado1 = snd_seq_get_any_client_info(m_secuenciador_alsa, cliente, informacion_cliente);
	estado2 = snd_seq_get_any_port_info(m_secuenciador_alsa, cliente, puerto, informacion_puerto);

	this->mostrar_estado_alsa(estado1, "No se ha podido obtener la informacion del cliente - nuevo dispositivo");
	this->mostrar_estado_alsa(estado2, "No se ha podido obtener la informacion del puerto - nuevo dispositivo");
	if(estado1 < 0 || estado2 < 0)
		return NULL;

	unsigned int capacidad = snd_seq_port_info_get_capability(informacion_puerto);

	unsigned char capacidad_dispositivo = 0;
	if((capacidad & Bandera_entrada) == Bandera_entrada)
		capacidad_dispositivo = ENTRADA;
	if((capacidad & Bandera_salida) == Bandera_salida)
		capacidad_dispositivo = capacidad_dispositivo | SALIDA;

	Dispositivo_Midi *nuevo = NULL;
	if(capacidad_dispositivo > 0)
	{
		std::string nombre_cliente = snd_seq_client_info_get_name(informacion_cliente);
		//std::string nombre_puerto = snd_seq_port_info_get_name(informacion_puerto);
		nuevo = new Dispositivo_Midi(cliente, puerto, capacidad_dispositivo, nombre_cliente, true);
	}

	free(informacion_cliente);
	free(informacion_puerto);

	return nuevo;
}

bool Secuenciador_Alsa::conectar(unsigned char cliente, unsigned char puerto, Tipo tipo) const
{
	int estado = 0;
	if(tipo == Entrada)
	{
		estado = snd_seq_connect_from(m_secuenciador_alsa, m_puerto_entrada, cliente, puerto);
		this->mostrar_estado_alsa(estado, "No se ha podido conectar al dispositivo de entrada");
	}
	else if(tipo == Salida)
	{
		estado = snd_seq_connect_to(m_secuenciador_alsa, m_puerto_salida, cliente, puerto);
		this->mostrar_estado_alsa(estado, "No se ha podido conectar al dispositivo de salida");
	}
	if(estado < 0)
		return false;
	return true;
}

bool Secuenciador_Alsa::desconectar(unsigned char cliente, unsigned char puerto, Tipo tipo) const
{
	int estado = 0;
	if(tipo == Entrada)
	{
		estado = snd_seq_disconnect_from(m_secuenciador_alsa, m_puerto_entrada, cliente, puerto);
		this->mostrar_estado_alsa(estado, "No se ha podido desconectar del dispositivo de entrada");
	}
	else if(tipo == Salida)
	{
		estado = snd_seq_disconnect_to(m_secuenciador_alsa, m_puerto_salida, cliente, puerto);
		this->mostrar_estado_alsa(estado, "No se ha podido desconectar del dispositivo de salida");
	}
	if(estado < 0)
		return false;
	return true;
}

void Secuenciador_Alsa::escribir(const Evento_Midi &evento_salida) const
{
	//Termina si recive un evento vacio
	if(evento_salida.datos() == NULL || evento_salida.largo_datos() == 0)
		return;

	snd_seq_event_t evento;
	snd_seq_ev_clear(&evento);

	evento.source.port = static_cast<unsigned char>(m_puerto_salida);
	if(evento_salida.cliente() == DIRECCIONES_SUSCRITAS || evento_salida.puerto() == DIRECCIONES_SUSCRITAS)
	{
		evento.dest.client = SND_SEQ_ADDRESS_SUBSCRIBERS;
		evento.dest.port = SND_SEQ_ADDRESS_UNKNOWN;
	}
	else
	{
		evento.dest.client = static_cast<unsigned char>(evento_salida.cliente());
		evento.dest.port = static_cast<unsigned char>(evento_salida.puerto());
	}
	evento.queue = SND_SEQ_QUEUE_DIRECT;

	if(evento_salida.tipo_evento() == EventoMidi_NotaApagada)
	{
		evento.type = SND_SEQ_EVENT_NOTEOFF;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
		evento.data.note.channel = evento_salida.canal();
		evento.data.note.note = static_cast<unsigned char>(evento_salida.id_nota());
		evento.data.note.velocity = evento_salida.velocidad_nota();
	}
	else if(evento_salida.tipo_evento() == EventoMidi_NotaEncendida)
	{
		evento.type = SND_SEQ_EVENT_NOTEON;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
		evento.data.note.channel = evento_salida.canal();
		evento.data.note.note = static_cast<unsigned char>(evento_salida.id_nota());
		evento.data.note.velocity = evento_salida.velocidad_nota();
	}
	else if(evento_salida.tipo_evento() == EventoMidi_Controlador)
	{
		evento.type = SND_SEQ_EVENT_CONTROLLER;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
		evento.data.control.channel = evento_salida.canal();
		evento.data.control.param = evento_salida.controlador_mensaje();
		evento.data.control.value = evento_salida.controlador_valor();
	}
	else if(evento_salida.tipo_evento() == EventoMidi_CambioPrograma)
	{
		evento.type = SND_SEQ_EVENT_PGMCHANGE;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
		evento.data.control.channel = evento_salida.canal();
		evento.data.control.value = evento_salida.programa();
	}
	else if(evento_salida.tipo_evento() == EventoMidi_ExclusivoDelSistema)
	{
		evento.type = SND_SEQ_EVENT_SYSEX;
		evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
		evento.flags |= SND_SEQ_EVENT_LENGTH_VARIABLE;
		evento.data.ext.len = static_cast<unsigned int>(evento_salida.largo_datos());
		evento.data.ext.ptr = evento_salida.datos();
	}

	int estado = snd_seq_event_output(m_secuenciador_alsa, &evento);
	this->mostrar_estado_alsa(estado, "No se ha podido enviar el evento");

	estado = snd_seq_drain_output(m_secuenciador_alsa);
	if(estado < 0)
	{
		//Se eliminan todos los eventos que no se enviaron correctamente
		snd_seq_drop_output_buffer(m_secuenciador_alsa);
		if(estado != -2)//Se desconecto el dispositivo justo cuando se le envio un evento
			this->mostrar_estado_alsa(estado, "No se ha podido drenar el evento");
	}
}

Evento_Midi Secuenciador_Alsa::leer() const
{
	if(!this->hay_eventos())
		return Evento_Midi();

	snd_seq_event_t* evento;
	int estado = snd_seq_event_input(m_secuenciador_alsa, &evento);
	this->mostrar_estado_alsa(estado, "Al leer los eventos de entrada");

	if(evento->type == SND_SEQ_EVENT_NOTEOFF)
	{
		Evento_Midi evento_nuevo(EventoMidi_NotaApagada);
		evento_nuevo.canal(evento->data.note.channel);
		evento_nuevo.id_nota(evento->data.note.note);
		evento_nuevo.velocidad_nota(evento->data.note.velocity);
		evento_nuevo.cliente(evento->source.client);
		evento_nuevo.puerto(evento->source.port);
		return evento_nuevo;
	}
	else if(evento->type == SND_SEQ_EVENT_NOTEON)
	{
		Evento_Midi evento_nuevo(EventoMidi_NotaEncendida);
		evento_nuevo.canal(evento->data.note.channel);
		evento_nuevo.id_nota(evento->data.note.note);
		evento_nuevo.velocidad_nota(evento->data.note.velocity);
		evento_nuevo.cliente(evento->source.client);
		evento_nuevo.puerto(evento->source.port);
		return evento_nuevo;
	}
	else if(evento->type == SND_SEQ_EVENT_PGMCHANGE)
	{
		Evento_Midi evento_nuevo(EventoMidi_CambioPrograma);
		evento_nuevo.canal(evento->data.note.channel);
		evento_nuevo.programa(static_cast<unsigned char>(evento->data.control.value));
		evento_nuevo.cliente(evento->source.client);
		evento_nuevo.puerto(evento->source.port);
		return evento_nuevo;
	}
	else if(evento->type == SND_SEQ_EVENT_CLIENT_START)
	{
		Evento_Midi evento_nuevo(EventoMidi_ClienteConectado);
		evento_nuevo.cliente(evento->data.addr.client);
		evento_nuevo.puerto(evento->data.addr.port);
		return evento_nuevo;
	}
	else if(evento->type == SND_SEQ_EVENT_CLIENT_EXIT)
	{
		Evento_Midi evento_nuevo(EventoMidi_ClienteDesconectado);
		evento_nuevo.cliente(evento->data.addr.client);
		evento_nuevo.puerto(evento->data.addr.port);
		return evento_nuevo;
	}
	else if(evento->type == SND_SEQ_EVENT_PORT_START)
	{
		Evento_Midi evento_nuevo(EventoMidi_PuertoConectado);
		evento_nuevo.cliente(evento->data.addr.client);
		evento_nuevo.puerto(evento->data.addr.port);

		return evento_nuevo;
	}
	else if(evento->type == SND_SEQ_EVENT_PORT_EXIT)
	{
		Evento_Midi evento_nuevo(EventoMidi_PuertoDesconectado);
		evento_nuevo.cliente(evento->data.addr.client);
		evento_nuevo.puerto(evento->data.addr.port);
		return evento_nuevo;
	}
	else if(evento->type == SND_SEQ_EVENT_CONTROLLER)//10
	{
		Evento_Midi evento_nuevo(EventoMidi_Controlador);
		evento_nuevo.canal(evento->data.note.channel);
		evento_nuevo.controlador_mensaje(static_cast<unsigned char>(evento->data.control.param));
		evento_nuevo.controlador_valor(static_cast<unsigned char>(evento->data.control.value));
		evento_nuevo.cliente(evento->source.client);
		evento_nuevo.puerto(evento->source.port);
		return evento_nuevo;
	}
	else if(evento->type == SND_SEQ_EVENT_PORT_SUBSCRIBED)//66
	{
		Evento_Midi evento_nuevo(EventoMidi_PuertoSuscrito);
		evento_nuevo.cliente(evento->data.addr.client);
		evento_nuevo.puerto(evento->data.addr.port);
		return evento_nuevo;
	}
	else if(evento->type == SND_SEQ_EVENT_SYSEX)//130
	{
		/*datos = static_cast<unsigned char*>(evento->data.ext.ptr);

		Evento_Midi evento_nuevo(EventoMidi_ExclusivoDelSistema, datos, evento->data.ext.len);
		evento_nuevo.cliente(evento->source.client);
		evento_nuevo.puerto(evento->source.port);
		return evento_nuevo;*/
	}
	return Evento_Midi();
}

bool Secuenciador_Alsa::hay_eventos() const
{
	if(snd_seq_event_input_pending(m_secuenciador_alsa, 1) > 0)
		return true;
	return false;
}

void Secuenciador_Alsa::enviar_nota(unsigned char id_nota, bool estado) const
{
	snd_seq_event_t evento;
	snd_seq_ev_clear(&evento);

	evento.source.port = static_cast<unsigned char>(m_puerto_virtual);
	evento.dest.client = SND_SEQ_ADDRESS_SUBSCRIBERS;
	evento.dest.port = SND_SEQ_ADDRESS_UNKNOWN;
	evento.queue = SND_SEQ_QUEUE_DIRECT;

	if(estado)
	{
		evento.type = SND_SEQ_EVENT_NOTEON;
		evento.data.note.velocity = VELOCIDAD_NORMAL;
	}
	else
	{
		evento.type = SND_SEQ_EVENT_NOTEOFF;
		evento.data.note.velocity = 0;
	}

	evento.flags &= static_cast<unsigned char>(~SND_SEQ_EVENT_LENGTH_MASK);
	evento.flags |= SND_SEQ_EVENT_LENGTH_FIXED;
	evento.data.note.channel = 0;
	evento.data.note.note = id_nota;

	int estado1 = snd_seq_event_output(m_secuenciador_alsa, &evento);
	this->mostrar_estado_alsa(estado1, "No se ha podido enviar el evento del teclado virtual");

	estado1 = snd_seq_drain_output(m_secuenciador_alsa);
	if(estado1 < 0)
	{
		//Se eliminan todos los eventos que no se enviaron correctamente
		snd_seq_drop_output_buffer(m_secuenciador_alsa);
		if(estado1 != -2)
			this->mostrar_estado_alsa(estado1, "No se ha podido drenar el evento del teclado virtual");
	}
}
