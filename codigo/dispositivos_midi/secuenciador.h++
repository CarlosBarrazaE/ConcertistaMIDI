#ifndef SECUENCIADOR_H
#define SECUENCIADOR_H

#include "../libreria_midi/evento_midi.h++"
#include "dispositivo_midi.h++"
#include <string>

#define DIRECCIONES_SUSCRITAS 254
#define VELOCIDAD_NORMAL 100

enum Tipo
{
	Entrada,
	Salida,
};

class Secuenciador
{
protected:
	unsigned char m_cliente;
	unsigned char m_puerto_entrada, m_puerto_salida, m_puerto_virtual;
	bool m_cambio_dispositivos;

public:
	Secuenciador();
	virtual ~Secuenciador();

	unsigned char cliente();

	virtual std::string nombre_dispositivo(unsigned char cliente, unsigned char puerto) const = 0;

	virtual void crear_lista_dispositivos(std::vector<Dispositivo_Midi*> &dispositivos) const = 0;
	virtual Dispositivo_Midi* crear_nuevo_dispositivo(unsigned char cliente, unsigned char puerto) const = 0;

	virtual bool conectar(unsigned char cliente, unsigned char puerto, Tipo tipo) const = 0;
	virtual bool desconectar(unsigned char cliente, unsigned char puerto, Tipo tipo) const = 0;

	virtual void escribir(const Evento_Midi &evento_salida) const = 0;
	virtual Evento_Midi leer() const = 0;
	virtual bool hay_eventos() const = 0;

	virtual void enviar_nota(unsigned char id_nota, bool estado) const = 0;
};

#endif
