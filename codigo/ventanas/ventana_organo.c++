#include "ventana_organo.h++"

VentanaOrgano::VentanaOrgano(Configuracion *configuracion, Datos_Musica *musica, Administrador_Recursos *recursos) : Ventana(), m_texto_velocidad(recursos), m_texto_pausa(recursos), m_subtitulos(recursos), m_texto_combos(recursos)
{
	m_configuracion = configuracion;
	m_musica = musica;
	m_evaluacion = m_musica->evaluacion();

	m_rectangulo = recursos->figura(F_Rectangulo);
	m_textura_subtitulo = recursos->textura(T_Nota);

	m_teclas_luminosas = m_configuracion->teclas_luminosas();

	m_volumen = m_configuracion->volumen();
	m_velocidad_musica = m_configuracion->velocidad();
	m_duracion_nota = m_configuracion->duracion_nota();
	m_mostrar_subtitulo = m_configuracion->subtitulos();
	m_teclado_visible = m_configuracion->teclado_visible();
	m_teclado_util = m_configuracion->teclado_util();

	if(m_volumen > 0)
		m_volumen_antes_mute = m_volumen;
	else
		m_volumen_antes_mute = 1;

	m_barra = new Barra_Progreso(0, 40, Pantalla::Ancho, 40, m_musica->musica()->GetSongLengthInMicroseconds(), m_musica->musica()->GetBarLines(), recursos);
	m_organo = new Organo(0, Pantalla::Alto, Pantalla::Ancho, &m_teclado_visible, &m_teclado_util, recursos);
	m_tablero = new Tablero_Notas(0, m_barra->alto()+40, Pantalla::Ancho, Pantalla::Alto - (m_organo->alto() + m_barra->alto() + 40), &m_teclado_visible, m_evaluacion, recursos);

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
	m_tablero->modificar_duracion_nota(m_duracion_nota);
	m_organo->notas_activas(&m_color_teclas);
	m_organo->notas_requeridas(&m_notas_requeridas);

	//Elimina las notas tocadas antes de esta ventana
	if(m_configuracion->dispositivo_entrada() != NULL)
		m_configuracion->dispositivo_entrada()->Reset();

	m_cambio_velocidad = false;
	m_pausa = false;
	m_retorno_carro = false;

	m_descartar_texto_inicial = true;

	this->inicializar();
}

VentanaOrgano::~VentanaOrgano()
{
	m_teclas_luminosas->reiniciar(m_configuracion->dispositivo_salida());

	if(m_configuracion->dispositivo_entrada() != NULL)
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
		m_musica->reiniciar();
		m_accion = CambiarASeleccionPista;
	}

	//Si selecciono un nuevo tiempo en la barra de progreso, se cambia la posicion.
	microseconds_t cambio_tiempo = m_barra->tiempo_seleccionado();
	if(cambio_tiempo >= 0)
	{
		m_musica->musica()->GoTo(cambio_tiempo);
		m_tiempo_actual_midi = m_musica->musica()->GetSongPositionInMicroseconds();
		m_tiempo_actual_midi = cambio_tiempo;
		this->reiniciar();
	}

	//Se actualizan las teclas luminosas
	m_teclas_luminosas->actualizar(diferencia_tiempo, m_configuracion->dispositivo_salida());

	//Se calculan los microsegundos entre fotogramas para actualizar el midi
	unsigned int microsegundos_actualizar = static_cast<unsigned int>((static_cast<double>(diferencia_tiempo) / 1000.0) * m_velocidad_musica);

	if(!this->hay_notas_requeridas())
		this->reproducir_eventos(microsegundos_actualizar);
	this->escuchar_eventos();
	this->calcular_teclas_activas(diferencia_tiempo);

	//Agregar al organo el color de las teclas presionada
	for(std::pair<unsigned int, Nota_Activa*> valor : m_notas_activas)
	{
		if(valor.second->visible)
			m_color_teclas[valor.second->id_nota] = valor.second->color;
	}

	//Actualiza la etiqueta de combos
	if(m_puntaje->combo() > COMBO_MINIMO_MOSTRAR)
		m_texto_combos.texto("¡Combo " + std::to_string(m_puntaje->combo()) + "!");

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
	unsigned int notas_jugables = 0;
	for(unsigned int i=0; i<m_notas.size(); i++)
	{
		if(m_pistas->at(i).modo() != Fondo)
		{
			for(unsigned int j=0; j<m_notas[i].size(); j++)
			{
				//Cuenta solo las notas que se pueden tocar con el teclado_util actual
				if(m_notas[i][j].note_id >= m_teclado_util.tecla_inicial() &&
					m_notas[i][j].note_id <= m_teclado_util.tecla_final() &&
					m_notas[i][j].end > m_notas[i][j].start
				)
					notas_jugables ++;

				Tiempos_Nota nota_nueva;
				nota_nueva.inicio = m_notas[i][j].start;
				nota_nueva.fin = m_notas[i][j].end;
				(*m_evaluacion)[i].push_back(nota_nueva);
			}
		}
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
		//Reproduce las notas que no puede tocar porque estan fuera del teclado
		else if(i->second.NoteNumber() < m_teclado_util.tecla_inicial() ||
			i->second.NoteNumber() > m_teclado_util.tecla_final())
			reproducir_evento = true;

		//Solo son erroneas las notas que no se tocan correctamente en el noteon
		if(i->second.Type() == MidiEventType_NoteOn && i->second.NoteVelocity() > 0)
		{
			//Agrega la nota actual a notas requeridas excepto si esta fuera de rango
			if(i->second.NoteNumber() >= m_teclado_util.tecla_inicial() &&
				i->second.NoteNumber() <= m_teclado_util.tecla_final())
			{
				if(m_pistas->at(i->first).modo() == Aprender)
				{
					this->agregar_nota_requerida(i->second.NoteNumber(), m_pistas->at(i->first).color());
					notas_requeridas_nuevas = true;
				}
				else if(m_pistas->at(i->first).visible())
				{
					//Activa la nota luminosa
					m_teclas_luminosas->encender(i->second.NoteNumber(), m_configuracion->dispositivo_salida());
				}
			}
		}
		else if((i->second.Type() == MidiEventType_NoteOn || i->second.Type() == MidiEventType_NoteOff) && i->second.NoteVelocity() == 0)
		{
			//Si nota activa y llega un noteoff apagar y cambiar a plomo
			if(i->second.NoteNumber() >= m_teclado_util.tecla_inicial() &&
				i->second.NoteNumber() <= m_teclado_util.tecla_final())
			{
				if(m_pistas->at(i->first).modo() != Fondo)
				{
					std::map<unsigned int, Nota_Activa*>::iterator nota = m_notas_activas.find(i->second.NoteNumber());
					if(nota != m_notas_activas.end())
					{
						if(nota->second->correcta && nota->second->visible)
						{
							//La nota si fue tocada correctamente pero ahora se paso de largo
							nota->second->color = Pista::Colores_pista[0];
							nota->second->correcta = false;
						}
						else if(nota->second->correcta && !nota->second->visible)
						{
							//Borra la nota que fue tocada correctamente pero ahora es invisible
							//porque el jugador solto la tecla antes que terminara
							unsigned int nota_a_borrar = nota->second->id_nota;
							delete nota->second;
							m_notas_activas.erase(nota_a_borrar);
						}
					}
					else
						m_puntaje->reiniciar_combo();//No toco la nota por lo que pierde el combo
				}
				if(m_pistas->at(i->first).modo() != Aprender && m_pistas->at(i->first).visible())
					m_teclas_luminosas->apagar(i->second.NoteNumber(), m_configuracion->dispositivo_salida());
			}
		}

		//Omite los eventos si la pista esta en silencio
		if(reproducir_evento && m_pistas->at(i->first).sonido())
		{
			if(m_configuracion->dispositivo_salida() != NULL)
			{
				//Se copia el evento para poder editarlo
				MidiEvent evento_salida = i->second;
				//Escala el volumen
				evento_salida.SetVelocity(static_cast<int>(evento_salida.NoteVelocity() * m_volumen));
				m_configuracion->dispositivo_salida()->Write(evento_salida);
			}
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
		/*Registro::Aviso("Evento capturado: " + GetMidiEventTypeDescription(evento.Type()));
		if(evento.Type() == MidiEventType_Meta)
		{
			Registro::Nota("Evento meta: " + GetMidiMetaEventTypeDescription(evento.MetaType()));
			if(evento.HasText())
			{
				Registro::Depurar(evento.Text());
			}
		}*/

		//Omitir eventos que no son NoteOn o NoteOff
		if(evento.Type() != MidiEventType_NoteOn && evento.Type() != MidiEventType_NoteOff)
			continue;

		if(evento.Type() == MidiEventType_NoteOn && evento.NoteVelocity() > 0)
		{
			//Eventos NoteOn
			TranslatedNote *nota_encontrada = NULL;
			unsigned long int pista_encontrada = 0;
			unsigned long int posicion_encontrada = 0;
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
							posicion_encontrada = n;//Guarda la posicion
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
								posicion_encontrada = n;//Guarda la posicion
							}
						}
					}
				}
			}

			//Se guarda la nota tocada por el jugador
			if(nota_encontrada != NULL && !m_pausa && !this->fue_tocada(nota_encontrada->note_id))
			{
				//Nota correcta
				this->insertar_nota_activa(nota_encontrada->note_id, nota_encontrada->channel, pista_encontrada, posicion_encontrada, m_pistas->at(pista_encontrada).color(), m_pistas->at(pista_encontrada).sonido(), true);
				//Se cambia el canal
				evento.SetChannel(nota_encontrada->channel);
				//Escala el volumen
				evento.SetVelocity(static_cast<int>(evento.NoteVelocity() * m_volumen));

				//Guarda el tiempo en el que se toco la nota para evaluar
				(*m_evaluacion)[pista_encontrada][posicion_encontrada].inicio_tocado = m_tiempo_actual_midi;

				//Aumenta el contador de combos
				//El modo aprender cuenta el puntaje cuando todas las notas son tocadas correctamente
				//en el metodo borrar_notas_requeridas()
				if(m_pistas->at(pista_encontrada).modo() != Aprender)
				{
					m_puntaje->combo(1);//Suma 1
					m_puntaje->nota_correcta(1, m_tiempo_actual_midi, m_velocidad_musica);
				}

				nuevas_notas_tocadas = true;

				//Se envia el evento
				if(m_configuracion->dispositivo_salida() != NULL && m_pistas->at(pista_encontrada).sonido())
					m_configuracion->dispositivo_salida()->Write(evento);
			}
			else
			{
				//Notas plomas son notas erroneas
				this->insertar_nota_activa(evento.NoteNumber(), evento.Channel(), 0, 0, Pista::Colores_pista[0], true, false);

				//Se envia el evento
				if(m_configuracion->dispositivo_salida() != NULL)
				{
					//Escala el volumen
					evento.SetVelocity(static_cast<int>(evento.NoteVelocity() * m_volumen));
					m_configuracion->dispositivo_salida()->Write(evento);
				}

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
					//nota_encendida->second-> = m_musica->musica()->GetSongPositionInMicroseconds();

					//Se envia el evento de apagado
					if(m_configuracion->dispositivo_salida() != NULL && nota_encendida->second->sonido)
						m_configuracion->dispositivo_salida()->Write(evento);

					//Guarda el tiempo en el que se solto la nota para evaluar
					//solo en modo tocar o aprender
					if(m_pistas->at(nota_encendida->second->pista).modo() != Fondo && nota_encendida->second->visible)
						(*m_evaluacion)[nota_encendida->second->pista][nota_encendida->second->posicion].fin_tocado = m_tiempo_actual_midi;
					nota_encendida->second->visible = false;

					//Borra la nota solo si es incorrecta
					if(!nota_encendida->second->correcta)
					{
						unsigned int nota_a_borrar = nota_encendida->second->id_nota;
						delete nota_encendida->second;
						m_notas_activas.erase(nota_a_borrar);
					}
				}
				//Se puede activar dos veces (o mas) la misma tecla por ejemplo con el teclado y el raton
				//no se borrara la nota hasta que se reciban la misma cantidad de NoteOff que NoteOn
				//pero no se cambia nada.
				else
					nota_encendida->second->contador_clic--;
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
				if(numero_nota < m_teclado_visible.tecla_inicial() || numero_nota > m_teclado_visible.tecla_final())
					continue;

				posicion_y = static_cast<float>(m_tiempo_actual_midi - m_notas[pista][n].start) / static_cast<float>(m_duracion_nota);

				//Si la nota no esta sonando termina el recorrido por la pista actual
				if(posicion_y < -5)//-5 igual al tiempo de espera
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

				//Solo se actualiza el color de las pistas de Fondo
				if(m_pistas->at(pista).modo() != Fondo &&
					numero_nota >= m_teclado_util.tecla_inicial() &&
					numero_nota <= m_teclado_util.tecla_final())
					continue;

				//Cambia el tiempo de espera de las notas
				if(posicion_y >= -5 && posicion_y < 0 && m_tiempo_espera[numero_nota] <= 0)
					m_tiempo_espera[numero_nota] = (-posicion_y)-1;
				else if(posicion_y >= 0)
				{
					//Almacena el color si cumplio el tiempo de espera
					if(m_tiempo_espera[numero_nota] <= 0)
						m_color_teclas[numero_nota] = m_pistas->at(pista).color();
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

	//Elimina todas las notas requeridas
	m_notas_requeridas.clear();

	//Reinicia la primera nota de cada pista a 0
	for(unsigned int i=0; i<m_primera_nota.size(); i++)
		m_primera_nota[i] = 0;

	//Reinicia las luces
	m_teclas_luminosas->reiniciar(m_configuracion->dispositivo_salida());

	//Reinicia la salida
	if(m_configuracion->dispositivo_salida() != NULL)
		m_configuracion->dispositivo_salida()->Reset();

	//Descarta texto inicial de archivo midi
	m_descartar_texto_inicial = true;
}

void VentanaOrgano::insertar_nota_activa(unsigned int id_nota, unsigned char canal, unsigned long int pista, unsigned long int posicion, Color color, bool sonido, bool correcta)
{
	Nota_Activa *nota_nueva = m_notas_activas[id_nota];

	if(nota_nueva == NULL)
	{
		nota_nueva = new Nota_Activa();
		nota_nueva->id_nota = id_nota;
		nota_nueva->canal = canal;
		nota_nueva->pista = pista;
		nota_nueva->posicion = posicion;
		nota_nueva->color = color;
		nota_nueva->sonido = sonido;
		nota_nueva->correcta = correcta;
		m_notas_activas[id_nota] = nota_nueva;
	}
	else
		nota_nueva->contador_clic++;
}

bool VentanaOrgano::fue_tocada(unsigned int id_nota)
{
	std::map<unsigned int, Nota_Activa*>::iterator nota = m_notas_activas.find(id_nota);
	if(nota != m_notas_activas.end())
	{
		if(nota->second->correcta)
			return true;
	}
	return false;
}

void VentanaOrgano::agregar_nota_requerida(unsigned int id_nota, const Color &color)
{
	m_teclas_luminosas->encender(id_nota, m_configuracion->dispositivo_salida());
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
			if((respuesta != m_notas_activas.end() && !respuesta->second->correcta) || respuesta == m_notas_activas.end())
				return;//La nota se toco antes de tiempo (ploma) o no fue tocada
		}

		for(std::pair<unsigned int, Color> valor : m_notas_requeridas)
		{
			//Apagando luces
			m_teclas_luminosas->apagar(valor.first, m_configuracion->dispositivo_salida());
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
		m_musica->reiniciar();
		m_accion = CambiarASeleccionPista;
	}
	else if(tecla == TECLA_FLECHA_ARRIBA && estado)
	{
		m_tablero->duracion_nota(1);
		m_duracion_nota = m_tablero->duracion_nota();
		m_configuracion->duracion_nota(m_duracion_nota);
	}
	else if(tecla == TECLA_FLECHA_ABAJO && estado)
	{
		m_tablero->duracion_nota(-1);
		m_duracion_nota = m_tablero->duracion_nota();
		m_configuracion->duracion_nota(m_duracion_nota);
	}
	else if(tecla == TECLA_RESTA_NUMERICO && estado)
	{
		m_volumen-=0.1;
		m_volumen_antes_mute = m_volumen;
		if(m_volumen <= 0)
		{
			m_volumen = 0;
			m_volumen_antes_mute = 0.1;
		}
		m_configuracion->volumen(m_volumen);
	}
	else if(tecla == TECLA_SUMA_NUMERIO && estado)
	{
		m_volumen+=0.1;
		if(m_volumen > 2)
			m_volumen = 2;
		m_volumen_antes_mute = m_volumen;
		m_configuracion->volumen(m_volumen);
	}
	else if(tecla == TECLA_M && estado)
	{
		if(m_volumen > 0)
		{
			m_volumen_antes_mute = m_volumen;
			m_volumen = 0;
		}
		else
			m_volumen = m_volumen_antes_mute;
		m_configuracion->volumen(m_volumen);
	}
	else if(tecla == TECLA_FLECHA_IZQUIERDA && estado)
	{
		m_velocidad_musica-=0.01;
		if(m_velocidad_musica < 0.01)
			m_velocidad_musica = 0.01;
		m_cambio_velocidad = true;
		m_configuracion->velocidad(m_velocidad_musica);
	}
	else if(tecla == TECLA_FLECHA_DERECHA && estado)
	{
		m_velocidad_musica+=0.01;
		if(m_velocidad_musica > 2)
			m_velocidad_musica = 2;
		m_cambio_velocidad = true;
		m_configuracion->velocidad(m_velocidad_musica);
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
		m_configuracion->subtitulos(m_mostrar_subtitulo);
		//Actualiza el subtitulo
		if(m_mostrar_subtitulo)
			m_subtitulos.texto(Texto::quitar_espacios_en_extremos(m_subtitulo_texto));
	}
	else if(tecla == TECLA_F5 && estado)
	{
		m_teclado_visible.cambiar(48, 37);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_F6 && estado)
	{
		m_teclado_visible.cambiar(36, 49);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_F7 && estado)
	{
		m_teclado_visible.cambiar(36, 61);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_F8 && estado)
	{
		m_teclado_visible.cambiar(28, 76);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_F9 && estado)
	{
		m_teclado_visible.cambiar(21, 88);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_INSERTAR && estado)
	{
		m_teclado_visible.tecla_inicial(m_teclado_visible.tecla_inicial()+1);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_SUPRIMIR && estado)
	{
		m_teclado_visible.tecla_inicial(m_teclado_visible.tecla_inicial()-1);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_REPAG && estado)
	{
		m_teclado_visible.numero_teclas(m_teclado_visible.numero_teclas()+1);
		cambio_teclado = true;
	}
	else if(tecla == TECLA_AVPAG && estado)
	{
		m_teclado_visible.numero_teclas(m_teclado_visible.numero_teclas()-1);
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
		m_tablero->reiniciar();
		m_configuracion->teclado_visible(m_teclado_visible.tecla_inicial(), m_teclado_visible.numero_teclas());
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
