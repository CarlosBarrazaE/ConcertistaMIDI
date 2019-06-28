#include "tablero_notas.h++"

Tablero_Notas::Tablero_Notas(int x, int y, int ancho, int alto, Administrador_Recursos *recursos) : Elemento()
{
	this->x = x;
	this->y = y;
	this->ancho = ancho;
	this->alto = alto;
	this->velocidad_caida = 6500;

	this->sombreador_solido = recursos->obtener_sombreador(Rectangulo_SinTextura);
	this->sombreador = recursos->obtener_sombreador(Rectangulo_Textura);
	this->textura_nota_blanca = recursos->obtener_textura(T_NotaBlanca);
	this->textura_nota_negra = recursos->obtener_textura(T_NotaNegra);
	this->textura_sombra_nota = recursos->obtener_textura(T_SombraNota);

	this->fondo = new Rectangulo(sombreador_solido, Color(0.5, 0.5, 0.5));
	this->estructura_nota = new Rectangulo(sombreador, textura_nota_blanca, Color(0.2, 0.7, 0.3));
	this->texto = recursos->obtener_tipografia(LetraMuyChica);
}

Tablero_Notas::~Tablero_Notas()
{
	delete fondo;
	delete estructura_nota;
}

void Tablero_Notas::e_tiempo(int tiempo)
{
	this->tiempo_actual_midi = tiempo;
}

void Tablero_Notas::e_notas(TranslatedNoteSet notas)
{
	this->notas = notas;
}

void Tablero_Notas::e_lineas(MidiEventMicrosecondList lineas)
{
	this->lineas = lineas;
}

void Tablero_Notas::e_pistas(std::map<int, Pista*> *pistas)
{
	this->pistas = pistas;
}

void Tablero_Notas::e_dimension(int ancho, int alto)
{
	this->ancho = ancho;
	this->alto = alto;
}

void Tablero_Notas::e_ancho_blanca(int valor)
{
	this->ancho_blanca = valor;
}

void Tablero_Notas::e_ancho_negra(int valor)
{
	this->ancho_negra = valor;
}

void Tablero_Notas::e_ajuste_x(int valor)
{
	this->ajuste_x = valor;
}

void Tablero_Notas::c_velocidad_caida(int valor)
{
	if(valor < 0)
		this->velocidad_caida = this->velocidad_caida - 100;
	else
		this->velocidad_caida = this->velocidad_caida + 100;

	if(this->velocidad_caida < 1500)
		this->velocidad_caida = 1500;
	else if(this->velocidad_caida > 14000)
		this->velocidad_caida = 14000;
}

void Tablero_Notas::actualizar(Raton *raton)
{
}

void Tablero_Notas::dibujar()
{
	this->fondo->seleccionar_color(Color(0.95, 0.95, 0.95));
	this->fondo->dibujar(0, 0, this->ancho, this->alto);
	this->fondo->seleccionar_color(Color(0.7, 0.7, 0.7));
	this->dibujar_lineas_horizontales();
	this->dibujar_lineas_verticales();
	this->dibujar_notas(this->textura_sombra_nota, NULL);//Dibuja la sombra de la nota
	this->dibujar_notas(this->textura_nota_blanca, this->textura_nota_negra);//Dibuja la nota
}

void Tablero_Notas::dibujar_lineas_horizontales()
{
	int numero_linea = 0;
	int posicion_y = 0;

	for(int x=0; x<lineas.size(); x++)
	{
		numero_linea++;
		posicion_y = ((this->tiempo_actual_midi - lineas[x]) / this->velocidad_caida) + this->alto;
		if(posicion_y < 0)
			break;
		else if(posicion_y > this->alto)
			continue;

		this->fondo->dibujar(0, posicion_y, this->ancho, 1);
		this->texto->dibujar_texto(5, posicion_y, std::to_string(numero_linea));
	}
}

void Tablero_Notas::dibujar_lineas_verticales()
{
	int posicion_x = this->ajuste_x + this->ancho_blanca * 2;
	bool en_do = true;
	for(int x=0; x<14; x++)
	{
		this->fondo->dibujar(posicion_x, 0, 1, this->alto);
		if(en_do)
			posicion_x += this->ancho_blanca * 3;
		else
			posicion_x += this->ancho_blanca * 4;
		en_do = !en_do;
	}
}

void Tablero_Notas::dibujar_notas(Textura2D *textura_nota_blanca, Textura2D *textura_nota_negra)
{
	if(textura_nota_negra == NULL)
		textura_nota_blanca->activar();
	int ajuste_negra = 0;
	int ancho_tecla = 0;
	int posicion = 0;
	int largo_nota = 0;
	int largo_final = 0;
	int posicion_y = 0;

	for(TranslatedNoteSet::const_iterator nota = notas.begin(); nota != notas.end(); nota++)
	{
		posicion_y = (this->tiempo_actual_midi - nota->start) / this->velocidad_caida;

		if(posicion_y < -this->alto)
			break;//No se dibujan las notas que aun no entran en la pantalla
		largo_nota = (nota->end - nota->start) / this->velocidad_caida;
		if(posicion_y-largo_nota > 0)
			continue;//No se dibujan las notas que ya salieron de la pantalla

		posicion = Octava::prosicion_nota(nota->note_id);

		this->estructura_nota->seleccionar_color(pistas->at(nota->track_id)->o_color());
		if(Octava::es_negra(nota->note_id))
		{
			ancho_tecla = this->ancho_negra;
			int negra = Octava::numero_negra(nota->note_id);
			if(negra==1 || negra == 3)
				ajuste_negra = this->ancho_blanca - (this->ancho_negra * 0.667);
			else if(negra==2 || negra == 5)
				ajuste_negra = this->ancho_blanca - (this->ancho_negra * 0.333);
			else if(negra==4)
				ajuste_negra = this->ancho_blanca - (this->ancho_negra * 0.5);

			//textura_nota_negra es NULL cuando solo se dibujan sombras
			if(textura_nota_negra != NULL)
				textura_nota_negra->activar();
		}
		else
		{
			ancho_tecla = this->ancho_blanca;
			ajuste_negra = 0;
			//Cuando se dibujan las sombras no es necesario volver a activar la textura
			if(textura_nota_negra != NULL)
				textura_nota_blanca->activar();
		}

		//Recorta la parte de la nota que no se ve
		if(posicion_y > 20)
			largo_final = largo_nota - (posicion_y-20);
		else
			largo_final = largo_nota;

		this->estructura_nota->dibujar(this->ajuste_x + posicion * this->ancho_blanca + ajuste_negra, this->alto + posicion_y-largo_nota, ancho_tecla, largo_final);
	}
}
