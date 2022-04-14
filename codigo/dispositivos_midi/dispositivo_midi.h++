#ifndef DISPOSITIVO_MIDI
#define DISPOSITIVO_MIDI

#include "teclas_luminosas.h++"
#include "../control/rango_organo.h++"

#include <string>
#include <map>

#define ENTRADA 1 << 0
#define SALIDA 1 << 1

class Dispositivo_Midi
{
private:
	unsigned char m_cliente;
	unsigned char m_puerto;
	unsigned char m_capacidad;//Lectura, escritura o ambos
	unsigned char m_capacidad_activa;
	std::string m_nombre;

	bool m_conectado;
	bool m_habilitado;
	bool m_cambio_cliente;
	bool m_reenviar_programa;

	//Entrada
	bool m_sensitivo;
	double m_volumen_entrada;
	Rango_Organo m_rango_teclado;
	std::vector<unsigned char> m_notas_entrada;

	//Salida
	double m_volumen_salida;
	Teclas_Luminosas *m_teclas_luminosas;
	std::map<unsigned char, unsigned int> m_notas_salida;

	void inicializar();

public:
	Dispositivo_Midi();
	Dispositivo_Midi(const Dispositivo_Midi &dispositivo);
	Dispositivo_Midi(unsigned char cliente, unsigned char puerto, unsigned char capacidad, const std::string &nombre, bool conectado);
	~Dispositivo_Midi();

	void cliente(unsigned char cliente);
	unsigned char cliente() const;
	void puerto(unsigned char puerto);
	unsigned char puerto() const;
	void capacidad(unsigned char capacidad);
	unsigned char capacidad() const;
	void capacidad_activa(unsigned char capacidad);
	unsigned char capacidad_activa() const;
	void nombre(const std::string &nombre);
	std::string nombre() const;

	void conectado(bool estado);
	bool conectado() const;
	void habilitado(bool estado);
	bool habilitado() const;
	void cambio_cliente(bool estado);
	bool cambio_cliente() const;
	void reenviar_programa(bool valor);
	bool reenviar_programa() const;

	bool es_entrada() const;
	bool entrada_activa() const;
	void sensitivo(bool estado);
	bool sensitivo() const;
	void volumen_entrada(double valor);
	double volumen_entrada() const;
	void rango_teclado(const std::string &rango);
	Rango_Organo rango_teclado() const;
	void nota_entrada(unsigned char id_nota, bool encendida);
	std::vector<unsigned char> notas_entrada() const;

	bool es_salida() const;
	bool salida_activa() const;
	void volumen_salida(double valor);
	double volumen_salida() const;
	void teclas_luminosas(unsigned int identificador);
	Teclas_Luminosas *teclas_luminosas() const;
	unsigned int id_teclas_luminosas() const;
	void nota_salida(unsigned char canal, bool encendida);
	std::map<unsigned char, unsigned int> notas_salida() const;

	void copiar_configuracion(const Dispositivo_Midi &datos);
	void limpiar();

	Dispositivo_Midi& operator = (const Dispositivo_Midi &dispositivo);
};

#endif
