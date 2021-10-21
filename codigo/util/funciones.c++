#include "funciones.h++"

namespace Funciones
{
	std::string microsegundo_a_texto(microseconds_t ms, bool mostrar_vacio)
	{
		int seg  = static_cast<int>(ms / 1000000);
		int horas = seg / 3600;
		int minutos = (seg - (horas * 3600)) / 60;
		int segundos =  (seg - (horas * 3600 + minutos * 60));

		std::string tminutos, tsegundos;
		if(minutos < 10)
			tminutos = "0" + std::to_string(minutos);
		else
			tminutos = std::to_string(minutos);

		if(segundos < 10)
			tsegundos = "0" + std::to_string(segundos);
		else
			tsegundos = std::to_string(segundos);

		if(!mostrar_vacio && horas == 0)
			return tminutos + ":" + tsegundos;
		else
			return std::to_string(horas) + ":" + tminutos + ":" + tsegundos;
	}

	microseconds_t duracion_midi(const std::string &ruta_midi)
	{
		Midi *archivo_midi = NULL;
		try
		{
			archivo_midi = new Midi(Midi::ReadFromFile(ruta_midi));
		}
		catch(const MidiError &e)
		{
			Registro::Error("No se puede abrir el archivo MIDI " + e.GetErrorDescription());
			archivo_midi = NULL;
		}
		if(archivo_midi != NULL)
		{
			microseconds_t duracion = archivo_midi->GetSongLengthInMicroseconds();//Tiempo en segundos leer midi
			delete archivo_midi;
			return duracion;
		}
		return 0;
	}

	bool comparar_float(float valor1, float valor2, float diferencia_minima)
	{
		if(valor1 < valor2 + diferencia_minima && valor1 > valor2 - diferencia_minima)
			return true;
		return false;
	}

	std::string nombre_archivo(const std::string &ruta, bool carpeta)
	{
		unsigned long int extencion = ruta.length()-1;
		for(unsigned long int i=ruta.length(); i>0; i--)
		{
			//Encuentra el primer punto
			if(extencion == ruta.length()-1 && ruta[i-1] == '.')
				extencion = i-1;
			//Inicio del nombre del archivo
			if(ruta[i-1] == '/' && i-1 < ruta.length()-1)
			{
				if(carpeta)
					return ruta.substr(i);
				//Es archivo
				if(i-1 < extencion && extencion != ruta.length()-1)
					return ruta.substr(i, extencion-(i));
			}
		}
		return "";
	}

	std::string extencion_archivo(const std::string &nombre)
	{
		for(unsigned long int i=nombre.length()-1; i>0; i--)
		{
			if(nombre[i] == '.' && i < nombre.length()-1)
				return nombre.substr(i+1);
		}
		return "";
	}

	bool es_midi(const std::string &extencion)
	{
		if(extencion.length() == 3 &&
			(extencion[0] == 'M' || extencion[0] == 'm') &&
			(extencion[1] == 'I' || extencion[1] == 'i') &&
			(extencion[2] == 'D' || extencion[2] == 'd'))
			return true;
		if(extencion.length() == 3 &&
			(extencion[0] == 'K' || extencion[0] == 'k') &&
			(extencion[1] == 'A' || extencion[1] == 'a') &&
			(extencion[2] == 'R' || extencion[2] == 'r'))
			return true;
		else if(extencion.length() == 4 &&
			(extencion[0] == 'M' || extencion[0] == 'm') &&
			(extencion[1] == 'I' || extencion[1] == 'i') &&
			(extencion[2] == 'D' || extencion[2] == 'd') &&
			(extencion[3] == 'I' || extencion[3] == 'i'))
			return true;
		return false;
	}

	unsigned int numero_de_archivos(const std::string &carpeta)
	{
		unsigned int archivos = 0;
		if(!std::ifstream(carpeta))
			return 0;
		for(const std::filesystem::directory_entry &elemento : std::filesystem::directory_iterator(carpeta))
		{
			std::string ruta = std::string(elemento.path());
			std::string extencion = Funciones::extencion_archivo(ruta);
			if(elemento.is_directory() || Funciones::es_midi(extencion))
				archivos++;
		}
		return archivos;
	}
}
