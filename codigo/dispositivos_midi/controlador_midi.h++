#ifndef CONTROLADOR_MIDI
#define CONTROLADOR_MIDI

#include "secuenciador.h++"
#include "secuenciador_alsa.h++"
#include "dispositivo_midi.h++"
#include "../libreria_midi/evento_midi.h++"
#include "../libmidi/MidiEvent.h++"

#include <map>

class Controlador_Midi
{
private:
	Secuenciador *m_secuenciador;
	bool m_cambio_dispositivos;
	unsigned int m_desconectado_pendiente;

	std::vector<Dispositivo_Midi*> m_dispositivos;
	std::vector<Dispositivo_Midi*> m_entrada;
	std::vector<Dispositivo_Midi*> m_salida;

	std::map<unsigned char, unsigned char> m_ultimo_programa;//canal,programa
	std::vector<std::string> m_mensajes;

	Dispositivo_Midi *dispositivo(unsigned char cliente, unsigned char puerto, bool conectado, bool exacto);
	Dispositivo_Midi *dispositivo_activo(unsigned char cliente, unsigned char puerto, unsigned char capacidad);

	void reenviar_programas(Dispositivo_Midi *dispositivo);
	void detener_eventos(Dispositivo_Midi *dispositivo);

public:
	Controlador_Midi();
	~Controlador_Midi();

	Dispositivo_Midi* configurar_dispositivo(unsigned char cliente, unsigned char puerto, unsigned char capacidad, const std::string &nombre);
	Dispositivo_Midi* obtener_dispositivo(unsigned char cliente, unsigned char puerto);
	void conectar(Dispositivo_Midi *dispositivo);
	void desconectar(Dispositivo_Midi *dispositivo);
	void eliminar_dispositivo(Dispositivo_Midi *dispositivo);

	bool hay_eventos();
	Evento_Midi leer();

	void escribir(MidiEvent &evento_salida);
	void escribir(Evento_Midi &evento_salida);
	void tecla_luninosa(unsigned char id_nota, bool estado);
	void enviar_nota(unsigned char id_nota, bool estado);

	void actualizar(unsigned int diferencia_tiempo, bool consumir_eventos);

	void reiniciar();//Limpiar luces con reiniciar igual
	void detener_eventos();

	bool hay_mensajes();
	std::string siguiente_mensaje();

	std::vector<Dispositivo_Midi*> lista_dispositivos();
	bool hay_cambios_de_dispositivos();
	std::string dispositivos_conectados(unsigned char capacidad);
};

#endif
