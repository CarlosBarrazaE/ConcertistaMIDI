#include "ventana_organo.h++"

VentanaOrgano::VentanaOrgano(Configuracion *configuracion, Datos_Musica *musica, Administrador_Recursos *recursos) : Ventana(), m_texto_velocidad(recursos), m_texto_pausa(recursos), m_subtitulos(recursos), m_texto_combos(recursos)
{
	m_configuracion = configuracion;
	m_musica = musica;

	m_rectangulo = recursos->figura(F_Rectangulo);
	m_textura_subtitulo = recursos->textura(T_Nota);

	//Carga la configuracion de la base de datos
	std::string resultado_velocidad = m_configuracion->leer("velocidad_musica");

	if(resultado_velocidad != "")
	{
		m_velocidad_musica = std::stod(resultado_velocidad);
		if(m_velocidad_musica <= 0)
			m_velocidad_musica = 0.01;
		else if(m_velocidad_musica > 2)
			m_velocidad_musica = 2;
	}
	else
		m_velocidad_musica = 1.0;

	std::string resultado_teclado = m_configuracion->leer("tipo_teclado");
	if(resultado_teclado != "")
		m_teclado_actual.cargar(resultado_teclado);
	else
		m_teclado_actual.cambiar(21, 88);//Teclado normal de 88 teclas

	m_barra = new Barra_Progreso(0, 40, Pantalla::Ancho, 40, m_musica->musica()->GetSongLengthInMicroseconds(), m_musica->musica()->GetBarLines(), recursos);
	m_organo = new Organo(0, Pantalla::Alto, Pantalla::Ancho, &m_teclado_actual, recursos);
	m_tablero = new Tablero_Notas(0, m_barra->alto()+40, Pantalla::Ancho, Pantalla::Alto - (m_organo->alto() + m_barra->alto() + 40), &m_teclado_actual, recursos);

	m_titulo_musica = new Titulo(0, m_barra->alto()+40, Pantalla::Ancho, Pantalla::Alto - (m_organo->alto() + m_barra->alto() + 40), recursos);
	m_titulo_musica->datos(musica);

	m_puntaje = new Puntuacion(20, 100, 200, 84, recursos);

	m_texto_velocidad.texto(std::to_string(static_cast<int>(m_velocidad_musica*100)) + "%");
	m_texto_velocidad.tipografia(recursos->tipografia(LetraTitulo));
	m_texto_velocidad.color(Color(1.0f, 1.0f, 1.0f));
	m_texto_velocidad.posicion(0, 0);
	m_texto_velocidad.dimension(Pantalla::Ancho, 40);
	m_texto_velocidad.centrado(true);

	m_texto_pausa.texto("Pausa");
	m_texto_pausa.tipografia(recursos->tipografia(LetraTitulo));
	m_texto_pausa.color(Color(0.0f, 0.0f, 0.0f));
	m_texto_pausa.posicion(0, 200);
	m_texto_pausa.dimension(Pantalla::Ancho, 40);
	m_texto_pausa.centrado(true);

	m_subtitulos.texto("");
	m_subtitulos.tipografia(recursos->tipografia(LetraTitulo));
	m_subtitulos.color(Color(0.0f, 0.0f, 0.0f));
	m_subtitulos.posicion(0, 100);
	m_subtitulos.dimension(Pantalla::Ancho, 20);
	m_subtitulos.centrado(true);

	m_texto_combos.texto("");
	m_texto_combos.tipografia(recursos->tipografia(LetraTitulo));
	m_texto_combos.color(Color(1.0f, 0.5f, 0.0f));
	m_texto_combos.posicion(0, 150);
	m_texto_combos.dimension(Pantalla::Ancho, 20);
	m_texto_combos.centrado(true);


	m_notas = m_musica->musica()->Notes();
	m_pistas = m_musica->pistas();
	m_tablero->notas(m_notas);
	m_tablero->pistas(m_pistas);
	m_tablero->lineas(m_musica->musica()->GetBarLines());
	m_organo->notas_activas(&m_color_teclas_teclas);
	m_organo->notas_requeridas(&m_notas_requeridas);

	//Carga la configuracion de la base de datos de la duracion
	std::string resultado_duracion = m_configuracion->leer("duracion_nota");
	if(resultado_duracion != "")
	{
		m_duracion_nota = std::stoi(resultado_duracion);
		m_tablero->modificar_duracion_nota(m_duracion_nota);
	}
	else
		m_duracion_nota = 6500;

	//Elimina las notas tocadas antes de esta ventana
	m_configuracion->dispositivo_entrada()->Reset();

	m_cambio_velocidad = false;
	m_pausa = false;
	m_retorno_carro = false;

	//Carga la configuracion de los subtitulos
	std::string estado_subtitulo = m_configuracion->leer("estado_subtitulo");
	if(estado_subtitulo == "inactivo")
		m_mostrar_subtitulo = false;
	else
		m_mostrar_subtitulo = true;
	m_descartar_texto_inicial = true;

	m_guardar_velocidad = false;
	m_guardar_duracion_nota = false;
	m_guardar_tipo_teclado = false;
	m_guardar_estado_subtitulo = false;
	this->inicializar();
}

VentanaOrgano::~VentanaOrgano()
{
	m_configuracion->dispositivo_entrada()->Reset();
	if(m_configuracion->dispositivo_salida() != NULL)
		m_configuracion->dispositivo_salida()->Reset();
	delete m_barra;
	delete m_tablero;
	delete m_organo;
	delete m_titulo_musica;
	delete m_puntaje;
}

void VentanaOrgano::actualizar(unsigned int diferencia_tiempo)
{
	//Cuando termina la cancion se retrocede a la ventana anterior
	if(m_musica->musica()->IsSongOver())
	{
		this->guardar_configuracion();

		m_musica->reiniciar();
		m_accion = CambiarASeleccionPista;
	}

	//Se calculan los microsegundos entre fotogramas para actualizar el midi
	unsigned int microsegundos_actualizar = static_cast<unsigned int>((static_cast<double>(diferencia_tiempo) / 1000.0) * m_velocidad_musica);

	if(!this->hay_notas_requeridas())
		this->reproducir_eventos(microsegundos_actualizar);
	this->calcular_teclas_activas(diferencia_tiempo);
	this->escuchar_eventos();

	//Agregar al organo el color de las teclas presionada
	for(std::pair<unsigned int, Nota_Activa*> valor : m_notas_activas)
		m_color_teclas_teclas[valor.second->id_nota] = valor.second->color;

	//Actualiza la etiqueta de combos
	if(m_puntaje->combo() > COMBO_MINIMO_MOSTRAR)
		m_texto_combos.texto("¡Combo " + std::to_string(m_puntaje->combo()) + "!");

	//Si selecciono un nuevo tiempo en la barra de progreso, se cambia la posicion.
	microseconds_t cambio_tiempo = m_barra->tiempo_seleccionado();
	if(cambio_tiempo >= 0)
	{
		m_musica->musica()->GoTo(cambio_tiempo);
		m_tiempo_actual_midi = m_musica->musica()->GetSongPositionInMicroseconds();
		m_tiempo_actual_midi = cambio_tiempo;
		this->reiniciar();
	}

	//Cambio de velocidad de la musica
	if(m_cambio_velocidad)
	{
		m_cambio_velocidad = false;
		m_texto_velocidad.texto(std::to_string(static_cast<int>(m_velocidad_musica*100)) + "%");
	}

	m_barra->tiempo(m_tiempo_actual_midi);
	m_tablero->tiempo(m_tiempo_actual_midi);

	//Se actualizan los componentes
	m_barra->actualizar(diferencia_tiempo);
	m_tablero->actualizar(diferencia_tiempo);
	m_organo->actualizar(diferencia_tiempo);
	m_titulo_musica->actualizar(diferencia_tiempo);
}

void VentanaOrgano::dibujar()
{
	m_tablero->dibujar();
	m_barra->dibujar();
	m_organo->dibujar();
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(0, 0, Pantalla::Ancho, 40, Color(0.141f, 0.624f, 0.933f));
	m_texto_velocidad.dibujar();
	if(m_pausa)
		m_texto_pausa.dibujar();
	if(m_mostrar_subtitulo && m_subtitulo_texto.length() > 0)
	{
		//Dibuja el fondo
		m_textura_subtitulo->activar();
		m_rectangulo->textura(true);
		m_rectangulo->extremos_fijos(true, true);
		m_rectangulo->color(Color(0.9f, 0.9f, 0.9f));
		m_rectangulo->dibujar_estirable(Pantalla::Centro_horizontal()-((m_subtitulos.largo_texto()+40)/2), 90, m_subtitulos.largo_texto()+40, m_subtitulos.alto_texto()+20, 15.0f, 12.0f);
		m_subtitulos.dibujar();
		m_rectangulo->extremos_fijos(false, false);
	}
	if(m_puntaje->combo() > COMBO_MINIMO_MOSTRAR)
		m_texto_combos.dibujar();
	m_puntaje->dibujar();
	m_titulo_musica->dibujar();
}

void VentanaOrgano::inicializar()
{
	//Se inician todas las pistas en 0
	for(unsigned int i=0; i<m_notas.size(); i++)
		m_primera_nota.push_back(0);

	//Inicializa todos los tiempos de espera a 0
	for(unsigned int i=0; i<128; i++)
		m_tiempo_espera[i] = 0;

	m_tiempo_actual_midi = 0;

	//Cuenta el numero de notas totales que seran jugadas
	std::vector<MidiTrack> pistas_midi = m_musica->musica()->Tracks();
	unsigned int notas_jugables = 0;
	for(unsigned int i=0; i<pistas_midi.size(); i++)
	{
		if(m_pistas->at(i).modo() != Fondo)
			notas_jugables += pistas_midi[i].AggregateNoteCount();
	}
	m_puntaje->notas_totales(notas_jugables);
}

void VentanaOrgano::reproducir_eventos(unsigned int microsegundos_actualizar)
{
	if(m_pausa)
		microsegundos_actualizar = 0;

	MidiEventListWithTrackId eventos = m_musica->musica()->Update(microsegundos_actualizar);
	m_tiempo_actual_midi = m_musica->musica()->GetSongPositionInMicroseconds();

	//Se escriben las notas
	bool notas_requeridas_nuevas = false;
	for (MidiEventListWithTrackId::const_iterator i = eventos.begin(); i != eventos.end(); i++)
	{
		bool reproducir_evento = false;
		//Muestra los subtitulos si los contiene
		this->reproducir_subtitulos(i->second);

		//Reproduce todos los eventos si es musica de Fondo
		if(m_pistas->at(i->first).modo() == Fondo)
			reproducir_evento = true;
		//Reproduce todos los eventos que no sean NoteOn o NoteOff de todas las pistas
		else if(i->second.Type() != MidiEventType_NoteOn && i->second.Type() != MidiEventType_NoteOff)
			reproducir_evento = true;

		//Solo son erroneas las notas que no se tocan correctamente en el noteon
		if(i->second.Type() == MidiEventType_NoteOn && i->second.NoteVelocity() > 0)
		{
			//Agrega la nota actual a notas requeridas
			if(m_pistas->at(i->first).modo() == Aprender)
			{
				this->agregar_nota_requerida(i->second.NoteNumber(), m_pistas->at(i->first).color());
				notas_requeridas_nuevas = true;
			}
		}
		else if((i->second.Type() == MidiEventType_NoteOn || i->second.Type() == MidiEventType_NoteOff) && i->second.NoteVelocity() == 0)
		{
			//Si nota activa y llega un noteoff apagar y cambiar a plomo
			if(m_pistas->at(i->first).modo() != Fondo)
			{
				if(this->esta_tocada(i->second.NoteNumber()))
					this->eliminar_nota_tocada(i->second.NoteNumber());
				else
					m_puntaje->reiniciar_combo();
				std::map<unsigned int, Nota_Activa*>::iterator nota = m_notas_activas.find(i->second.NoteNumber());
				if(nota != m_notas_activas.end())
				{
					//La nota si fue tocada correctamente pero ahora se paso de largo
					//y asegurandome que no sea una nueva nota que empezo un momento antes
					//no se considera erroneo pero cambia el color a plomo
					if(nota->second->correcta && !this->esta_tocada(i->second.NoteNumber()))
					{
						nota->second->color = Pista::Colores_pista[NUMERO_COLORES_PISTA];
						nota->second->correcta = false;
					}
				}
			}
		}

		//Omite los eventos si la pista esta en silencio
		if(reproducir_evento && m_pistas->at(i->first).sonido())
		{
			if(m_configuracion->dispositivo_salida() != NULL)
				m_configuracion->dispositivo_salida()->Write(i->second);
		}
	}
	//Borra las notas requeridas si todas son tocadas a la vez
	if(notas_requeridas_nuevas)
		this->borrar_notas_requeridas();
}

void VentanaOrgano::escuchar_eventos()
{
	//No hay nada que hacer si no hay dispositivo de entrada
	if(m_configuracion->dispositivo_entrada() == NULL)
		return;

	//Lee todos los eventos
	bool nuevas_notas_tocadas = false;
	while(m_configuracion->dispositivo_entrada()->KeepReading())
	{
		MidiEvent evento = m_configuracion->dispositivo_entrada()->Read();

		//Omitir eventos que no son NoteOn o NoteOff
		if(evento.Type() != MidiEventType_NoteOn && evento.Type() != MidiEventType_NoteOff)
			continue;

		if(evento.Type() == MidiEventType_NoteOn && evento.NoteVelocity() > 0)
		{
			//Eventos NoteOn
			TranslatedNote *nota_encontrada = NULL;
			unsigned long int pista_encontrada = 0;
			for(unsigned long int pista = 0; pista < m_notas.size(); pista++)
			{
				//Se salta las pistas que no corresponde tocar
				if(m_pistas->at(pista).modo() == Fondo)
					continue;

				for(unsigned int n=m_primera_nota[pista]; n<m_notas[pista].size(); n++)
				{
					TranslatedNote *nota_actual = &m_notas[pista][n];
					microseconds_t tiempo_inicio = nota_actual->start - TIEMPO_DETECCION;
					microseconds_t tiempo_final = nota_actual->start + TIEMPO_DETECCION;

					//Termina con esta pista si la nota aun no llega
					if(tiempo_inicio > m_tiempo_actual_midi)
						break;

					//Nota dentro del rango
					if(tiempo_final > m_tiempo_actual_midi && nota_actual->note_id == evento.NoteNumber())
					{
						//Primera coincidencia detectada
						if(nota_encontrada == NULL)
						{
							nota_encontrada = &m_notas[pista][n];
							pista_encontrada = pista;//Guarda la pista
						}
						else
						{
							microseconds_t distancia_actual = abs(m_tiempo_actual_midi - nota_encontrada->start);
							microseconds_t distancia_anterior = abs(m_tiempo_actual_midi - nota_encontrada->start);

							//Se encuentra una nota mas cercana al evento
							if(distancia_actual < distancia_anterior)
							{
								nota_encontrada = &m_notas[pista][n];
								pista_encontrada = pista;//Guarda la pista
							}
						}
					}
				}
			}

			//Se guarda la nota tocada por el jugador
			if(nota_encontrada != NULL && !m_pausa)
			{
				//Nota correcta
				this->insertar_nota_activa(nota_encontrada->note_id, nota_encontrada->channel, m_pistas->at(pista_encontrada).color(), m_pistas->at(pista_encontrada).sonido(), true);

				//Se cambia el canal y la velocidad del evento
				evento.SetChannel(nota_encontrada->channel);

				//Aumenta el contador de combos
				//El modo aprender cuenta el puntaje cuando todas las notas son tocadas correctamente
				//en el metodo borrar_notas_requeridas()
				if(m_pistas->at(pista_encontrada).modo() != Aprender)
				{
					m_puntaje->combo(1);//Suma 1
					m_puntaje->nota_correcta(1, m_tiempo_actual_midi, m_velocidad_musica);
				}
				m_notas_correctas.push_back(nota_encontrada->note_id);

				nuevas_notas_tocadas = true;

				//Se envia el evento
				if(m_configuracion->dispositivo_salida() != NULL && m_pistas->at(pista_encontrada).sonido())
					m_configuracion->dispositivo_salida()->Write(evento);
			}
			else
			{
				//Notas plomas son notas erroneas
				this->insertar_nota_activa(evento.NoteNumber(), evento.Channel(), Pista::Colores_pista[NUMERO_COLORES_PISTA], true, false);

				//Se envia el evento
				if(m_configuracion->dispositivo_salida() != NULL)
					m_configuracion->dispositivo_salida()->Write(evento);

				//Pierde el combo
				m_puntaje->reiniciar_combo();
				if(m_tiempo_actual_midi >= 0)
					m_puntaje->sumar_error();//Solo cuenta el error cuando toca cuando no correspone
			}
		}
		else
		{
			//Eventos NoteOff
			std::map<unsigned int, Nota_Activa*>::iterator nota_encendida = m_notas_activas.find(evento.NoteNumber());
			if(nota_encendida != m_notas_activas.end())
			{
				if(nota_encendida->second->contador_clic == 0)
				{
					//Se selecciona el canal
					evento.SetChannel(nota_encendida->second->canal);

					//Se envia el evento de apagado
					if(m_configuracion->dispositivo_salida() != NULL && nota_encendida->second->sonido)
						m_configuracion->dispositivo_salida()->Write(evento);

					//Borra la nota
					m_notas_activas.erase(nota_encendida->second->id_nota);
					delete nota_encendida->second;
				}
				//Se puede activar dos veces (o mas) la misma tecla por ejemplo con el teclado y el raton
				//no se borrara la nota hasta que se reciban la misma cantidad de NoteOff que NoteOn
				//pero no se cambia nada.
				else
					nota_encendida->second->contador_clic--;
			}
			else
			{
				//La nota no existe, esto puede ocurrir al utilizar un teclado MIDI
				//y estar tocando antes de entrar en esta ventana
				Registro::Aviso("Intento borrar una nota que no existe");
			}
		}
	}
	//Borra las notas requeridas si todas son tocadas a la vez
	if(nuevas_notas_tocadas)
		this->borrar_notas_requeridas();
}

void VentanaOrgano::reproducir_subtitulos(const MidiEvent &evento)
{
	if(!evento.HasText())
		return;
	std::string nuevo_texto = evento.Text();
	std::replace(nuevo_texto.begin(), nuevo_texto.end(), '_', ' ');
	if(evento.MetaType() == MidiMetaEvent_Lyric || (evento.MetaType() == MidiMetaEvent_Text && (evento.GetDeltaPulses() > 0 || !m_descartar_texto_inicial)))
	{
		//Retorno de carro para la proxima linea generalmente en MidiMetaEvent_Lyric
		if(nuevo_texto.length() > 0 && (nuevo_texto[0] == '\r' || nuevo_texto[0] == '\n'))
			m_retorno_carro = true;
		else if(nuevo_texto.length() > 0 && (nuevo_texto[0] == '\\' || nuevo_texto[0] == '/'))
		{
			//Midi karaoke generalmente en MidiMetaEvent_Text
			std::string recortado = nuevo_texto.substr(1);//Quita el primer caracter
			m_subtitulo_texto = recortado;
		}
		else
		{
			if(m_subtitulo_texto.length() >= 80 || m_retorno_carro)
			{
				m_subtitulo_texto = nuevo_texto;
				m_retorno_carro = false;
			}
			else
				m_subtitulo_texto += nuevo_texto;
		}

		//Al comienzo de algunos midi se incluye texto que no corresponde a la letra de la cancion
		//con GetDeltaPulses igual a cero se puede detectar, en cambio cuando es letra normalmente es mayor a 0 pero no siempre,
		//por eso se espera a que al menos exista texto con GetDeltaPulses > 0 para asegurarse que no se muestra
		//texto incorrecto, solo es valido para MidiMetaEvent_Text
		m_descartar_texto_inicial = false;
	}
	//Limpia la cadena si contiene caracteres invisibles
	if(Texto::esta_vacio(m_subtitulo_texto) && m_subtitulo_texto.length() > 0)
		m_subtitulo_texto = "";
	if(m_mostrar_subtitulo)
		m_subtitulos.texto(Texto::quitar_espacios_en_extremos(m_subtitulo_texto));
	/*
	if(evento.MetaType() == MidiMetaEvent_Copyright)
		Registro::Depurar("Evento Meta: MidiMetaEvent_Copyright Contenido: " + evento.Text() + " Largo: " + std::to_string(evento.Text().length()));
	else if(evento.MetaType() == MidiMetaEvent_TrackName)
		Registro::Depurar("Evento Meta: MidiMetaEvent_TrackName Contenido: " + evento.Text() + " Largo: " + std::to_string(evento.Text().length()));
	else if(evento.MetaType() == MidiMetaEvent_Instrument)
		Registro::Depurar("Evento Meta: MidiMetaEvent_Instrument Contenido: " + evento.Text() + " Largo: " + std::to_string(evento.Text().length()));
	else if(evento.MetaType() == MidiMetaEvent_Marker)
		Registro::Depurar("Evento Meta: MidiMetaEvent_Marker Contenido: " + evento.Text() + " Largo: " + std::to_string(evento.Text().length()));
	else if(evento.MetaType() == MidiMetaEvent_Cue)
		Registro::Depurar("Evento Meta: MidiMetaEvent_Cue Contenido: " + evento.Text() + " Largo: " + std::to_string(evento.Text().length()));
	else if(evento.MetaType() == MidiMetaEvent_PatchName)
		Registro::Depurar("Evento Meta: MidiMetaEvent_PatchName Contenido: " + evento.Text() + " Largo: " + std::to_string(evento.Text().length()));
	else if(evento.MetaType() == MidiMetaEvent_DeviceName)
		Registro::Depurar("Evento Meta: MidiMetaEvent_DeviceName Contenido: " + evento.Text() + " Largo: " + std::to_string(evento.Text().length()));*/
}

void VentanaOrgano::guardar_configuracion()
{
	//Se guarda la configuracion
	if(m_guardar_velocidad)
		m_configuracion->escribir("velocidad_musica", std::to_string(m_velocidad_musica));
	if(m_guardar_duracion_nota)
		m_configuracion->escribir("duracion_nota", std::to_string(m_tablero->duracion_nota()));//modificar_duracion_nota
	if(m_guardar_tipo_teclado)
		m_configuracion->escribir("tipo_teclado", m_teclado_actual.texto());
	if(m_guardar_estado_subtitulo)
	{
		if(m_mostrar_subtitulo)
			m_configuracion->escribir("estado_subtitulo", "activo");
		else
			m_configuracion->escribir("estado_subtitulo", "inactivo");
	}
}

void VentanaOrgano::calcular_teclas_activas(unsigned int diferencia_tiempo)
{
	float posicion_y = 0;
	float largo_nota = 0;
	unsigned int numero_nota = 0;//Id de la nota desde 0 hasta 127
	for(unsigned int pista=0; pista<m_notas.size(); pista++)
	{
		//Dibuja solo las pistas que tienen notas, hay pistas vacias
		if(m_notas[pista].size() > 0 && m_pistas->at(pista).visible())
		{
			for(unsigned int n=m_primera_nota[pista]; n<m_notas[pista].size(); n++)
			{
				//Numero_nota incluye blancas y negras
				numero_nota = m_notas[pista][n].note_id;

				//Se salta las notas fuera de la pantalla
				if(numero_nota < m_teclado_actual.tecla_inicial() || numero_nota >= m_teclado_actual.tecla_inicial() + m_teclado_actual.numero_teclas())
					continue;

				posicion_y = static_cast<float>(m_tiempo_actual_midi - m_notas[pista][n].start) / static_cast<float>(m_duracion_nota);

				//Si la nota no esta sonando termina el recorrido por la pista actual
				if(posicion_y < -5)
					break;

				largo_nota = static_cast<float>(m_notas[pista][n].end - m_notas[pista][n].start) / static_cast<float>(m_duracion_nota);

				//El alto minimo de la nota es de 20 pixeles
				if((posicion_y-largo_nota > 0 && largo_nota >= 20) || (posicion_y > 20 && largo_nota < 20) || largo_nota <= 0)//La nota n salio de la pantalla
				{
					//Almacena la posicion de la primera nota visible desde la posicion actual para no tener que recorrer todo de nuevo
					if(n == m_primera_nota[pista])
						m_primera_nota[pista] = n+1;
					//No se dibujan las notas que ya salieron de la pantalla o son invisibles (largo igual a cero)
					continue;
				}

				//Revisa las notas que debieron ser tocadas
				if(posicion_y >= TIEMPO_DETECCION/static_cast<float>(m_duracion_nota) && m_pistas->at(pista).modo() != Fondo)
				{

					//Acelera la deteccion de teclas erroneas
					//al no tener que esperar un NoteOff
					//ya que se acabo el tiempo para tocar la nota
					if(!this->esta_tocada(numero_nota))
					{
						m_puntaje->reiniciar_combo();
						//Se inserta para que no vuelva a borrar el combo la misma nota
						m_notas_correctas.push_back(numero_nota);
					}
				}

				//Solo se actualiza el color de las pistas de Fondo
				if(m_pistas->at(pista).modo() != Fondo)
					continue;

				//Cambia el tiempo de espera de las notas
				if(posicion_y >= -5 && posicion_y < 0 && m_tiempo_espera[numero_nota] <= 0)
					m_tiempo_espera[numero_nota] = (-posicion_y)-1;
				else if(posicion_y >= 0)
				{
					if(m_tiempo_espera[numero_nota] <= 0)
						m_color_teclas_teclas[numero_nota] = m_pistas->at(pista).color();
				}
			}
		}
	}

	float tiempo = static_cast<float>(diferencia_tiempo)/1000000000.0f*(1.0f/0.0166f);
	for(unsigned int i=0; i<128; i++)
	{
		if(m_tiempo_espera[i] > 0)
			m_tiempo_espera[i] -= tiempo;
	}
}

void VentanaOrgano::reiniciar()
{
	//Reinicia el tablero
	m_tablero->reiniciar();

	//Reinicia el contador de combos
	m_puntaje->reiniciar_combo();
	m_puntaje->cambiar_a(m_tiempo_actual_midi);

	//Reinicia las notas tocadas
	m_notas_correctas.clear();

	//Elimina todas las notas requeridas
	m_notas_requeridas.clear();

	//Reinicia la primera nota de cada pista a 0
	for(unsigned int i=0; i<m_primera_nota.size(); i++)
		m_primera_nota[i] = 0;

	//Reinicia la salida
	if(m_configuracion->dispositivo_salida() != NULL)
		m_configuracion->dispositivo_salida()->Reset();

	//Descarta texto inicial de archivo midi
	m_descartar_texto_inicial = true;
}

void VentanaOrgano::insertar_nota_activa(unsigned int id_nota, unsigned char canal, Color color, bool sonido, bool correcta)
{
	Nota_Activa *nota_nueva = m_notas_activas[id_nota];

	if(nota_nueva == NULL)
	{
		nota_nueva = new Nota_Activa();
		nota_nueva->id_nota = id_nota;
		nota_nueva->canal = canal;
		nota_nueva->color = color;
		nota_nueva->sonido = sonido;
		nota_nueva->correcta = correcta;
		m_notas_activas[id_nota] = nota_nueva;
	}
	else
		nota_nueva->contador_clic++;
}

bool VentanaOrgano::esta_tocada(unsigned int id_nota)
{
	for(unsigned long int x=0; x<m_notas_correctas.size(); x++)
	{
		if(m_notas_correctas[x] == id_nota)
			return true;
	}
	return false;
}

void VentanaOrgano::eliminar_nota_tocada(unsigned int id_nota)
{
	for(unsigned long int x=0; x<m_notas_correctas.size(); x++)
	{
		if(m_notas_correctas[x] == id_nota)
		{
			//Elimina solo una intercambiandola por la ultima y borra la ultima
			std::swap(m_notas_correctas[x], m_notas_correctas.back());
			m_notas_correctas.pop_back();
			return;
		}
	}
}

void VentanaOrgano::agregar_nota_requerida(unsigned int id_nota, const Color &color)
{
	m_notas_requeridas[id_nota] = color;
}

void VentanaOrgano::borrar_notas_requeridas()
{
	//Borra las notas requerida solo si todas estan activas al mismo tiempo
	if(m_notas_requeridas.size() > 0)
	{
		for(std::pair<unsigned int, Color> valor : m_notas_requeridas)
		{
			//Si falta alguna nota o no se toco a tiempo, entonces no se borra nada
			std::map<unsigned int, Nota_Activa*>::iterator respuesta = m_notas_activas.find(valor.first);
			if(respuesta != m_notas_activas.end())
			{
				if(!respuesta->second->correcta)
					return;//La nota se toco antes de tiempo (ploma)
			}
			else
				return;//La nota no es tocada
		}
		//Todas son tocadas correctamente por lo que se suman al combo
		m_puntaje->combo(static_cast<unsigned int>(m_notas_requeridas.size()));
		m_puntaje->nota_correcta(static_cast<unsigned int>(m_notas_requeridas.size()), m_tiempo_actual_midi, 0);
		m_notas_requeridas.clear();
	}
}

bool VentanaOrgano::hay_notas_requeridas()
{
	if(m_notas_requeridas.size() > 0)
		return true;
	return false;
}

void VentanaOrgano::evento_raton(Raton *raton)
{
	m_barra->evento_raton(raton);
	m_tablero->evento_raton(raton);
	m_organo->evento_raton(raton);
	m_titulo_musica->evento_raton(raton);
}

void VentanaOrgano::evento_teclado(Tecla tecla, bool estado)
{
	bool cambio_teclado = false;
	if(tecla == TECLA_ESCAPE && !estado)
	{
		this->guardar_configuracion();

		m_musica->reiniciar();
		m_accion = CambiarASeleccionPista;
	}
	else if(tecla == TECLA_FLECHA_ARRIBA && estado)
	{
		m_tablero->duracion_nota(1);
		m_duracion_nota = m_tablero->duracion_nota();
		m_guardar_duracion_nota = true;
	}
	else if(tecla == TECLA_FLECHA_ABAJO && estado)
	{
		m_tablero->duracion_nota(-1);
		m_duracion_nota = m_tablero->duracion_nota();
		m_guardar_duracion_nota = true;
	}
	else if(tecla == TECLA_FLECHA_IZQUIERDA && estado)
	{
		m_velocidad_musica-=0.01;
		if(m_velocidad_musica < 0.01)
			m_velocidad_musica = 0.01;
		m_cambio_velocidad = true;
		m_guardar_velocidad = true;
	}
	else if(tecla == TECLA_FLECHA_DERECHA && estado)
	{
		m_velocidad_musica+=0.01;
		if(m_velocidad_musica > 2)
			m_velocidad_musica = 2;
		m_cambio_velocidad = true;
		m_guardar_velocidad = true;
	}
	else if(tecla == TECLA_ESPACIO && !estado)
	{
		m_pausa = !m_pausa;
		if(m_pausa)
			if(m_configuracion->dispositivo_salida() != NULL)
				m_configuracion->dispositivo_salida()->Reset();
	}
	else if(tecla == TECLA_F4 && estado)
	{
		m_mostrar_subtitulo = !m_mostrar_subtitulo;
		m_guardar_estado_subtitulo = !m_guardar_estado_subtitulo;
		//Actualiza el subtitulo
		if(m_mostrar_subtitulo)
			m_subtitulos.texto(Texto::quitar_espacios_en_extremos(m_subtitulo_texto));
	}
	else if(tecla == TECLA_F5 && estado)
	{
		m_teclado_actual.cambiar(48, 37);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_F6 && estado)
	{
		m_teclado_actual.cambiar(36, 49);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_F7 && estado)
	{
		m_teclado_actual.cambiar(36, 61);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_F8 && estado)
	{
		m_teclado_actual.cambiar(28, 76);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_F9 && estado)
	{
		m_teclado_actual.cambiar(21, 88);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_INSERTAR && estado)
	{
		m_teclado_actual.tecla_inicial(m_teclado_actual.tecla_inicial()+1);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_SUPRIMIR && estado)
	{
		m_teclado_actual.tecla_inicial(m_teclado_actual.tecla_inicial()-1);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_REPAG && estado)
	{
		m_teclado_actual.numero_teclas(m_teclado_actual.numero_teclas()+1);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_AVPAG && estado)
	{
		m_teclado_actual.numero_teclas(m_teclado_actual.numero_teclas()-1);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_INICIO && estado)
	{
		//Va al inicio de la canción
		m_musica->reiniciar(2000000);
		m_tiempo_actual_midi = m_musica->musica()->GetSongPositionInMicroseconds();
		this->reiniciar();
	}
	if(cambio_teclado)
	{
		m_organo->calcular_tamannos();
		//El organo puede cambiar de tamaño al cambiar el numero de teclas, entonces hay que
		//ajustar el tamaño del tablero de notas
		m_tablero->dimension(Pantalla::Ancho, Pantalla::Alto - (m_organo->alto() + m_barra->alto()+40));
		m_guardar_tipo_teclado = true;
	}
}

void VentanaOrgano::evento_pantalla(float ancho, float alto)
{
	m_barra->dimension(ancho, m_barra->alto());
	m_organo->posicion(m_organo->x(), alto);
	m_organo->dimension(ancho, m_organo->alto());
	m_tablero->dimension(ancho, alto - (m_organo->alto() + m_barra->alto()+40));
	m_titulo_musica->dimension(Pantalla::Ancho, Pantalla::Alto - (m_organo->alto() + m_barra->alto() + 40));

	m_texto_velocidad.dimension(ancho, 40);
	m_texto_pausa.dimension(ancho, 40);
	m_subtitulos.dimension(Pantalla::Ancho, 20);
	m_texto_combos.dimension(Pantalla::Ancho, 20);
}
