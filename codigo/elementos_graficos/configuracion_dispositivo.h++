#ifndef CONFIGURACION_DISPOSITIVO_H
#define CONFIGURACION_DISPOSITIVO_H

#include "elemento.h++"
#include "etiqueta.h++"
#include "boton.h++"
#include "casilla_verificacion.h++"
#include "lista_opciones.h++"
#include "control_deslizante.h++"
#include "../dispositivos_midi/dispositivo_midi.h++"

class Configuracion_Dispositivo : public Elemento
{
private:
	Dispositivo_Midi *m_dispositivo;

	Administrador_Recursos *m_recursos;
	Rectangulo *m_rectangulo;
	Textura2D *m_flecha_abajo, *m_flecha_arriba;

	//Elementos minimos
	Boton m_desplegar;
	Etiqueta m_nombre;
	Casilla_Verificacion m_habilitado;

	//Elementos
	Etiqueta *m_texto_tipo_dispositivo;
	Lista_Opciones *m_opciones_tipo_dispositivo;
	//Elementos Entrada
	Etiqueta *m_texto_entrada;
	Etiqueta *m_texto_sensitivo;
	Etiqueta *m_texto_volumen_entrada;
	Etiqueta *m_texto_rango;
	Casilla_Verificacion *m_sensitivo;
	Control_Deslizante *m_volumen_entrada;
	Lista_Opciones *m_rango_teclado;

	//Elementos Salida
	Etiqueta *m_texto_salida;
	Etiqueta *m_texto_volumen_salida;
	Etiqueta *m_texto_teclado_luminoso;
	Control_Deslizante *m_volumen_salida;
	Lista_Opciones *m_teclado_luminoso;

	//Control
	bool m_mostrar_configuracion, m_elementos_creados;
	bool m_cambio_altura;
	float m_alto_minimo, m_alto_maximo;
	int m_direccion;
	bool m_mostrar_linea;

	void crear_nuevos_elementos();

public:
	Configuracion_Dispositivo(float x, float y, float ancho, Dispositivo_Midi *dispositivo, Administrador_Recursos *recursos);
	~Configuracion_Dispositivo();

	void actualizar(unsigned int diferencia_tiempo) override;
	void dibujar() override;
	void evento_raton(Raton *raton) override;

	void posicion(float x, float y) override;
	//void dimension(float ancho, float alto) override;

	bool cambio_altura();
};

#endif
