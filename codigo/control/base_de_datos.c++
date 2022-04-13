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
		Registro::Error(consulta_entrada);
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
	this->consulta("CREATE TABLE configuracion (atributo TEXT NOT NULL, valor TEXT NOT NULL)");
	this->consulta("CREATE TABLE carpetas (nombre TEXT NOT NULL, ruta TEXT NOT NULL PRIMARY KEY)");
	this->consulta("CREATE TABLE seleccion (ruta TEXT NOT NULL PRIMARY KEY, seleccion INTEGER DEFAULT 0, ruta_seleccion TEXT NOT NULL)");
	this->consulta("CREATE TABLE archivos (ruta TEXT NOT NULL PRIMARY KEY, visitas INTEGER DEFAULT 0, duracion INTEGER DEFAULT 0, ultimo_acceso DATETIME)");
	this->consulta("CREATE TABLE dispositivos (	cliente INTEGER DEFAULT 0,"
												"puerto INTEGER DEFAULT 0,"
												"nombre TEXT NOT NULL,"
												"capacidad INTEGER DEFAULT 0,"
												"capacidad_activa INTEGER DEFAULT 0,"
												"habilitado INTEGER DEFAULT 0,"
												"sensitivo INTEGER DEFAULT 1,"
												"volumen_entrada REAL DEFAULT 1,"
												"rango_teclado TEXT NOT NULL,"
												"volumen_salida REAL DEFAULT 1,"
												"teclado_luminoso INTEGER DEFAULT 0,"
												"PRIMARY KEY(cliente, puerto, nombre))");

	this->escribir_configuracion("version_base_de_datos", VERSION_BASE_DE_DATOS);
	this->escribir_configuracion("ruta_instalacion", RUTA_ARCHIVOS);
	this->agregar_carpeta("Canciones", std::string(RUTA_ARCHIVOS) + "/musica/");

	Dispositivo_Midi teclado_y_raton;
	teclado_y_raton.cliente(129);
	teclado_y_raton.puerto(0);
	teclado_y_raton.nombre("Teclado y Ratón");
	teclado_y_raton.capacidad(ENTRADA);
	teclado_y_raton.capacidad_activa(ENTRADA);
	teclado_y_raton.habilitado(true);
	teclado_y_raton.sensitivo(false);
	teclado_y_raton.rango_teclado("48,24");
	this->agregar_dispositivo(teclado_y_raton);

	Dispositivo_Midi timidity;
	timidity.cliente(128);
	timidity.puerto(0);
	timidity.nombre("TiMidity");
	timidity.capacidad(SALIDA);
	timidity.capacidad_activa(SALIDA);
	timidity.habilitado(true);
	this->agregar_dispositivo(timidity);
}

void Base_de_Datos::actualizar()
{
	//Verificar version de la base de datos
	std::string version_actual = this->leer_configuracion("version_base_de_datos");
	std::string version_original = version_actual;
	Registro::Nota("Versión de la base de datos: " + version_actual);
	if(version_actual != VERSION_BASE_DE_DATOS)
	{
		bool actualizado = false;
		if(version_actual == "1.0")
		{
			actualizado = true;
			this->consulta("CREATE TABLE seleccion (ruta TEXT NOT NULL PRIMARY KEY, seleccion INT DEFAULT 0, ruta_seleccion TEXT)");
			version_actual = "1.1";
		}
		if(version_actual == "1.1")
		{
			actualizado = true;
			//No existia ruta de instalacion
			this->escribir_configuracion("ruta_instalacion", "..");
			version_actual = "1.2";
		}
		if(version_actual == "1.2")
		{
			actualizado = true;
			this->iniciar_transaccion();
			//No vale la pena intentar recuperar el dispositivo seleccionado
			this->consulta("DELETE FROM configuracion WHERE 	atributo = 'dispositivo_entrada'"
															"OR atributo = 'dispositivo_salida'"
															"OR atributo = 'teclado_util'"
															"OR atributo = 'teclas_luminosas'");

			//Actualiza tablas configuracion
			this->consulta("DELETE FROM configuracion WHERE atributo IS NULL OR valor IS NULL");
			this->consulta("ALTER TABLE configuracion RENAME TO configuracion_ant");
			this->consulta("CREATE TABLE configuracion (atributo TEXT NOT NULL, valor TEXT NOT NULL)");
			this->consulta("INSERT INTO configuracion (atributo, valor) SELECT atributo, valor FROM configuracion_ant");
			this->consulta("DROP TABLE configuracion_ant");

			//Actualiza tablas carpetas
			this->consulta("ALTER TABLE carpetas RENAME TO carpetas_ant");
			this->consulta("CREATE TABLE carpetas (nombre TEXT NOT NULL, ruta TEXT NOT NULL PRIMARY KEY)");
			this->consulta("INSERT INTO carpetas (nombre, ruta) SELECT nombre, ruta FROM carpetas_ant");
			this->consulta("DROP TABLE carpetas_ant");

			//Actualiza tabla selección
			this->consulta("DELETE FROM seleccion WHERE ruta_seleccion IS NULL");
			this->consulta("ALTER TABLE seleccion RENAME TO seleccion_ant");
			this->consulta("CREATE TABLE seleccion (ruta TEXT NOT NULL PRIMARY KEY, seleccion INTEGER DEFAULT 0, ruta_seleccion TEXT NOT NULL)");
			this->consulta("INSERT INTO seleccion (ruta, seleccion, ruta_seleccion) SELECT ruta, seleccion, ruta_seleccion FROM seleccion_ant");
			this->consulta("DROP TABLE seleccion_ant");

			//Actualiza tabla archivos
			this->consulta("ALTER TABLE archivos RENAME TO archivos_ant");
			this->consulta("CREATE TABLE archivos (ruta TEXT NOT NULL PRIMARY KEY, visitas INTEGER DEFAULT 0, duracion INTEGER DEFAULT 0, ultimo_acceso DATETIME)");
			this->consulta("INSERT INTO archivos (ruta, visitas, duracion, ultimo_acceso) SELECT ruta, visitas, duracion, ultimo_acceso FROM archivos_ant");
			this->consulta("DROP TABLE archivos_ant");

			//Crea la nueva tabla para almacenar la configuracion de los dispositivos
			this->consulta("CREATE TABLE dispositivos (	cliente INTEGER DEFAULT 0,"
														"puerto INTEGER DEFAULT 0,"
														"nombre TEXT NOT NULL,"
														"capacidad INTEGER DEFAULT 0,"
														"capacidad_activa INTEGER DEFAULT 0,"
														"habilitado INTEGER DEFAULT 0,"
														"sensitivo INTEGER DEFAULT 1,"
														"volumen_entrada REAL DEFAULT 1,"
														"rango_teclado TEXT NOT NULL,"
														"volumen_salida REAL DEFAULT 1,"
														"teclado_luminoso INTEGER DEFAULT 0,"
														"PRIMARY KEY(cliente, puerto, nombre))");

			Dispositivo_Midi teclado_y_raton;
			teclado_y_raton.cliente(129);
			teclado_y_raton.puerto(0);
			teclado_y_raton.nombre("Teclado y Ratón");
			teclado_y_raton.capacidad(ENTRADA);
			teclado_y_raton.capacidad_activa(ENTRADA);
			teclado_y_raton.habilitado(true);
			teclado_y_raton.sensitivo(false);
			teclado_y_raton.rango_teclado("48,24");
			this->agregar_dispositivo(teclado_y_raton);

			Dispositivo_Midi timidity;
			timidity.cliente(128);
			timidity.puerto(0);
			timidity.nombre("TiMidity");
			timidity.capacidad(SALIDA);
			timidity.capacidad_activa(SALIDA);
			timidity.habilitado(true);
			this->agregar_dispositivo(timidity);

			version_actual = "1.3";
			this->finalizar_transaccion();
		}
		if(actualizado)
		{
			Registro::Nota("Se actualizó la base de datos de la versión: " + version_original + " a la versión " + VERSION_BASE_DE_DATOS);
			this->escribir_configuracion("version_base_de_datos", VERSION_BASE_DE_DATOS);
		}
	}

	//Cambia la ruta de instalacion de la carpeta musica
	std::string ruta_actual = this->leer_configuracion("ruta_instalacion");
	std::string ruta_nueva = RUTA_ARCHIVOS;
	if(ruta_actual != ruta_nueva)
	{
		this->iniciar_transaccion();
		this->consulta("UPDATE carpetas SET ruta='"+ruta_nueva+"' || substr(ruta, length('"+ruta_actual+"')+1) WHERE ruta = '"+ruta_actual+"/musica/'");
		this->consulta("UPDATE archivos SET ruta='"+ruta_nueva+"' || substr(ruta, length('"+ruta_actual+"')+1) WHERE ruta LIKE '"+ruta_actual+"/musica/%'");
		this->consulta("UPDATE configuracion SET valor='"+ruta_nueva+"' || substr(valor, length('"+ruta_actual+"')+1) WHERE valor LIKE '"+ruta_actual+"/musica/%'");
		this->consulta("UPDATE seleccion SET ruta='"+ruta_nueva+"' || substr(ruta, length('"+ruta_actual+"')+1) WHERE ruta LIKE '"+ruta_actual+"/musica/%'");
		this->consulta("UPDATE seleccion SET ruta_seleccion='"+ruta_nueva+"' || substr(ruta_seleccion, length('"+ruta_actual+"')+1) WHERE ruta_seleccion LIKE '"+ruta_actual+"/musica/%'");
		this->finalizar_transaccion();
		//Ruta actualizada
		this->escribir_configuracion("ruta_instalacion", ruta_nueva);
		Registro::Nota("Ruta de instalación actualizada a: " + ruta_nueva);
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
	return this->consulta_texto("SELECT valor FROM configuracion WHERE atributo = '"+atributo+"' LIMIT 1");
}

bool Base_de_Datos::existe_dispositivo(const Dispositivo_Midi &dispositivo)
{
	std::string respuesta = this->consulta_texto("SELECT nombre FROM dispositivos "
												"WHERE cliente = '"+ std::to_string(static_cast<unsigned int>(dispositivo.cliente()))+"' AND "
													"puerto = '"+ std::to_string(static_cast<unsigned int>(dispositivo.puerto()))+"' AND "
													"nombre = '"+dispositivo.nombre()+"'");
	if(respuesta.length() > 0)
		return true;
	return false;
}

void Base_de_Datos::agregar_dispositivo(const Dispositivo_Midi &dispositivo)
{
	//Inserta dispositivo nuevo
	this->consulta("INSERT INTO dispositivos (cliente, puerto, nombre, capacidad, capacidad_activa, "
												"habilitado, sensitivo, volumen_entrada, rango_teclado, "
												"volumen_salida, teclado_luminoso) "
							"VALUES ('"+std::to_string(static_cast<unsigned int>(dispositivo.cliente()))+"', "
									"'"+std::to_string(static_cast<unsigned int>(dispositivo.puerto()))+"', "
									"'"+dispositivo.nombre()+"', "
									"'"+std::to_string(static_cast<unsigned int>(dispositivo.capacidad()))+"', "
									"'"+std::to_string(static_cast<unsigned int>(dispositivo.capacidad_activa()))+"', "
									"'"+std::to_string(static_cast<unsigned int>(dispositivo.habilitado()))+"', "
									"'"+std::to_string(static_cast<unsigned int>(dispositivo.sensitivo()))+"', "
									"'"+std::to_string(dispositivo.volumen_entrada())+"', "
									"'"+dispositivo.rango_teclado().texto()+"', "
									"'"+std::to_string(dispositivo.volumen_salida())+"', "
									"'"+std::to_string(static_cast<unsigned int>(dispositivo.id_teclas_luminosas()))+"')");
}

void Base_de_Datos::eliminar_dispositivo(const Dispositivo_Midi &dispositivo)
{
	this->consulta("DELETE FROM dispositivos "
							"WHERE cliente = '"+ std::to_string(static_cast<unsigned int>(dispositivo.cliente()))+"' AND "
									"puerto = '"+ std::to_string(static_cast<unsigned int>(dispositivo.puerto()))+"' AND "
									"nombre = '"+dispositivo.nombre()+"'");
}

void Base_de_Datos::actualizar_cliente_dispositivo(unsigned char cliente_antiguo, const Dispositivo_Midi &dispositivo)
{
	//El dispositivo es encontrado en otro id cliente y hay que actualizarlo
	this->consulta("UPDATE dispositivos SET cliente = '"+ std::to_string(static_cast<unsigned int>(cliente_antiguo))+"' "
									"WHERE cliente = '"+ std::to_string(static_cast<unsigned int>(dispositivo.cliente()))+"' AND "
											"puerto = '"+ std::to_string(static_cast<unsigned int>(dispositivo.puerto()))+"' AND "
											"nombre = '"+dispositivo.nombre()+"'");
}

std::vector<Dispositivo_Midi> Base_de_Datos::lista_dispositivos()
{
	std::vector<std::vector<std::string>> datos = this->consulta_tabla("SELECT 	cliente, puerto, nombre, capacidad, capacidad_activa, "
																				"habilitado, sensitivo, volumen_entrada, rango_teclado, "
																				"volumen_salida, teclado_luminoso "
																		"FROM dispositivos", 11);
	std::vector<Dispositivo_Midi> dispositivos;

	for(unsigned long int x=0; x<datos.size(); x++)
	{
		Dispositivo_Midi nuevo;

		if(datos[x][0].size() <= 3)
			nuevo.cliente(static_cast<unsigned char>(std::stoi(datos[x][0])));
		if(datos[x][1].size() <= 3)
			nuevo.puerto(static_cast<unsigned char>(std::stoi(datos[x][1])));
		nuevo.nombre(datos[x][2]);
		if(datos[x][3].size() <= 1)
			nuevo.capacidad(static_cast<unsigned char>(std::stoi(datos[x][3])));
		if(datos[x][4].size() <= 1)
			nuevo.capacidad_activa(static_cast<unsigned char>(std::stoi(datos[x][4])));
		if(datos[x][5].size() <= 1)
			nuevo.habilitado(static_cast<bool>(std::stoi(datos[x][5])));
		if(datos[x][6].size() <= 1)
			nuevo.sensitivo(static_cast<bool>(std::stoi(datos[x][6])));
		nuevo.volumen_entrada(static_cast<double>(std::stod(datos[x][7])));
		nuevo.rango_teclado(datos[x][8]);
		nuevo.volumen_salida(static_cast<double>(std::stod(datos[x][9])));
		if(datos[x][10].size() <= 3)
			nuevo.teclas_luminosas(static_cast<unsigned int>(std::stoi(datos[x][10])));

		dispositivos.push_back(nuevo);
	}
	return dispositivos;
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
