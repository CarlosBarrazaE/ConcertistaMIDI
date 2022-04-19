#include "etiqueta.h++"

Color Etiqueta::Ultimo_color;

Etiqueta::Etiqueta(Administrador_Recursos *recursos) : Elemento(0, 0, 0, 0), Figura(recursos->sombreador(S_Texto))
{
	m_rectangulo = recursos->figura(F_Rectangulo);
}

Etiqueta::Etiqueta(float x, float y, bool centrado, const std::string &texto, ModeloLetra tipografia, Administrador_Recursos *recursos) : Elemento(x, y, 0, 0), Figura(recursos->sombreador(S_Texto))
{
	m_centrado_horizontal = centrado;
	m_texto_actual = texto;
	m_texto = texto.c_str();
	m_tipografia = recursos->tipografia(tipografia);
	m_rectangulo = recursos->figura(F_Rectangulo);
	this->actualizar_texto();
}

Etiqueta::Etiqueta(float x, float y, bool centrado, const std::string &texto, Tipografia *tipografia, Administrador_Recursos *recursos) : Elemento(x, y, 0, 0), Figura(recursos->sombreador(S_Texto))
{
	m_centrado_horizontal = centrado;
	m_texto_actual = texto;
	m_texto = texto.c_str();
	m_tipografia = tipografia;
	m_rectangulo = recursos->figura(F_Rectangulo);
	this->actualizar_texto();
}

Etiqueta::Etiqueta(float x, float y, float ancho, float alto, bool centrado, const std::string &texto, ModeloLetra tipografia, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto), Figura(recursos->sombreador(S_Texto))
{
	m_centrado_horizontal = centrado;
	m_texto_actual = texto;
	m_texto = texto.c_str();
	m_tipografia = recursos->tipografia(tipografia);
	m_rectangulo = recursos->figura(F_Rectangulo);
	this->actualizar_texto();
}

Etiqueta::~Etiqueta()
{
	this->limpiar();
}

void Etiqueta::actualizar_texto()
{
	if(m_tipografia != NULL && m_texto.length() > 0)
	{
		this->limpiar();
		this->seleccionar_figura();
		m_ancho_texto = static_cast<float>(m_tipografia->crear_texto(m_texto, &m_indice_objeto));
		m_alto_texto = static_cast<float>(m_tipografia->alto_texto());
	}
	else
	{
		m_ancho_texto = 0;
		m_alto_texto = 0;
	}
}

void Etiqueta::limpiar()
{

	if(m_indice_objeto > 0)
	{
		this->seleccionar_figura();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_indice_objeto);
		m_indice_objeto = 0;
	}
}

void Etiqueta::actualizar(unsigned int /*diferencia_tiempo*/)
{
}

void Etiqueta::dibujar()
{
	if(m_texto.length() == 0)
		return;

	if(m_tipografia == NULL)
		m_mostrar_fondo = true;

	if(m_mostrar_fondo)
	{
		m_rectangulo->textura(false);
		m_rectangulo->dibujar(this->x(), this->y(), this->ancho(), this->alto(), Color(0.2f, 0.7f, 0.5f, 0.5f));
	}

	if(m_tipografia == NULL)
	{
		Registro::Error("Etiqueta sin tipografia en texto: " + m_texto_actual);
		return;
	}

	int nueva_x = 0;
	int nueva_y = 0;
	if(m_centrado_horizontal)
		nueva_x = static_cast<int>(this->x()+this->ancho()/2.0f - m_ancho_texto / 2.0f);
	else
		nueva_x = static_cast<int>(this->x()+m_margen);

	if(m_centrado_vertical)
		nueva_y = static_cast<int>(this->y()+m_alto_texto+this->alto()/2.0f-m_alto_texto/2.0f);
	else
		nueva_y = static_cast<int>(this->y()+m_alto_texto+m_margen);

	//TODO ¿Esto deberia estar a cargo de la tipografia?
	glm::mat4 modelo = glm::mat4(1.0f);
	modelo = glm::translate(modelo, glm::vec3(nueva_x, nueva_y, 0.0f));
	//modelo = glm::scale(modelo, glm::vec3(ancho, alto, 1.0f));

	m_sombreador->activar();
	m_sombreador->uniforme_matriz4("modelo", modelo);
	if(Etiqueta::Ultimo_color != m_color)
	{
		m_sombreador->uniforme_vector4f("color_texto", m_color.rojo(), m_color.verde(), m_color.azul(), m_color.alfa());
		Etiqueta::Ultimo_color = m_color;
	}

	this->seleccionar_figura();
	m_tipografia->activar();
	glDrawArrays(GL_TRIANGLES, 0, m_texto.length()*6);
}

void Etiqueta::evento_raton(Raton */*raton*/)
{
}

void Etiqueta::centrado(bool centrado)
{
	m_centrado_horizontal = centrado;
	m_centrado_vertical = centrado;
}

void Etiqueta::centrado_horizontal(bool centrado_h)
{
	m_centrado_horizontal = centrado_h;
}

void Etiqueta::centrado_vertical(bool centrado_v)
{
	m_centrado_vertical = centrado_v;
}

void Etiqueta::margen(float margen)
{
	m_margen = margen;
}

void Etiqueta::texto(const std::string &texto)
{
	//No se vuelve a crear el mismo texto dos veces
	if(m_texto_actual != texto)
		m_texto_actual = texto;
	else
		return;
	m_texto = texto.c_str();
	this->actualizar_texto();
}

void Etiqueta::color(const Color &color)
{
	m_color = color;
}

void Etiqueta::tipografia(Tipografia *tipografia)
{
	m_tipografia = tipografia;
	m_alto_texto = static_cast<float>(m_tipografia->alto_texto());
	this->actualizar_texto();
}

float Etiqueta::largo_texto()
{
	return m_ancho_texto;
}

float Etiqueta::alto_texto()
{
	return m_alto_texto;
}

std::string& Etiqueta::texto()
{
	return m_texto_actual;
}

void Etiqueta::mostrar_fondo(bool estado)
{
	m_mostrar_fondo = estado;
}
