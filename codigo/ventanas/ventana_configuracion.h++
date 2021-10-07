#ifndef VENTANACONFIGURACION_H
#define VENTANACONFIGURACION_H

#include "ventana.h++"
#include "../elementos_graficos/panel_solapa.h++"
#include "../elementos_graficos/boton.h++"
#include "../elementos_graficos/casilla_verificacion.h++"
#include "../elementos_graficos/lista_opciones.h++"
#include "../elementos_graficos/tabla.h++"
#include "../elementos_graficos/selector_archivos.h++"
#include "../dispositivos/teclas_luminosas.h++"
#include "../control/configuracion.h++"
#include "../util/usuario.h++"

class VentanaConfiguracion : public Ventana
{
private:
	//Recursos
	Administrador_Recursos *m_recursos;
	Rectangulo *m_rectangulo;

	//Componentes
	Etiqueta m_texto_titulo;
	Boton *m_boton_atras;
	Panel_Solapa *m_solapa;
	Selector_Archivos *m_selector_archivos;

	//Solapa General (1)
	Etiqueta *m_solapa1_titulo, *m_solapa1_desarrollo;
	Etiqueta *m_solapa1_texto_restablecer, *m_solapa1_texto_limpiar, *m_solapa1_texto_borrar;
	Boton *m_solapa1_restablecer, *m_solapa1_limpiar_bd, *m_solapa1_borrar_db;
 	Casilla_Verificacion *m_solapa1_casilla_desarrollo, *m_solapa1_casilla_modo_alambre;

	//Solapa Carpetas MIDI (2)
	Etiqueta *m_solapa2_titulo;
	Tabla *m_solapa2_tabla;
	Boton *m_solapa2_agregar, *m_solapa2_eliminar;

	//Solapa Dispositivos (3)
	Etiqueta *m_solapa3_titulo;
	Etiqueta *m_solapa3_texto_entrada, *m_solapa3_texto_salida, *m_solapa3_texto_teclas_luminosas;
	Lista_Opciones *m_solapa3_opcion_entrada, *m_solapa3_opcion_salida, *m_solapa3_teclas_luminosas;

	//Solapa Video (4)
	Etiqueta *m_solapa4_titulo;
	Casilla_Verificacion *m_solapa4_casilla_pantalla_completa;

	//Datos
	Configuracion *m_configuracion;
	unsigned int m_ultima_solapa;
	unsigned int m_id_dispositivo_entrada;
	unsigned int m_id_entrada_anterior;

	unsigned int m_id_dispositivo_salida;
	unsigned int m_id_salida_anterior;

	unsigned int m_id_teclas_luminosas;
	unsigned int m_id_teclas_luminosas_anterior;

	std::vector<std::string> obtener_dispositivos(MidiCommDescriptionList lista);
	void guardar_configuracion();
	void cargar_tabla_carpetas();
	unsigned int limpiar_base_de_datos();

public:
	VentanaConfiguracion(Configuracion *configuracion, Administrador_Recursos *recursos);
	~VentanaConfiguracion();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;

	void evento_raton(Raton *raton) override;
	void evento_teclado(Tecla tecla, bool estado) override;
	void evento_pantalla(float ancho, float alto) override;
};
#endif
