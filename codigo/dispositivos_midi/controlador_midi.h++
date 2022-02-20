#ifndef CONTROLADOR_MIDI
#define CONTROLADOR_MIDI

#include "dispositivo_midi.h++"
#include "../libreria_midi/evento_midi.h++"
#include "../libmidi/MidiEvent.h++"

#include <alsa/asoundlib.h>
#include <map>

#define VELOCIDAD_NORMAL 100

enum Habilitado
{
	Indeterminado,
	Activo,
	Inactivo,
};

enum TipoDispositivo
{
	Entrada,
	Salida,
};

class Controlador_Midi
{
private:
	static unsigned int Bandera_entrada, Bandera_salida;

	std::vector<Dispositivo_Midi*> m_dispositivos;
	std::vector<Dispositivo_Midi*> m_entrada;
	std::vector<Dispositivo_Midi*> m_salida;

	snd_seq_t *m_secuenciador_alsa;
	int m_cliente;
	int m_puerto_entrada, m_puerto_salida, m_puerto_virtual;

	std::map<unsigned char, unsigned char> m_ultimo_programa;//canal,programa
	bool m_cambio_dispositivos;
	std::vector<std::string> m_mensajes;

	void mostrar_estado_alsa(int estado, const std::string &mensaje) const;
	void suscribir_puerto(int cliente_origen, int puerto_origen, int cliente_destino, int puerto_destino);
	std::string nombre_dispositivo(int cliente, int puerto) const;

	void crear_lista_dispositivos();
	void agregar_nuevo_dispositivo(int cliente, int puerto);
	Dispositivo_Midi *dispositivo(int cliente, int puerto, bool conectado, Habilitado habilitado, bool exacto);
	Dispositivo_Midi *dispositivo_activo(int cliente, int puerto, unsigned char capacidad);

	void escribir(const MidiEvent &evento, Dispositivo_Midi *dispositivo);
	void escribir(const Evento_Midi &evento, Dispositivo_Midi *dispositivo);
	void reenviar_programas(Dispositivo_Midi *dispositivo);

public:
	Controlador_Midi();
	~Controlador_Midi();

	Dispositivo_Midi* configurar_dispositivo(int cliente, int puerto, unsigned char capacidad, const std::string &nombre);
	void conectar(Dispositivo_Midi *dispositivo, bool conexion_fisica);
	void desconectar(Dispositivo_Midi *dispositivo, bool desconexion_fisica);

	bool hay_eventos();
	Evento_Midi leer();

	void escribir(const MidiEvent &evento);
	void escribir(const Evento_Midi &evento);
	void tecla_luninosa(unsigned char id_nota, bool estado);
	void enviar_nota(unsigned char id_nota, bool estado);

	void actualizar(unsigned int diferencia_tiempo, bool consumir_eventos);

	void reiniciar();//Limpiar luces con reiniciar igual
	void detener_eventos();

	//void lista();

	bool hay_mensajes();
	std::string siguiente_mensaje();

	bool hay_cambios_de_dispositivos();
	std::string dispositivos_conectados(unsigned char capacidad);
};

#endif
