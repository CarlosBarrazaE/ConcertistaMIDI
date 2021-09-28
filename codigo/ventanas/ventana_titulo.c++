#include "ventana_titulo.h++"

VentanaTitulo::VentanaTitulo(Administrador_Recursos *recursos) : Ventana(), m_texto_version(recursos)
{
	m_rectangulo = recursos->figura(F_Rectangulo);

	m_textura_fondo = recursos->textura(T_FondoTitulo);
	m_textura_titulo = recursos->textura(T_Titulo);

	m_boton_tocar_cancion = new Boton(Pantalla::Centro_horizontal(), 220, 250, 50, "Tocar una canción", recursos);
	m_boton_tocar_cancion->color_boton(Color(0.145f, 0.707f, 1.0f));
	m_boton_tocar_cancion->color_texto(Color(1.0f, 1.0f, 1.0f));
	m_boton_tocar_cancion->centrado(true);

	m_boton_tocar = new Boton(Pantalla::Centro_horizontal(), 280, 250, 50, "Tocar", recursos);
	m_boton_tocar->color_boton(Color(0.9f, 0.9f, 0.9f));
	m_boton_tocar->centrado(true);
	m_boton_tocar->habilitado(false);

	m_boton_configurar = new Boton(Pantalla::Centro_horizontal(), 340, 250, 50, "Configuración", recursos);
	m_boton_configurar->color_boton(Color(0.9f, 0.9f, 0.9f));
	m_boton_configurar->centrado(true);

	m_boton_salir = new Boton(Pantalla::Centro_horizontal(), 400, 250, 50, "Salir", recursos);
	m_boton_salir->color_boton(Color(0.9f, 0.9f, 0.9f));
	m_boton_salir->centrado(true);

	m_texto_version.texto("Versión: " + std::string(VERSION));
	m_texto_version.tipografia(recursos->tipografia(LetraChica));
	m_texto_version.color(Color(1.0f, 1.0f, 1.0f));
	m_texto_version.posicion(20, Pantalla::Alto - 26);
}

VentanaTitulo::~VentanaTitulo()
{
	delete m_boton_tocar_cancion;
	delete m_boton_tocar;
	delete m_boton_configurar;
	delete m_boton_salir;
}

void VentanaTitulo::actualizar(unsigned int diferencia_tiempo)
{
	m_boton_tocar_cancion->actualizar(diferencia_tiempo);
	m_boton_tocar->actualizar(diferencia_tiempo);
	m_boton_configurar->actualizar(diferencia_tiempo);
	m_boton_salir->actualizar(diferencia_tiempo);
}

void VentanaTitulo::dibujar()
{
	m_textura_fondo->activar();
	m_rectangulo->textura(true);
	m_rectangulo->color(Color(1.0f, 1.0f, 1.0f));
	m_rectangulo->dibujar(0, 0, Pantalla::Ancho, 150);
	m_rectangulo->dibujar(0, Pantalla::Alto - 40, Pantalla::Ancho, 40);

	m_textura_titulo->activar();
	m_rectangulo->dibujar(Pantalla::Centro_horizontal() - 256, 30, 512, 128);

	m_boton_tocar_cancion->dibujar();
	m_boton_tocar->dibujar();
	m_boton_configurar->dibujar();
	m_boton_salir->dibujar();

	m_texto_version.dibujar();
}

void VentanaTitulo::evento_raton(Raton *raton)
{
	m_boton_tocar_cancion->evento_raton(raton);
	m_boton_tocar->evento_raton(raton);
	m_boton_configurar->evento_raton(raton);
	m_boton_salir->evento_raton(raton);

	if(m_boton_tocar_cancion->esta_activado())
		m_accion = CambiarASeleccionMusica;
	else if(m_boton_tocar->esta_activado())
		m_accion = CambiarAOrganoLibre;
	else if(m_boton_configurar->esta_activado())
		m_accion = CambiarAConfiguracion;
	else if(m_boton_salir->esta_activado())
		m_accion = Salir;
}

void VentanaTitulo::evento_teclado(Tecla tecla, bool estado)
{
	if(tecla == TECLA_ESCAPE && estado)
		m_accion = Salir;
	else if(tecla == TECLA_ENTRAR && !estado)
		m_accion = CambiarASeleccionMusica;
}

void VentanaTitulo::evento_pantalla(float /*ancho*/, float alto)
{
	m_boton_tocar_cancion->posicion(Pantalla::Centro_horizontal(), m_boton_tocar_cancion->y());
	m_boton_tocar->posicion(Pantalla::Centro_horizontal(), m_boton_tocar->y());
	m_boton_configurar->posicion(Pantalla::Centro_horizontal(), m_boton_configurar->y());
	m_boton_salir->posicion(Pantalla::Centro_horizontal(), m_boton_salir->y());
	m_texto_version.posicion(20, alto - 26);
}
