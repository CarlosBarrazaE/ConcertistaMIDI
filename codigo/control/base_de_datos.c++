#include "base_de_datos.h++"

Base_de_Datos::Base_de_Datos()
{
	m_base_de_datos_abierta = false;
}

Base_de_Datos::~Base_de_Datos()
{
	if(m_base_de_datos_abierta)
		sqlite3_close(m_base_de_datos);
}

bool Base_de_Datos::consulta(const std::string &consulta_entrada)
{
	if(!m_base_de_datos_abierta)
	{
		Registro::Error("La base de datos no esta abierta");
		return false;
	}
	char *error = 0;
	int respuesta = sqlite3_exec(m_base_de_datos, consulta_entrada.c_str(), NULL, 0, &error);
	if(respuesta != SQLITE_OK)
	{
		Registro::Error(std::string(error));
		sqlite3_free(error);
        return false;
	}
	return true;
}

int Base_de_Datos::consulta_int(const std::string &consulta)
{
	if(!m_base_de_datos_abierta)
	{
		Registro::Error("La base de datos no esta abierta");
		return 0;
	}
	sqlite3_stmt * respuesta_consulta;
	int respuesta = sqlite3_prepare(m_base_de_datos, consulta.c_str(), -1, &respuesta_consulta, NULL);
	int resultado = 0;
	if(respuesta == SQLITE_OK)
	{
		sqlite3_step(respuesta_consulta);
		resultado = sqlite3_column_int(respuesta_consulta, 0);
	}
	sqlite3_finalize(respuesta_consulta);
	return resultado;
}

std::string Base_de_Datos::consulta_texto(const std::string &consulta)
{
	if(!m_base_de_datos_abierta)
	{
		Registro::Error("La base de datos no esta abierta");
		return "";
	}
	sqlite3_stmt * respuesta_consulta;
	int respuesta = sqlite3_prepare(m_base_de_datos, consulta.c_str(), -1, &respuesta_consulta, NULL);
	if(respuesta == SQLITE_OK)
	{
		sqlite3_step(respuesta_consulta);
		if(sqlite3_column_text(respuesta_consulta, 0) != 0)
		{
			std::string texto = std::string(reinterpret_cast<const char*>(sqlite3_column_text(respuesta_consulta, 0)));
			sqlite3_finalize(respuesta_consulta);
			return texto;
		}
	}
	sqlite3_finalize(respuesta_consulta);
	return "";
}

std::vector<std::vector<std::string>> Base_de_Datos::consulta_tabla(const std::string &consulta, int columnas)
{
	sqlite3_stmt * respuesta_consulta;
	int respuesta = sqlite3_prepare(m_base_de_datos, consulta.c_str(), -1, &respuesta_consulta, NULL);
	std::vector<std::vector<std::string>> tabla;
	if(respuesta == SQLITE_OK)
	{
		sqlite3_step(respuesta_consulta);
		unsigned int x = 0;
		while(sqlite3_column_text(respuesta_consulta, 0))
		{
			tabla.push_back(std::vector<std::string>());
			for(int c=0; c<columnas; c++)
			{
				if(sqlite3_column_text(respuesta_consulta, c) != NULL)
					tabla[x].push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(respuesta_consulta, c))));
				else
					tabla[x].push_back("");
			}
			sqlite3_step(respuesta_consulta);
			x++;
		}
	}
	sqlite3_finalize(respuesta_consulta);
	return tabla;
}

std::vector<std::string> Base_de_Datos::consulta_fila(const std::string &consulta, int columnas)
{
	sqlite3_stmt * respuesta_consulta;
	int respuesta = sqlite3_prepare(m_base_de_datos, consulta.c_str(), -1, &respuesta_consulta, NULL);
	std::vector<std::string> fila;
	if(respuesta == SQLITE_OK)
	{
		sqlite3_step(respuesta_consulta);
		if(sqlite3_column_text(respuesta_consulta, 0))
		{
			for(int c=0; c<columnas; c++)
			{
				if(sqlite3_column_text(respuesta_consulta, c) != NULL)
					fila.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(respuesta_consulta, c))));
				else
					fila.push_back("");
			}
			sqlite3_step(respuesta_consulta);
		}
	}
	sqlite3_finalize(respuesta_consulta);
	return fila;
}

std::vector<std::string> Base_de_Datos::consulta_columna(const std::string &consulta)
{
	//Retorna siempre la primera columna
	sqlite3_stmt * respuesta_consulta;
	int respuesta = sqlite3_prepare(m_base_de_datos, consulta.c_str(), -1, &respuesta_consulta, NULL);
	std::vector<std::string> datos;
	if(respuesta == SQLITE_OK)
	{
		sqlite3_step(respuesta_consulta);
		while(sqlite3_column_text(respuesta_consulta, 0))
		{
			datos.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(respuesta_consulta, 0))));
			sqlite3_step(respuesta_consulta);
		}
	}
	sqlite3_finalize(respuesta_consulta);

	return datos;
}

bool Base_de_Datos::abrir(const std::string &direccion)
{
	int respuesta = sqlite3_open(direccion.c_str(), &m_base_de_datos);
	if(respuesta != SQLITE_OK)
	{
		Registro::Error("No se puede abrir o crear la base de datos.");
		m_base_de_datos_abierta = false;
		return true;
	}
	m_base_de_datos_abierta = true;
	return false;
}

void Base_de_Datos::crear()
{
	//Crea todas las tablas de la base de datos
	this->consulta("CREATE TABLE configuracion (atributo VARCHAR(30), valor TEXT)");
	this->consulta("CREATE TABLE carpetas (nombre VARCHAR(30) NOT NULL, ruta TEXT NOT NULL PRIMARY KEY)");
	this->consulta("CREATE TABLE seleccion (ruta TEXT NOT NULL PRIMARY KEY, seleccion INT DEFAULT 0, ruta_seleccion TEXT)");
	this->consulta("CREATE TABLE archivos (ruta TEXT NOT NULL PRIMARY KEY, visitas INT DEFAULT 0, duracion BIGINT DEFAULT 0, ultimo_acceso DATETIME)");

	this->escribir_configuracion("version_base_de_datos", VERSION_BASE_DE_DATOS);
	this->agregar_carpeta("Canciones", "../musica/");
}

void Base_de_Datos::actualizar()
{
	//Verificar version de la base de datos
	std::string version = this->leer_configuracion("version_base_de_datos");
	Registro::Nota("Versión de la base de datos: " + version);
	if(version != VERSION_BASE_DE_DATOS)
	{
		this->escribir_configuracion("version_base_de_datos", VERSION_BASE_DE_DATOS);
		if(version == "1.0")
		{
			this->consulta("CREATE TABLE seleccion (ruta TEXT NOT NULL PRIMARY KEY, seleccion INT DEFAULT 0, ruta_seleccion TEXT)");
		}
		Registro::Aviso("Base de datos actualizada de la versión: " + version + " a la versión " + VERSION_BASE_DE_DATOS);
	}
}

void Base_de_Datos::iniciar_transaccion()
{
	sqlite3_exec(m_base_de_datos, "BEGIN TRANSACTION;", NULL, NULL, NULL);
}

void Base_de_Datos::finalizar_transaccion()
{
	sqlite3_exec(m_base_de_datos, "END TRANSACTION;", NULL, NULL, NULL);
}

//Tabla configuracion
bool Base_de_Datos::escribir_configuracion(const std::string &atributo, const std::string &valor)
{
	if(atributo.size() > 30)
	{
		Registro::Error("Nombre del atributo '"+atributo+"' es demaciado largo, maximo 30");
		return false;
	}
	if(valor != "")
	{
		Registro::Depurar("Escribiendo el registro: " + atributo + "->"+valor);
		//No se aceptan atributos vacios
		if(this->leer_configuracion(atributo).size() == 0)
			this->consulta("INSERT INTO configuracion ('atributo', 'valor') VALUES ('"+atributo+"', '"+valor+"')");
		else
			this->consulta("UPDATE configuracion SET valor = '"+valor+"' WHERE atributo = '"+atributo+"'");
		return true;
	}
	else
	{
		Registro::Error("Atributo: " + atributo + " con valor vacio.");
		return false;
	}
}

std::string Base_de_Datos::leer_configuracion(const std::string &atributo)
{
	if(atributo.size() > 30)
	{
		Registro::Error("Nombre del atributo '"+atributo+"' es demaciado largo, maximo 30");
		return "";
	}
	return this->consulta_texto("SELECT valor FROM configuracion WHERE atributo = '"+atributo+"' LIMIT 1");
}

bool Base_de_Datos::agregar_carpeta(const std::string &nombre, const std::string &ruta)
{
	return this->consulta("INSERT INTO carpetas ('nombre', 'ruta') VALUES ('"+nombre+"', '"+ruta+"')");
}

std::vector<std::vector<std::string>> Base_de_Datos::carpetas()
{
	return this->consulta_tabla("SELECT nombre, ruta FROM carpetas", 2);
}

bool Base_de_Datos::eliminar_carpeta(const std::string &ruta)
{
	return this->consulta("DELETE FROM carpetas WHERE ruta = '"+ruta+"'");
}

void Base_de_Datos::guardar_ultima_seleccion(const std::string &ruta, unsigned long int ultima_seleccion, const std::string &ruta_seleccion)
{
	if(this->consulta_int("SELECT EXISTS(SELECT seleccion FROM seleccion WHERE ruta = '"+ruta+"' LIMIT 1)") > 0)
		this->consulta("UPDATE seleccion SET seleccion = '"+std::to_string(ultima_seleccion)+"', ruta_seleccion = '"+ruta_seleccion+"' WHERE ruta = '"+ruta+"'");
	else
		this->consulta("INSERT INTO seleccion ('ruta', 'seleccion', 'ruta_seleccion') VALUES ('"+ruta+"', '"+std::to_string(ultima_seleccion)+"', '"+ruta_seleccion+"')");
}

std::vector<std::string> Base_de_Datos::leer_ultima_seleccion(const std::string &ruta)
{
	return this->consulta_fila("SELECT seleccion, ruta_seleccion FROM seleccion WHERE ruta = '"+ruta+"' LIMIT 1", 2);;
}

void Base_de_Datos::agregar_archivo(const std::string &ruta, long int duracion)
{
	if(ruta.length() > 0)
		this->consulta("INSERT INTO archivos ('ruta', 'duracion') VALUES ('"+ruta+"', '"+std::to_string(duracion)+"')");
}

void Base_de_Datos::actualizar_archivo(const std::string &ruta, long int duracion)
{
	if(ruta.length() > 0)
		this->consulta("UPDATE archivos SET duracion = '"+std::to_string(duracion)+"' WHERE ruta = '"+ruta+"'");
}

void Base_de_Datos::sumar_visita_archivo(const std::string &ruta)
{
	if(ruta.length() > 0)
		this->consulta("UPDATE archivos SET visitas = visitas+1, ultimo_acceso = datetime('now', 'localtime') WHERE ruta = '"+ruta+"'");
}

std::vector<std::string> Base_de_Datos::datos_archivo(const std::string &ruta)
{
	std::vector<std::string> fila;
	if(ruta.length() > 0)
		fila = this->consulta_fila("SELECT visitas, duracion, ultimo_acceso FROM archivos WHERE ruta = '"+ruta+"' LIMIT 1", 3);
	return fila;
}

std::vector<std::string> Base_de_Datos::lista_archivos()
{
	return this->consulta_columna("SELECT ruta FROM archivos");
}

std::vector<std::string> Base_de_Datos::lista_seleccion()
{
	return this->consulta_columna("SELECT ruta FROM seleccion");
}

void Base_de_Datos::borrar_archivo(const std::string &ruta)
{
	if(ruta.length() > 0)
		this->consulta("DELETE FROM archivos WHERE ruta = '"+ruta+"' LIMIT 1");
}

void Base_de_Datos::borrar_archivos()
{
	this->consulta("DELETE FROM archivos");
}

void Base_de_Datos::borrar_seleccion(const std::string &ruta)
{
	if(ruta.length() > 0)
		this->consulta("DELETE FROM seleccion WHERE ruta = '"+ruta+"' LIMIT 1");
}

void Base_de_Datos::borrar_selecciones()
{
	this->consulta("DELETE FROM seleccion");
}
