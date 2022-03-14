#ifndef DATOS_DISPOSITIVO_H
#define DATOS_DISPOSITIVO_H

struct Datos_Dispositivo
{
	unsigned char cliente = 0;
	unsigned char puerto = 0;
	std::string nombre;
	unsigned char capacidad = 0;
	unsigned char capacidad_activa = 0;
	bool habilitado = false;
	bool sensitivo = true;
	double volumen_entrada = 1.0;
	std::string rango_teclado;
	double volumen_salida = 1.0;
	unsigned int teclas_luminosas = 0;
};

#endif
