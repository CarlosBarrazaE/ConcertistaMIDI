#ifndef BASE_DE_DATOS_H
#define BASE_DE_DATOS_H

#include <sqlite3.h>
#include <string>
#include <vector>

#include "../registro.h++"
#include "../version.h++"

class Base_de_Datos
{
	private:
		bool m_base_de_datos_abierta;
		sqlite3 *m_base_de_datos;
		bool consulta(const std::string &consulta);
		int consulta_int(const std::string &consulta);
		std::string consulta_texto(const std::string &consulta);
		std::vector<std::vector<std::string>> consulta_tabla(const std::string &consulta, int columnas);
		std::vector<std::string> consulta_fila(const std::string &consulta, int columnas);
	public:
		Base_de_Datos();
		~Base_de_Datos();
		bool abrir(const std::string &direccion);
		void crear();
		void actualizar();
		void iniciar_transaccion();
		void finalizar_transaccion();

		//Tabla configuracion
		bool escribir_configuracion(const std::string &atributo, const std::string &valor);
		std::string leer_configuracion(const std::string &atributo);

		//Tabla carpetas
		bool agregar_carpeta(const std::string &nombre, const std::string &ruta);
		std::vector<std::vector<std::string>> carpetas();
		bool eliminar_carpeta(const std::string &ruta);

		//Tabla carpeta seleccion
		void guardar_ultima_seleccion(const std::string &ruta, unsigned long int ultima_seleccion);
		unsigned long int leer_ultima_seleccion(const std::string &ruta);

		//Tabla archivos
		void agregar_archivo(const std::string &ruta, long int duracion);
		void actualizar_archivo(const std::string &ruta, long int duracion);
		void sumar_visita_archivo(const std::string &ruta);
		std::vector<std::string> datos_archivo(const std::string &ruta);
		std::vector<std::string> lista_archivos();
		void borrar_archivo(const std::string &ruta);
		void borrar_archivos();
};

#endif
