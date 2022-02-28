#include "ventana_organo_libre.h++"

VentanaOrganoLibre::VentanaOrganoLibre(Configuracion *configuracion, Administrador_Recursos *recursos) : Ventana()
{
	m_rectangulo = recursos->figura(F_Rectangulo);
	m_configuracion = configuracion;
	m_controlador_midi = m_configuracion->controlador_midi();

	m_teclado_visible = m_configuracion->teclado_visible();
	m_teclado_util = m_configuracion->teclado_util();

	m_organo = new Organo(0, Pantalla::Alto, Pantalla::Ancho, &m_teclado_visible, &m_teclado_util, recursos);
	m_tablero = new Tablero_Notas(0, 40, Pantalla::Ancho, Pantalla::Alto - (m_organo->alto() + 40), &m_teclado_visible, &m_teclado_util, NULL, recursos);
	m_tablero->desplazamiento(Subir);

	m_organo->notas_activas(&m_color_teclas);
	//m_tablero->notas(m_notas);
	m_pistas.push_back(Pista("Pista Midi", 1, 1, Pista::Colores_pista[0], Tocar, true, true));
	m_tablero->pistas(&m_pistas);
	//m_tablero->lineas(m_musica->musica()->GetBarLines());
}

VentanaOrganoLibre::~VentanaOrganoLibre()
{
	m_controlador_midi->reiniciar();

	delete m_tablero;
	delete m_organo;
}

void VentanaOrganoLibre::insertar_nota(unsigned int id_nota)
{
	Nota_Activa *nota_nueva = m_notas_activas[id_nota];

	if(nota_nueva == NULL)
	{
		nota_nueva = new Nota_Activa();
		nota_nueva->id_nota = id_nota;
		nota_nueva->color = Pista::Colores_pista[0];
		m_notas_activas[id_nota] = nota_nueva;
	}
	else
		nota_nueva->contador_clic++;
}

void VentanaOrganoLibre::eliminar_nota(unsigned int id_nota)
{
	std::map<unsigned int, Nota_Activa*>::iterator nota = m_notas_activas.find(id_nota);
	if(nota != m_notas_activas.end())
	{
		if(nota->second->contador_clic == 0)
		{
			m_notas_activas.erase(nota->second->id_nota);
			delete nota->second;
		}
		else
			nota->second->contador_clic--;
	}
}

void VentanaOrganoLibre::actualizar(unsigned int diferencia_tiempo)
{
	//m_tablero->tiempo(diferencia_tiempo);

	m_organo->actualizar(diferencia_tiempo);
	m_tablero->actualizar(diferencia_tiempo);

	//Lee todos los eventos
	while(m_controlador_midi->hay_eventos())
	{
		Evento_Midi evento = m_controlador_midi->leer();
		/*Registro::Aviso("Evento capturado: " + std::to_string(evento.Type()));
		if(evento.Type() == MidiEventType_Meta)
		{
			Registro::Nota("Evento meta: " + std::to_string(evento.MetaType()));
			if(evento.HasText())
			{
				Registro::Depurar(evento.Text());
			}
		}*/

		//Omitir eventos que no son NoteOn o NoteOff
		if(evento.tipo_evento() != EventoMidi_NotaEncendida && evento.tipo_evento() != EventoMidi_NotaApagada)
			continue;

		if(evento.tipo_evento() == EventoMidi_NotaEncendida && evento.velocidad_nota() > 0)
			this->insertar_nota(evento.id_nota());
		else
			this->eliminar_nota(evento.id_nota());
	}

	//Agregar al organo el color de las teclas presionada
	for(std::pair<unsigned int, Nota_Activa*> valor : m_notas_activas)
		m_color_teclas[valor.second->id_nota] = valor.second->color;
}

void VentanaOrganoLibre::dibujar()
{
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(0, 0, Pantalla::Ancho, 40, Color(0.141f, 0.624f, 0.933f));

	m_tablero->dibujar();
	m_organo->dibujar();
}

void VentanaOrganoLibre::evento_raton(Raton *raton)
{
	m_tablero->evento_raton(raton);
	m_organo->evento_raton(raton);
}

void VentanaOrganoLibre::evento_teclado(Tecla tecla, bool estado)
{
	if(tecla == TECLA_ESCAPE && estado)
		m_accion = CambiarATitulo;
}

void VentanaOrganoLibre::evento_pantalla(float ancho, float alto)
{
	m_tablero->dimension(ancho, alto - (m_organo->alto() + 40));
	m_organo->posicion(m_organo->x(), alto);
	m_organo->dimension(ancho, m_organo->alto());
}
