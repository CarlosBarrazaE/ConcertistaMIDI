#ifndef TECLADO_ORGANO
#define TECLADO_ORGANO

#define PROPORCION_ANCHO_NEGRA 0.666667f

#include "../util/texto.h++"
#include <vector>

class Teclado_Organo
{
private:
	unsigned char m_tecla_inicial = 0;
	unsigned char m_numero_teclas = 128;
public:
	Teclado_Organo();
	void cambiar(unsigned char inicial, unsigned char largo);

	unsigned char tecla_inicial();
	void tecla_inicial(unsigned char inicial);
	unsigned char tecla_final();

	unsigned char numero_teclas();
	void numero_teclas(unsigned char largo);

	void cargar(std::string datos);
	std::string texto();

	bool operator == (const Teclado_Organo &tc) const;
	bool operator != (const Teclado_Organo &tc) const;
	bool operator < (const Teclado_Organo &tc) const;
	bool operator <= (const Teclado_Organo &tc) const;
	bool operator > (const Teclado_Organo &tc) const;
	bool operator >= (const Teclado_Organo &tc) const;
};

#endif
