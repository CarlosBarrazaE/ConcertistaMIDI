#ifndef DATOS_MUSICA_H
#define DATOS_MUSICA_H

#include "../control/pista.h++"
#include "../control/tiempos_nota.h++"
#include "../libmidi/Midi.h++"
#include "../elementos_graficos/notificacion.h++"

#include <vector>
#include <map>

class Datos_Musica
{
private:
	Midi *m_musica;
	std::vector<Pista> m_pistas;
	std::map<unsigned long int, std::vector<Tiempos_Nota>> m_evaluacion;

	std::string m_nombre_musica;
	std::string m_autor;
public:
	Datos_Musica();
	~Datos_Musica();

	bool cargar_midi(std::string direccion);
	void pistas(std::vector<Pista> pistas);
	void nombre_musica(std::string nombre);
	void autor(std::string autor);

	void reiniciar();
	void reiniciar(microseconds_t espera_inicio);

	Midi *musica();
	std::vector<Pista> *pistas();
	std::map<unsigned long int, std::vector<Tiempos_Nota>> *evaluacion();

	std::string nombre_musica();
	std::string autor();
};

#endif
