#include "boton.h++"

Boton::Boton(int x, int y, int ancho, int alto, std::string texto, Textura2D *textura, Color color, bool centrado, Administrador_Recursos *recursos) : Elemento(x, y, ancho, alto, centrado)
{
	this->texto_boton = texto;
	this->textura_boton = textura;
	this->centrado = centrado;
	this->sobre_boton = false;
	this->boton_pre_activado = false;
	this->boton_activado = false;

	color_boton_normal = color;
	color_boton_sobre.e_color(color.o_rojo()-0.1f, color.o_verde()-0.1f, color.o_azul()-0.1f);
	color_texto.e_color(0.2f, 0.2f, 0.2f);

	color_boton = color_boton_normal;

	this->texto = recursos->obtener_tipografia(LetraMediana);
	rectangulo = recursos->obtener_figura(F_Rectangulo);
	this->ajuste_texto = this->texto->ancho_texto(texto) / 2;
}

Boton::~Boton()
{
}

void Boton::e_color_texto(Color color)
{
	this->color_texto = color;
}

void Boton::e_letra(Texto *texto)
{
	this->texto = texto;
	this->ajuste_texto = this->texto->ancho_texto(this->texto_boton) / 2;
}

void Boton::e_textura(Textura2D *textura)
{
	this->textura_boton = textura;
}

void Boton::actualizar(unsigned int diferencia_tiempo)
{
}

void Boton::dibujar()
{
	this->textura_boton->activar();
	rectangulo->textura(true);
	rectangulo->dibujar(this->x+this->dx, this->y+this->dy, this->ancho, this->alto, color_boton);
	this->texto->imprimir(this->x+this->dx+this->ancho/2 - this->ajuste_texto, this->y+this->dy+this->alto/2 + this->texto->alto_texto()/2, this->texto_boton, color_texto);
}

void Boton::evento_raton(Raton *raton)
{
	if(raton->x() >= this->x+this->dx && raton->x() <= this->x+this->dx + this->ancho &&
		raton->y() >= this->y+this->dy && raton->y() <= this->y+this->dy + this->alto)
	{
		if(raton->activado(BotonIzquierdo) && this->sobre_boton)
			this->boton_pre_activado = true;
		else if(!raton->activado(BotonIzquierdo))
		{
			color_boton = color_boton_sobre;
			this->sobre_boton = true;
			if(this->boton_pre_activado)
			{
				this->boton_activado = true;
				this->boton_pre_activado = false;
			}
		}
	}
	else
	{
		color_boton = color_boton_normal;
		this->sobre_boton = false;
		this->boton_pre_activado = false;
		this->boton_activado = false;
	}
}

bool Boton::esta_activado()
{
	bool estado = this->boton_activado;
	this->boton_activado = false;
	return estado;
}