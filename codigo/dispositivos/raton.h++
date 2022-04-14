#ifndef RATON_H
#define RATON_H

enum BotonRaton
{
	BotonIzquierdo,
	BotonCentral,
	BotonDerecho,
	Ninguno
};

enum EventoRaton
{
	EventoVacio,
	EventoPosicion,
	EventoClic,
	EventoDesplazamiento,
};

class Raton
{
private:
	int m_posicion_x, m_posicion_y;
	int m_desplazamiento_x, m_desplazamiento_y;
	bool m_boton_izquierdo, m_boton_central, m_boton_derecho;
	int m_numero_clics;
	EventoRaton m_ultimo_evento;
public:
	Raton();
	void actualizar_boton(BotonRaton boton, bool estado, int numero_clics);
	void actualizar_posicion(int x, int y);
	void actualizar_desplazamiento(int desplazamiento_x, int desplazamiento_y);
	void anular_desplazamiento();

	bool activado(BotonRaton boton) const;
	BotonRaton boton_activado() const;
	int numero_clics() const;
	int x() const;
	int y() const;
	int dx() const;
	int dy() const;
	bool ultimo_evento_fue(const EventoRaton &tipo) const;

	bool esta_sobre(float x, float y, float ancho, float alto) const;

	Raton& operator = (const Raton &r);
};

#endif
