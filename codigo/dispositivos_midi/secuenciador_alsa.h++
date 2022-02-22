#ifndef SECUENCIADOR_ALSA_H
#define SECUENCIADOR_ALSA_H

#include "secuenciador.h++"

#include <alsa/asoundlib.h>

class Secuenciador_Alsa : public Secuenciador
{
private:
	snd_seq_t *m_secuenciador_alsa;

	static unsigned int Bandera_entrada, Bandera_salida;
	void mostrar_estado_alsa(int estado, const std::string &mensaje) const;
	void suscribir_puerto(int cliente_origen, int puerto_origen, int cliente_destino, int puerto_destino) const;
public:
	Secuenciador_Alsa();
	~Secuenciador_Alsa();

	std::string nombre_dispositivo(unsigned char cliente, unsigned char puerto) const;

	void crear_lista_dispositivos(std::vector<Dispositivo_Midi*> &dispositivos) const;
	Dispositivo_Midi* crear_nuevo_dispositivo(unsigned char cliente, unsigned char puerto) const;

	bool conectar(unsigned char cliente, unsigned char puerto, Tipo tipo) const;
	bool desconectar(unsigned char cliente, unsigned char puerto, Tipo tipo) const;

	void escribir(const Evento_Midi &evento_entrada) const;
	Evento_Midi leer() const;
	bool hay_eventos() const;

	void enviar_nota(unsigned char id_nota, bool estado) const;
};

#endif
