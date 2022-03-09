#ifndef DISPOSITIVO_MIDI
#define DISPOSITIVO_MIDI

#include "teclas_luminosas.h++"
#include "../control/teclado_organo.h++"

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
	bool m_reenviar_programa;

	//Entrada
	bool m_sensitivo;
	double m_volumen_entrada;
	Teclado_Organo m_rango_teclado;
	std::vector<unsigned char> m_notas_entrada;

	//Salida
	double m_volumen_salida;
	Teclas_Luminosas *m_teclas_luminosas;
	std::map<unsigned char, unsigned int> m_notas_salida;

public:
	Dispositivo_Midi(unsigned char cliente, unsigned char puerto, unsigned char capacidad, const std::string &nombre, bool conectado);
	~Dispositivo_Midi();

	void cliente(unsigned char cliente);
	unsigned char cliente();
	unsigned char puerto();
	void capacidad_activa(unsigned char capacidad);
	std::string nombre();

	void conectado(bool estado);
	bool conectado();
	void habilitado(bool estado);
	bool habilitado();
	void reenviar_programa(bool valor);
	bool reenviar_programa();

	bool es_entrada();
	bool entrada_activa();
	void sensitivo(bool estado);
	bool sensitivo();
	void volumen_entrada(double valor);
	double volumen_entrada();
	void rango_teclado(const std::string &rango);
	Teclado_Organo rango_teclado();
	void nota_entrada(unsigned char id_nota, bool encendida);
	std::vector<unsigned char> notas_entrada() const;

	bool es_salida();
	bool salida_activa();
	void volumen_salida(double valor);
	double volumen_salida();
	void teclas_luminosas(unsigned int identificador);
	Teclas_Luminosas *teclas_luminosas();
	void nota_salida(unsigned char canal, bool encendida);
	std::map<unsigned char, unsigned int> notas_salida() const;

	void limpiar();
};

#endif
