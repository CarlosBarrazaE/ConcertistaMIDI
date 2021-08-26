#include "ventana_organo_libre.h++"

VentanaOrganoLibre::VentanaOrganoLibre(Configuracion *configuracion, Administrador_Recursos *recursos) : Ventana()
{
	m_rectangulo = recursos->figura(F_Rectangulo);
	m_configuracion = configuracion;
}

VentanaOrganoLibre::~VentanaOrganoLibre()
{
}

void VentanaOrganoLibre::actualizar(unsigned int /*diferencia_tiempo*/)
{
}

void VentanaOrganoLibre::dibujar()
{
}

void VentanaOrganoLibre::evento_raton(Raton */*raton*/)
{
}

void VentanaOrganoLibre::evento_teclado(Tecla tecla, bool estado)
{
	if(tecla == TECLA_ESCAPE && estado)
		m_accion = CambiarATitulo;
}

void VentanaOrganoLibre::evento_pantalla(float /*ancho*/, float /*alto*/)
{
}
