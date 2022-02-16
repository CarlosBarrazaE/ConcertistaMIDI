#include "notificacion.h++"

std::vector<Mensaje *> Notificacion::notificaciones;

Notificacion::Notificacion(Administrador_Recursos *recursos) : Elemento(0, 0, 400, 40, true)
{
	m_recursos = recursos;
	m_textura_fondo = recursos->textura(T_Nota);
	m_rectangulo = recursos->figura(F_Rectangulo);
}

Notificacion::~Notificacion()
{
	for(Mensaje *m : Notificacion::notificaciones)
		delete m;
}

void Notificacion::actualizar(unsigned int diferencia_tiempo)
{
	unsigned int contador = 0;
	float mover = 0;
	Mensaje *actual = NULL;
	for(unsigned int x=0; x<Notificacion::notificaciones.size(); x++)
	{
		actual = Notificacion::notificaciones.at(x);
		if(actual->etiqueta == NULL)
		{
			actual->etiqueta = new Etiqueta(0, 0, 400, 40, true, Notificacion::notificaciones[x]->texto, LetraChica, m_recursos);
			actual->etiqueta->centrado_vertical(true);
			actual->etiqueta->color(Color(1.0f, 1.0f, 1.0f));
		}
		if(actual->tiempo > 0)
		{
			if(contador < MAXIMAS_NOTIFICACIONES)
			{
				actual->tiempo -= (static_cast<float>(diferencia_tiempo) / 1000000000.0f);
				//0.25 Segundos para aparecer

				if(actual->tiempo > 1 && actual->opacidad < 1)
					actual->opacidad += (static_cast<float>(diferencia_tiempo) / 1000000000.0f)*4;//0.25 Segundos para aparecer
				else if(actual->tiempo <= 0.25f && actual->opacidad > 0)
					actual->opacidad -= (static_cast<float>(diferencia_tiempo) / 1000000000.0f)*4;//0.25 Segundos para desaparecer
			}

			if(mover > 0)
				actual->mover += mover;

			//Animacion moviendo hacia arriba
			if(actual->mover > 0)
			{
				//Se mueve 45 pixeles en 0.25 segundos
				actual->posicion_y -= (static_cast<float>(diferencia_tiempo) / 1000000000.0f)*180;
				actual->mover -= (static_cast<float>(diferencia_tiempo) / 1000000000.0f)*180;
				if(actual->mover < 0)
				{
					actual->posicion_y += actual->mover*-1;
					actual->mover = 0;
				}
			}

			if(actual->tiempo <= 0)
			{
				//Eliminar esta notificacion
				delete actual;
				actual = NULL;
				Notificacion::notificaciones.erase(Notificacion::notificaciones.begin()+x);
				x--;
				mover += 45;
			}
			else
				contador++;
		}
	}
}

void Notificacion::dibujar()
{
	//Si no hay notificaciones se omite
	if(Notificacion::notificaciones.size() == 0)
		return;

	m_rectangulo->textura(true);
	m_rectangulo->extremos_fijos(true, true);
	for(unsigned int x=0; x<Notificacion::notificaciones.size() && x < MAXIMAS_NOTIFICACIONES; x++)
	{
		Mensaje *actual = Notificacion::notificaciones[x];
		if(actual->etiqueta == NULL)
			continue;

		//Selecciona el color de fonde deacuero al tipo de mensaje
		if(actual->estado == EstadoError)
			m_rectangulo->color(Color(0.7f, 0.0f, 0.0f, actual->opacidad));
		else if(actual->estado == EstadoAviso)
			m_rectangulo->color(Color(0.9f, 0.5f, 0.0f, actual->opacidad));
		else if(actual->estado == EstadoNota)
			m_rectangulo->color(Color(0.0f, 0.598f, 0.0f, actual->opacidad));
		else if(actual->estado == EstadoDepurar)
			m_rectangulo->color(Color(0.145f, 0.707f, 1.0f, actual->opacidad));

		//Dibuja el fondo
		m_textura_fondo->activar();
		m_rectangulo->dibujar_estirable(this->x(), this->y() + actual->posicion_y, this->ancho(), this->alto(), 15.0f, 12.0f);

		actual->etiqueta->color(Color(1.0f, 1.0f, 1.0f, actual->opacidad));
		actual->etiqueta->posicion(this->x(), this->y() + actual->posicion_y);
		actual->etiqueta->dibujar();
	}
	m_rectangulo->extremos_fijos(false, false);
}

void Notificacion::evento_raton(Raton */*raton*/)
{

}

bool Notificacion::mostrando_notificaciones()
{
	if(Notificacion::notificaciones.size() > 0)
		return true;
	return false;
}

void Notificacion::Registrar(std::string texto, int tiempo, CodigoEstado estado)
{
	if(tiempo < 3)
		tiempo = 3;

	Notificacion::notificaciones.push_back(new Mensaje(NULL, texto, static_cast<float>(tiempo), estado));

	//Se actualiza la posicion del ultimo elemento agregado
	if(Notificacion::notificaciones.size() > 1)
	{
		unsigned long int posicion = Notificacion::notificaciones.size()-1;

		Notificacion::notificaciones[posicion]->posicion_y = Notificacion::notificaciones[posicion-1]->posicion_y + 45;
		Notificacion::notificaciones[posicion]->mover = Notificacion::notificaciones[posicion-1]->mover;
	}
}

void Notificacion::Error(std::string texto, int tiempo)
{
	Notificacion::Registrar(texto, tiempo, EstadoError);
	Registro::Error(texto);
}

void Notificacion::Aviso(std::string texto, int tiempo)
{
	Notificacion::Registrar(texto, tiempo, EstadoAviso);
	Registro::Aviso(texto);
}

void Notificacion::Nota(std::string texto, int tiempo)
{
	Notificacion::Registrar(texto, tiempo, EstadoNota);
	Registro::Nota(texto);
}

void Notificacion::Depurar(std::string texto, int tiempo)
{
	Notificacion::Registrar(texto, tiempo, EstadoDepurar);
	Registro::Depurar(texto);
}
