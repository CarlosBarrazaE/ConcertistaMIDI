#include "ventana_seleccion_pista.h++"

VentanaSeleccionPista::VentanaSeleccionPista(Configuracion *configuracion, Datos_Musica *musica, Administrador_Recursos *recursos) : Ventana(), m_texto_titulo(recursos)
{
	m_configuracion = configuracion;
	m_musica = musica;

	m_rectangulo = recursos->figura(F_Rectangulo);

	m_boton_atras = new Boton(10, Pantalla::Alto - 32, 120, 25, "Atrás", LetraChica, recursos);
	m_boton_atras->color_boton(Color(0.9f, 0.9f, 0.9f));

	m_boton_continuar = new Boton(Pantalla::Ancho - 130, Pantalla::Alto - 32, 120, 25, "Continuar", LetraChica, recursos);
	m_boton_continuar->color_boton(Color(0.9f, 0.9f, 0.9f));

	m_texto_titulo.texto(m_musica->nombre_musica());
	m_texto_titulo.tipografia(recursos->tipografia(LetraTitulo));
	m_texto_titulo.color(Color(1.0f, 1.0f, 1.0f));
	m_texto_titulo.posicion(0, 0);
	m_texto_titulo.dimension(Pantalla::Ancho, 40);
	m_texto_titulo.centrado(true);

	m_panel_desplazamiento = new Panel_Desplazamiento(0, 50, Pantalla::Ancho, Pantalla::Alto - 100, 350, 150, 10, 10, recursos);

	if(m_musica->pistas()->size() > 0)
		this->cargar_configuracion(recursos);
	else
		this->crear_configuracion(recursos);
}

VentanaSeleccionPista::~VentanaSeleccionPista()
{
	delete m_boton_atras;
	delete m_boton_continuar;
	delete m_panel_desplazamiento;
	for(Elemento* e : m_configuracion_pistas)
		delete e;
}

void VentanaSeleccionPista::crear_configuracion(Administrador_Recursos *recursos)
{
	unsigned long int numero_pistas = m_musica->musica()->Notes().size();
	int color_usado = 0;
	Configuracion_Pista *configuracion;
	Color color_pista;
	bool visible = true;
	for(unsigned long int i=0; i<numero_pistas; i++)
	{
		MidiTrack pista_actual = m_musica->musica()->Tracks()[i];
		if(pista_actual.Notes().size() > 0)
		{
			if(pista_actual.IsPercussion())
			{
				color_pista = Pista::Colores_pista[0];
				visible = false;
			}
			else
			{
				color_pista = Pista::Colores_pista[1+color_usado % NUMERO_COLORES_PISTA];
				visible = true;
				color_usado++;
			}

			configuracion = new Configuracion_Pista(0, 0, 350, 150, Pista(pista_actual.InstrumentName(), pista_actual.Notes().size(), pista_actual.canal(), color_pista, Fondo, visible, true), recursos);
			m_configuracion_pistas.push_back(configuracion);
			m_panel_desplazamiento->agregar_elemento(configuracion);
		}
		else
		{
			configuracion = new Configuracion_Pista(0, 0, 350, 150, Pista(pista_actual.InstrumentName(), pista_actual.Notes().size(), 0, Color(0.0f, 0.0f, 0.0f), Fondo, false, false), recursos);
			m_configuracion_pistas.push_back(configuracion);
		}
	}
}

void VentanaSeleccionPista::cargar_configuracion(Administrador_Recursos *recursos)
{
	std::vector<Pista> *pistas = m_musica->pistas();
	Configuracion_Pista *configuracion;
	for(unsigned int i=0; i<pistas->size(); i++)
	{
		configuracion = new Configuracion_Pista(0, 0, 350, 150, pistas->at(i), recursos);
		m_configuracion_pistas.push_back(configuracion);
		if(pistas->at(i).numero_notas() > 0)
			m_panel_desplazamiento->agregar_elemento(configuracion);
	}
}

void VentanaSeleccionPista::guardar_configuracion()
{
	std::vector<Pista> pistas;

	for(unsigned int i=0; i<m_configuracion_pistas.size(); i++)
	{
		pistas.push_back(m_configuracion_pistas[i]->pista());
	}
	m_musica->pistas(pistas);
}

void VentanaSeleccionPista::actualizar(unsigned int diferencia_tiempo)
{
	m_boton_atras->actualizar(diferencia_tiempo);
	m_boton_continuar->actualizar(diferencia_tiempo);
	m_panel_desplazamiento->actualizar(diferencia_tiempo);
}

void VentanaSeleccionPista::dibujar()
{
	m_panel_desplazamiento->dibujar();
	m_rectangulo->textura(false);
	m_rectangulo->dibujar(0, 0, Pantalla::Ancho, 40, Color(0.141f, 0.624f, 0.933f));
	m_rectangulo->dibujar(0, Pantalla::Alto - 40, Pantalla::Ancho, 40, Color(0.761f, 0.887f, 0.985f));
	m_texto_titulo.dibujar();

	m_boton_atras->dibujar();
	m_boton_continuar->dibujar();
}

void VentanaSeleccionPista::evento_raton(Raton *raton)
{
	m_boton_atras->evento_raton(raton);
	m_boton_continuar->evento_raton(raton);
	m_panel_desplazamiento->evento_raton(raton);

	if(m_boton_atras->esta_activado())
		m_accion = CambiarASeleccionMusica;

	if(m_boton_continuar->esta_activado())
	{
		m_accion = CambiarAOrgano;
		this->guardar_configuracion();
	}
}

void VentanaSeleccionPista::evento_teclado(Tecla tecla, bool estado)
{
	if(tecla == TECLA_ESCAPE && !estado)
		m_accion = CambiarASeleccionMusica;
	else if(tecla == TECLA_ENTRAR && !estado)
	{
		m_accion = CambiarAOrgano;
		this->guardar_configuracion();
	}
}

void VentanaSeleccionPista::evento_pantalla(float ancho, float alto)
{
	m_texto_titulo.dimension(Pantalla::Ancho, 40);
	m_panel_desplazamiento->dimension(ancho, alto-100);
	m_boton_atras->posicion(m_boton_atras->x(), alto - 32);
	m_boton_continuar->posicion(ancho - 130, alto - 32);
}
