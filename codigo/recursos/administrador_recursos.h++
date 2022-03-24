#ifndef ADMINISTRADOR_DE_RECURSOS_H
#define ADMINISTRADOR_DE_RECURSOS_H

#include <map>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sombreador.h++"
#include "textura_2d.h++"
#include "tipografia.h++"
#include "color.h++"
#include "rectangulo.h++"

#include "../archivos/archivo.h++"
#include "../archivos/imagen_tga.h++"
#include "../dispositivos/pantalla.h++"

#include "../registro.h++"
#include "configuracion_cmake.h++"

enum Textura
{
	T_FondoTitulo,
	T_Titulo,
	T_Boton,
	T_TeclaBlanca,
	T_TeclaNegra,
	T_TeclaBlancaPresionada,
	T_TeclaBlancaPresionadaDoble,
	T_TeclaNegraPresionada,
	T_Circulo,
	T_BordeOrganoRojo,
	T_BordeOrganoNegro,
	T_Nota,
	T_NotaResaltada,
	T_Sombra,
	T_FrenteBarraProgreso,
	T_ParticulaNota,
	T_Barra,
	T_ConfiguracionPista,
	T_SonidoActivado,
	T_SonidoDesactivado,
	T_Reproducir,
	T_Pausar,
	T_TituloMusica,
	T_Color_1,
	T_Color_2,
	T_Color_3,
	T_Color_4,
	T_Color_5,
	T_Color_6,
	T_Color_7,
	T_Color_8,
	T_Color_9,
	T_Color_Invisible,
	T_Tocar,
	T_Aprender,
	T_MusicaFondo,
	T_CasillaInactiva,
	T_CasillaActiva,
	T_CasillaSombra,
	T_FlechaIzquierda,
	T_FlechaDerecha,
	T_FlechaAbajo,
	T_FlechaArriba,
	T_ControlDeslizante_Fondo,
	T_ControlDeslizante_Relleno,
	T_ControlDeslizante_Boton,
};

enum SombreadorVF
{
	S_Rectangulo,
	S_Texto,
	S_Particula,
};

enum FiguraGeometrica
{
	F_Rectangulo,
};

enum ModeloLetra
{
	LetraTituloGrande = 35,
	LetraTitulo = 20,
	LetraGrande = 16,
	LetraMediana = 14,
	LetraChica = 12,
	LetraMuyChica = 8,
};

class Administrador_Recursos
{
private:
	std::map<Textura, Textura2D*> m_lista_texturas;
	std::map<SombreadorVF, Sombreador*> m_lista_sombreadores;
	std::map<FiguraGeometrica, Rectangulo*> m_lista_figuras;

	std::map<Textura, std::string> m_archivo_texturas;
	std::map<SombreadorVF, std::string> m_archivo_sombreador_vertice;
	std::map<SombreadorVF, std::string> m_archivo_sombreador_fragmento;

	std::map<ModeloLetra, Tipografia*> m_formato_letras;

	std::stack<std::array<int, 4>> m_recortes;
	glm::mat4 m_matriz_proyeccion;
public:
	Administrador_Recursos();
	~Administrador_Recursos();

	Textura2D *textura(Textura valor);
	Sombreador *sombreador(SombreadorVF valor);
	Rectangulo *figura(FiguraGeometrica valor);
	Tipografia *tipografia(ModeloLetra tipo);

	void recortar_pantalla(float x, float y, float ancho, float alto);
	void revertir_recorte();

	void actualizar_pantalla(float ancho, float alto);
};

#endif
