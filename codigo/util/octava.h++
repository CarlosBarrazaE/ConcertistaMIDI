#ifndef OCTAVA_H
#define OCTAVA_H

namespace Octava
{
	bool es_blanca(unsigned char id_nota);
	unsigned char blancas_desde_inicio(unsigned char id_nota);
	unsigned char negras_desde_inicio(unsigned char id_nota);
	unsigned char numero_blancas(unsigned char inicio, unsigned char largo);
	unsigned char numero_negras(unsigned char inicio, unsigned char largo);
	unsigned char nota_id_desde_blanca(unsigned char blanca_actual);
	float desplazamiento_negra(unsigned char nota);
}

#endif
