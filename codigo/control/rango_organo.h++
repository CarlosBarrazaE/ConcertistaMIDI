#ifndef RANGO_ORGANO
#define RANGO_ORGANO

#define PROPORCION_ANCHO_NEGRA 0.666667f

#include "../util/texto.h++"
#include <vector>

class Rango_Organo
{
private:
	unsigned char m_tecla_inicial = 0;
	unsigned char m_numero_teclas = 128;
public:
	Rango_Organo();
	void cambiar(unsigned char inicial, unsigned char largo);

	unsigned char tecla_inicial() const;
	void tecla_inicial(unsigned char inicial);
	unsigned char tecla_final() const;

	unsigned char numero_teclas() const;
	void numero_teclas(unsigned char largo);

	void cargar(std::string datos);
	std::string texto();

	bool operator == (const Rango_Organo &tc) const;
	bool operator != (const Rango_Organo &tc) const;
	bool operator < (const Rango_Organo &tc) const;
	bool operator <= (const Rango_Organo &tc) const;
	bool operator > (const Rango_Organo &tc) const;
	bool operator >= (const Rango_Organo &tc) const;
};

#endif
