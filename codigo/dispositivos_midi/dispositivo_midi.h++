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
	int m_cliente;
	int m_puerto;
	unsigned char m_capacidad;//Lectura, escritura o ambos
	unsigned char m_capacidad_habilitada;
	std::string m_nombre;
	bool m_conectado;
	bool m_habilitado;
	bool m_reenviar_programa;

	//Entrada
	float m_volumen_entrada;
	bool m_sensitivo;
	Teclado_Organo m_rango_teclado;

	//Salida
	float m_volumen_salida;
	Teclas_Luminosas *m_teclas_luminosas;
	std::map<unsigned char, std::vector<unsigned char>> m_notas_activas;

public:
	Dispositivo_Midi(int cliente, int puerto, unsigned char capacidad, const std::string &nombre, bool conectado);
	~Dispositivo_Midi();

	void cliente(int cliente);
	int cliente();

	int puerto();

	unsigned char capacidad();

	void modo(unsigned char modo);
	unsigned char modo();

	std::string nombre();

	void conectado(bool estado);
	bool conectado();

	void habilitado(bool estado);
	bool habilitado();

	void reenviar_programa(bool valor);
	bool reenviar_programa();

	void teclas_luminosas(unsigned int identificador);
	Teclas_Luminosas *teclas_luminosas();

	void rango_teclado(const std::string &rango);
	Teclado_Organo rango_teclado();

	void volumen_entrada(float valor);
	float volumen_entrada();

	void volumen_salida(float valor);
	float volumen_salida();

	void sensitivo(bool estado);
	bool sensitivo();
	//void conectar();
	//void desconectar();

	//bool es_entrada();
	//bool es_salida();
	void activar_tecla(unsigned char canal, unsigned char id_nota);
	void desactivar_tecla(unsigned char canal, unsigned char id_nota);
	std::map<unsigned char, std::vector<unsigned char>> teclas_activas() const;
	void limpiar();
};

#endif
