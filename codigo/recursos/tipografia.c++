#include "tipografia.h++"

Tipografia::Tipografia(Formato formato, int tamanno_letra)
{
	if(FT_Init_FreeType(&m_libreria))
		Registro::Error("La libreria Freetype fallo al iniciar.");

	std::string ruta_tipografia = RUTA_ARCHIVOS;
	if(formato == Normal)
		ruta_tipografia += "/tipografias/NotoSans-Regular.ttf";
	else if(formato == Negrita)
		ruta_tipografia += "/tipografias/NotoSans-Black.ttf";
	else if(formato == Cursiva)
		ruta_tipografia += "/tipografias/NotoSans-Italic.ttf";
	else if(formato == CursivaNegrita)
		ruta_tipografia += "/tipografias/NotoSans-BlackItalic.ttf";

	if(FT_New_Face(m_libreria, ruta_tipografia.c_str(), 0, &m_tipografia))
		Registro::Error("Freetype fallo al cargar la tipografia del archivo: " + ruta_tipografia);

	if(tamanno_letra > 50)
		tamanno_letra = 50;
	else if(tamanno_letra < 5)
		tamanno_letra = 5;
	m_tamanno_letra = tamanno_letra;

	FT_Set_Char_Size(m_tipografia, 0, m_tamanno_letra*64, 92, 92);//91.79
	generar_caracteres();
}

Tipografia::~Tipografia()
{
	for(std::map<unsigned int, Caracter*>::iterator i = m_caracteres.begin(); i != m_caracteres.end(); i++)
		delete i->second;
	m_caracteres.clear();

	//Destruye freetype
	FT_Done_Face(m_tipografia);
	FT_Done_FreeType(m_libreria);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &m_indice_atlas);
}

void Tipografia::generar_caracteres()
{
	icu::UnicodeString idioma_espannol = "0123456789 ⁰¹²³⁴⁵⁶⁷⁸⁹ABCDEFGHIJKLMNÑOPQRSTUVWXYZabcdefghijklmnñopqrstuvwxyzÁÉÍÓÚáéíóúÄËÏÖÜäëïöüâêîôû,._;:'·#$€%&@\"\\()[]{}+-*/=¿?¡!<>ºªḉḈçÇ^�";

	int contador = 0;
	m_ancho_atlas = 0;
	m_alto_atlas = 0;
	bool error = false;
	FT_UInt letra_actual;
	for(int n=0; n<idioma_espannol.length(); n++)
	{
		letra_actual = FT_Get_Char_Index(m_tipografia, idioma_espannol[n]);
		if(letra_actual == 0)
		{
			Registro::Aviso("Caracter unicode desconocido \"" + std::to_string(static_cast<unsigned int>(idioma_espannol[n])) + "\" se requiere otra tipografia.");
			continue;
		}

		error = FT_Load_Glyph(m_tipografia, letra_actual, FT_LOAD_RENDER);
		if(error)
		{
			Registro::Error("Error al cargar el caracter: " + std::to_string(static_cast<unsigned int>(idioma_espannol[n])));
			continue;
		}

		FT_GlyphSlot letra = m_tipografia->glyph;

		m_ancho_atlas += static_cast<int>(letra->bitmap.width);
		if(static_cast<int>(letra->bitmap.rows) > m_alto_atlas)
			m_alto_atlas = static_cast<int>(letra->bitmap.rows);
		contador++;
	}


	m_indice_atlas = 0;
	glGenTextures(1, &m_indice_atlas);
	glBindTexture(GL_TEXTURE_2D, m_indice_atlas);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	Textura2D::Ultimo_indice_seleccionado = m_indice_atlas;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_ancho_atlas, m_alto_atlas, 0, GL_RED,GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	letra_actual = 0;
	int posicion_textura = 0;
	for(int n=0; n<idioma_espannol.length(); n++)
	{
		letra_actual = FT_Get_Char_Index(m_tipografia, idioma_espannol[n]);
		if(letra_actual == 0)
		{
			Registro::Aviso("Caracter unicode desconocido \"" + std::to_string(static_cast<unsigned int>(idioma_espannol[n])) + "\" se requiere otra tipografia.");
			continue;
		}

		error = FT_Load_Glyph(m_tipografia, letra_actual, FT_LOAD_RENDER);
		if(error)
		{
			Registro::Error("Error al cargar el caracter: " + std::to_string(static_cast<unsigned int>(idioma_espannol[n])));
			continue;
		}

		FT_GlyphSlot letra = m_tipografia->glyph;

		if(!m_caracteres[idioma_espannol[n]])
		{
			Caracter *letra_nueva = new Caracter();
			letra_nueva->codigo_unicode = letra_actual;
			letra_nueva->ancho = static_cast<int>(letra->bitmap.width);
			letra_nueva->alto = static_cast<int>(letra->bitmap.rows);
			letra_nueva->ajuste_izquierda = letra->bitmap_left;
			letra_nueva->ajuste_arriba = letra->bitmap_top;
			letra_nueva->avance_x = letra->advance.x;
			letra_nueva->avance_y = letra->advance.y;
			letra_nueva->textura_x = posicion_textura;
			m_caracteres[idioma_espannol[n]] = letra_nueva;

			glTexSubImage2D(GL_TEXTURE_2D, 0, posicion_textura, 0, letra_nueva->ancho, letra_nueva->alto, GL_RED, GL_UNSIGNED_BYTE, letra->bitmap.buffer);
			posicion_textura += static_cast<int>(letra->bitmap.width);
		}
		else
			Registro::Aviso("El caracter codigo: " + std::to_string(static_cast<unsigned int>(idioma_espannol[n])) + " ya existe");
	}
}

Caracter *Tipografia::obtener_caracter(unsigned int caracter)
{
	return m_caracteres[caracter];
}

int Tipografia::crear_texto(icu::UnicodeString texto, unsigned int *indice_objeto)
{
	long int x=0, y=0;

	long int x_inicial = x;

	if(Textura2D::Ultimo_indice_seleccionado != m_indice_atlas)
	{
		glBindTexture(GL_TEXTURE_2D, m_indice_atlas);
		Textura2D::Ultimo_indice_seleccionado = m_indice_atlas;
	}

	//Contar el numero de espacios a omitir
	int numero_espacio = 0;
	for(int l=0; l<texto.length(); l++)
	{
		if(texto[l] == ' ')
			numero_espacio++;
	}

	Caracter *letra;
	unsigned int letra_anterior = 0;
	float vertices[static_cast<unsigned long int>(6*(texto.length()-numero_espacio))][4];
	int posicion_arreglo = 0;

	for(int n=0; n<texto.length(); n++)
	{
		letra = obtener_caracter(static_cast<unsigned int>(texto[n]));

		if(!letra)
			continue;

		//Se calcula el Interletraje
		FT_Bool interletraje = FT_HAS_KERNING(m_tipografia);
		if(interletraje && letra_anterior && letra->codigo_unicode)
		{
			FT_Vector delta;
			FT_Get_Kerning(m_tipografia, letra_anterior, letra->codigo_unicode, FT_KERNING_DEFAULT, &delta);
			x += delta.x >> 6;
		}

		if(texto[n] != ' ')//Se omiten los espacios
		{
			float xpos = static_cast<float>(x + letra->ajuste_izquierda);
			float ypos = static_cast<float>(y - letra->ajuste_arriba);

			float an = static_cast<float>(letra->ancho);
			float al = static_cast<float>(letra->alto);
			vertices[posicion_arreglo][0] = xpos + an;
			vertices[posicion_arreglo][1] = ypos;
			vertices[posicion_arreglo][2] = (static_cast<float>(letra->textura_x) + an) / static_cast<float>(m_ancho_atlas);//mi valor / total
			vertices[posicion_arreglo][3] = 0.0f;
			posicion_arreglo++;

			vertices[posicion_arreglo][0] = xpos;
			vertices[posicion_arreglo][1] = ypos;
			vertices[posicion_arreglo][2] = static_cast<float>(letra->textura_x) / static_cast<float>(m_ancho_atlas);
			vertices[posicion_arreglo][3] = 0.0f;
			posicion_arreglo++;

			vertices[posicion_arreglo][0] = xpos + an;
			vertices[posicion_arreglo][1] = ypos + al;
			vertices[posicion_arreglo][2] = (static_cast<float>(letra->textura_x) + an) / static_cast<float>(m_ancho_atlas);
			vertices[posicion_arreglo][3] = al / static_cast<float>(m_alto_atlas);
			posicion_arreglo++;

			vertices[posicion_arreglo][0] = xpos;
			vertices[posicion_arreglo][1] = ypos + al;
			vertices[posicion_arreglo][2] = static_cast<float>(letra->textura_x) / static_cast<float>(m_ancho_atlas);
			vertices[posicion_arreglo][3] = al / static_cast<float>(m_alto_atlas);
			posicion_arreglo++;

			vertices[posicion_arreglo][0] = xpos + an;
			vertices[posicion_arreglo][1] = ypos + al;
			vertices[posicion_arreglo][2] = (static_cast<float>(letra->textura_x) + an) / static_cast<float>(m_ancho_atlas);
			vertices[posicion_arreglo][3] = al / static_cast<float>(m_alto_atlas);
			posicion_arreglo++;

			vertices[posicion_arreglo][0] = xpos;
			vertices[posicion_arreglo][1] = ypos;
			vertices[posicion_arreglo][2] = static_cast<float>(letra->textura_x) / static_cast<float>(m_ancho_atlas);
			vertices[posicion_arreglo][3] = 0.0f;
			posicion_arreglo++;
		}

		x += letra->avance_x >> 6;
		letra_anterior = letra->codigo_unicode;
	}

	glGenBuffers(1, indice_objeto);
	glBindBuffer(GL_ARRAY_BUFFER, *indice_objeto);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(vertices)), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	return static_cast<int>(x - x_inicial);
}

int Tipografia::alto_texto()
{
	return m_tamanno_letra;
}

void Tipografia::activar()
{
	if(Textura2D::Ultimo_indice_seleccionado != m_indice_atlas)
	{
		glBindTexture(GL_TEXTURE_2D, m_indice_atlas);
		Textura2D::Ultimo_indice_seleccionado = m_indice_atlas;
	}
}
