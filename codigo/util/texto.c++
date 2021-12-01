#include "texto.h++"

namespace Texto
{
	std::vector<std::string> dividir_texto(const std::string &texto, char caracter)
	{
		std::vector<std::string> dividido;
		std::string temporal;
		for(unsigned long int x = 0; x<texto.length(); x++)
		{
			if(texto[x] == caracter)
			{
				if(temporal.length() > 0)
					dividido.push_back(temporal);
				temporal.clear();
			}
			else
			{
				temporal += texto[x];
			}
		}
		if(temporal.length() > 0)
			dividido.push_back(temporal);
		return dividido;
	}

	std::string convertir_a_utf8(const char *entrada, int largo_entrada, const char *nombre_codificacion)
	{
		UErrorCode  estado = U_ZERO_ERROR;
		UConverter *convertidor = ucnv_open(nombre_codificacion, &estado);

		//Se calcula el tamaño de la nueva cadena
		int32_t largo = ucnv_toUChars(convertidor, NULL, 0, entrada, largo_entrada, &estado);
		//Se omite el error evidente de desbordamiento, no conozco otra forma de calcular el nuevo tamaño
		if(U_FAILURE(estado) && estado != U_BUFFER_OVERFLOW_ERROR)
		{
			Registro::Error("Fallo la conversion: " + std::string(u_errorName(estado)));
			ucnv_close(convertidor);
			return "";
		}

		//Se construye la nueva cadena
		UChar *cadena_salida = new UChar[largo+1];
		estado = U_ZERO_ERROR;
		ucnv_toUChars(convertidor, cadena_salida, largo, entrada, largo_entrada, &estado);
		if(U_FAILURE(estado))
		{
			Registro::Error("Fallo la conversion: " + std::string(u_errorName(estado)));
			ucnv_close(convertidor);
			delete[] cadena_salida;
			return "";
		}

		//Se convierte a un string
		std::string resultado;
		icu::UnicodeString unicode = icu::UnicodeString(cadena_salida, largo);
		unicode.toUTF8String(resultado);

		ucnv_close(convertidor);
		delete[] cadena_salida;

		return resultado;
	}

	bool esta_vacio(const std::string &texto)
	{
		//Verifica si la cadena de texto esta vacia
		for(unsigned long int x=0; x<texto.length(); x++)
		{
			if(texto[x] != ' ' && texto[x] != '\t' && texto[x] != '\r' && texto[x] != '\n')
				return false;
		}
		return true;
	}

	std::string bytes_a_texto(unsigned long int tamanno)
	{
		if(tamanno < 1024)
			return std::to_string(tamanno) + " B";
		else if (tamanno >= 1024 && tamanno < 1024*1024)
			return std::to_string(tamanno/1024) + " KB";
		else if (tamanno >= (1024*1024) && tamanno < 1024*1024*1024)
			return std::to_string(tamanno/(1024*1024)) + " MB";
		else
			return std::to_string(tamanno/(1024*1024*1024)) + " GB";
	}

	std::string primera_letra_mayuscula(const std::string &texto)
	{
		icu::UnicodeString texto_transformado = texto.c_str();
		texto_transformado.toTitle(0).trim();

		std::string resultado = "";
		texto_transformado.toUTF8String(resultado);

		return resultado;
	}

	std::string quitar_espacios_en_extremos(std::string texto)
	{
		//Quita los espacios en blanco al principio y al final
		texto.erase(0, texto.find_first_not_of(" \t\n\r"));//Limpia el comienzo
		texto.erase(texto.find_last_not_of(" \t\n\r")+1);//Limpia el final
		return texto;
	}

	bool contiene_caracter(const std::string &texto, char caracter)
	{
		for(unsigned long int x = 0; x<texto.length(); x++)
		{
			if(texto[x] == caracter)
				return true;
		}
		return false;
	}
}
