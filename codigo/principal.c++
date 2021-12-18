#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "recursos/administrador_recursos.h++"
#include "dispositivos/pantalla.h++"
#include "controlador_juego.h++"

#include "registro.h++"

#define ANCHO 800
#define ALTO 600

void configurar_gl();
void ajustar_ventana(Controlador_Juego *controlador, int ancho_nuevo, int alto_nuevo);
void eventos_raton_rueda(Controlador_Juego *controlador, int desplazamiento_x, int desplazamiento_y);
void eventos_raton_botones(Controlador_Juego *controlador, int boton, int accion, int numero_clics);
void eventos_raton_posicion(Controlador_Juego *controlador, int x, int y);
void eventos_taclado(Controlador_Juego *controlador, int tecla, bool estado);
void controlar_eventos(Controlador_Juego *controlador, SDL_Event *evento);

int main (int /*n*/, char **/*argumentos*/)
{
	Pantalla::Ancho = ANCHO;
	Pantalla::Alto = ALTO;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");//No suspende el compositor de ventana (No funciona en devuan mate)
	//error: ‘SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR’ was not declared in this scope (devuan mate)
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
	SDL_Window *ventana = SDL_CreateWindow("Concertista Midi", 0, 0, ANCHO, ALTO, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	/*SDL_GLContext contexto = */SDL_GL_CreateContext(ventana);
	SDL_GL_SetSwapInterval(1);//Limita a 60 fps

	glewExperimental = GL_TRUE;
	GLenum estado = glewInit();
	if(estado != GLEW_OK)
	{
		Registro::Error(std::string(reinterpret_cast<const char*>(glewGetErrorString(estado))));
		return 0;
	}

	Registro::Nota("Concertista MIDI " + std::to_string(CONCERTISTAMIDI_VERSION_MAYOR) + "." + std::to_string(CONCERTISTAMIDI_VERSION_MENOR));
	Registro::Mostrar_detalles();

	configurar_gl();

	Administrador_Recursos recursos;
	Controlador_Juego controlador(&recursos);

	while (!controlador.terminar())
	{
		SDL_Event evento;
		while (SDL_PollEvent(&evento))
			controlar_eventos(&controlador, &evento);

		glClear(GL_COLOR_BUFFER_BIT);

		controlador.actualizar();

		//Pantalla Completa
		if(controlador.es_pantalla_completa() && !Pantalla::PantallaCompleta)
		{
			SDL_SetWindowFullscreen(ventana, SDL_WINDOW_FULLSCREEN_DESKTOP);
			Pantalla::PantallaCompleta = true;
		}
		else if(!controlador.es_pantalla_completa() && Pantalla::PantallaCompleta)
		{
			SDL_SetWindowFullscreen(ventana, 0);
			Pantalla::PantallaCompleta = false;
		}

		//Modo Alambre
		if(controlador.modo_alambre_activado() && !Pantalla::ModoAlambre)
		{
			glPolygonMode(GL_FRONT, GL_LINE);
			Pantalla::ModoAlambre = true;
		}
		else if(!controlador.modo_alambre_activado() && Pantalla::ModoAlambre)
		{
			glPolygonMode(GL_FRONT, GL_FILL);
			Pantalla::ModoAlambre = false;
		}

		SDL_GL_SwapWindow(ventana);
	}

	SDL_DestroyWindow(ventana);
	SDL_Quit();

	return 0;
}

void configurar_gl()
{
	glEnable (GL_CULL_FACE);
	glViewport(0, 0, ANCHO, ALTO);
	glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void controlar_eventos(Controlador_Juego *controlador, SDL_Event *evento)
{
	if (evento->type == SDL_KEYDOWN)
		eventos_taclado(controlador, evento->key.keysym.sym, true);
	else if (evento->type == SDL_KEYUP)
		eventos_taclado(controlador, evento->key.keysym.sym, false);
	else if(evento->type == SDL_MOUSEBUTTONDOWN)
		eventos_raton_botones(controlador, evento->button.button, evento->button.state, evento->button.clicks);
	else if(evento->type == SDL_MOUSEBUTTONUP)
		eventos_raton_botones(controlador, evento->button.button, evento->button.state, evento->button.clicks);
	else if(evento->type == SDL_MOUSEMOTION)
		eventos_raton_posicion(controlador, evento->button.x, evento->button.y);
	else if(evento->type == SDL_MOUSEWHEEL)
		eventos_raton_rueda(controlador, evento->wheel.x, evento->wheel.y);
	else if (evento->type == SDL_WINDOWEVENT)
	{
		if(evento->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			ajustar_ventana(controlador, evento->window.data1, evento->window.data2);
		else if (evento->window.event == SDL_WINDOWEVENT_CLOSE)
			controlador->evento_salir();
	}
}

void ajustar_ventana(Controlador_Juego *controlador, int ancho_nuevo, int alto_nuevo)
{
	glViewport(0, 0, ancho_nuevo, alto_nuevo);
	controlador->evento_ventana(static_cast<float>(ancho_nuevo), static_cast<float>(alto_nuevo));
}

void eventos_raton_botones(Controlador_Juego *controlador, int boton, int accion, int numero_clics)
{
	Raton *raton = controlador->raton();

	bool estado = false;
	if(accion == SDL_PRESSED)
		estado = true;
	else if(accion == SDL_RELEASED)
		estado = false;

	if(boton == SDL_BUTTON_LEFT)
		raton->actualizar_boton(BotonIzquierdo, estado, numero_clics);
	else if(boton == SDL_BUTTON_MIDDLE)
		raton->actualizar_boton(BotonCentral, estado, numero_clics);
	else if(boton == SDL_BUTTON_RIGHT)
		raton->actualizar_boton(BotonDerecho, estado, numero_clics);
	controlador->eventos_raton();
}
void eventos_raton_posicion(Controlador_Juego *controlador, int x, int y)
{
	Raton *raton = controlador->raton();
	raton->actualizar_posicion(x, y);
	controlador->eventos_raton();
}

void eventos_raton_rueda(Controlador_Juego *controlador, int desplazamiento_x, int desplazamiento_y)
{
	Raton *raton = controlador->raton();
	raton->actualizar_desplazamiento(desplazamiento_x, desplazamiento_y);
	controlador->eventos_raton();
	raton->actualizar_desplazamiento(0, 0);//Elimina el evento
}

void eventos_taclado(Controlador_Juego *controlador, int tecla, bool estado)
{
	switch(tecla)
	{
		case SDLK_0: controlador->eventos_teclado(TECLA_0, estado); break;
		case SDLK_1: controlador->eventos_teclado(TECLA_1, estado); break;
		case SDLK_2: controlador->eventos_teclado(TECLA_2, estado); break;
		case SDLK_3: controlador->eventos_teclado(TECLA_3, estado); break;
		case SDLK_4: controlador->eventos_teclado(TECLA_4, estado); break;
		case SDLK_5: controlador->eventos_teclado(TECLA_5, estado); break;
		case SDLK_6: controlador->eventos_teclado(TECLA_6, estado); break;
		case SDLK_7: controlador->eventos_teclado(TECLA_7, estado); break;
		case SDLK_8: controlador->eventos_teclado(TECLA_8, estado); break;
		case SDLK_9: controlador->eventos_teclado(TECLA_9, estado); break;

		case SDLK_a: controlador->eventos_teclado(TECLA_A, estado); break;
		case SDLK_b: controlador->eventos_teclado(TECLA_B, estado); break;
		case SDLK_c: controlador->eventos_teclado(TECLA_C, estado); break;
		case SDLK_d: controlador->eventos_teclado(TECLA_D, estado); break;
		case SDLK_e: controlador->eventos_teclado(TECLA_E, estado); break;
		case SDLK_f: controlador->eventos_teclado(TECLA_F, estado); break;
		case SDLK_g: controlador->eventos_teclado(TECLA_G, estado); break;
		case SDLK_h: controlador->eventos_teclado(TECLA_H, estado); break;
		case SDLK_i: controlador->eventos_teclado(TECLA_I, estado); break;
		case SDLK_j: controlador->eventos_teclado(TECLA_J, estado); break;
		case SDLK_k: controlador->eventos_teclado(TECLA_K, estado); break;
		case SDLK_l: controlador->eventos_teclado(TECLA_L, estado); break;
		case SDLK_m: controlador->eventos_teclado(TECLA_M, estado); break;
		case SDLK_n: controlador->eventos_teclado(TECLA_N, estado); break;
		case 241: controlador->eventos_teclado(TECLA_NN, estado); break;//Ñ
		case SDLK_o: controlador->eventos_teclado(TECLA_O, estado); break;
		case SDLK_p: controlador->eventos_teclado(TECLA_P, estado); break;
		case SDLK_q: controlador->eventos_teclado(TECLA_Q, estado); break;
		case SDLK_r: controlador->eventos_teclado(TECLA_R, estado); break;
		case SDLK_s: controlador->eventos_teclado(TECLA_S, estado); break;
		case SDLK_t: controlador->eventos_teclado(TECLA_T, estado); break;
		case SDLK_u: controlador->eventos_teclado(TECLA_U, estado); break;
		case SDLK_v: controlador->eventos_teclado(TECLA_V, estado); break;
		case SDLK_w: controlador->eventos_teclado(TECLA_W, estado); break;
		case SDLK_x: controlador->eventos_teclado(TECLA_X, estado); break;
		case SDLK_y: controlador->eventos_teclado(TECLA_Y, estado); break;
		case SDLK_z: controlador->eventos_teclado(TECLA_Z, estado); break;

		case SDLK_LEFT: controlador->eventos_teclado(TECLA_FLECHA_IZQUIERDA, estado); break;
		case SDLK_RIGHT: controlador->eventos_teclado(TECLA_FLECHA_DERECHA, estado); break;
		case SDLK_UP: controlador->eventos_teclado(TECLA_FLECHA_ARRIBA, estado); break;
		case SDLK_DOWN: controlador->eventos_teclado(TECLA_FLECHA_ABAJO, estado); break;

		case SDLK_SPACE: controlador->eventos_teclado(TECLA_ESPACIO, estado); break;
		case SDLK_BACKSPACE: controlador->eventos_teclado(TECLA_BORRAR, estado); break;
		case SDLK_PAUSE: controlador->eventos_teclado(TECLA_PAUSA, estado); break;
		case SDLK_ESCAPE: controlador->eventos_teclado(TECLA_ESCAPE, estado); break;
		case SDLK_RETURN: controlador->eventos_teclado(TECLA_ENTRAR, estado); break;
		case SDLK_KP_ENTER: controlador->eventos_teclado(TECLA_ENTRAR, estado); break;
		case SDLK_TAB: controlador->eventos_teclado(TECLA_TABULADOR, estado); break;
		case SDLK_QUOTE: controlador->eventos_teclado(TECLA_APOSTRPFE, estado); break;
		case 1073741824: controlador->eventos_teclado(TECLA_ACENTOGRAVE, estado); break;
		case 161: controlador->eventos_teclado(TECLA_EXCLAMACIONINICIO, estado); break;
		case SDLK_PLUS: controlador->eventos_teclado(TECLA_SUMA, estado); break;

		case SDLK_KP_MINUS: controlador->eventos_teclado(TECLA_RESTA_NUMERICO, estado); break;
		case SDLK_KP_PLUS: controlador->eventos_teclado(TECLA_SUMA_NUMERIO, estado); break;

		case SDLK_F1: controlador->eventos_teclado(TECLA_F1, estado); break;
		case SDLK_F2: controlador->eventos_teclado(TECLA_F2, estado); break;
		case SDLK_F3: controlador->eventos_teclado(TECLA_F3, estado); break;
		case SDLK_F4: controlador->eventos_teclado(TECLA_F4, estado); break;
		case SDLK_F5: controlador->eventos_teclado(TECLA_F5, estado); break;
		case SDLK_F6: controlador->eventos_teclado(TECLA_F6, estado); break;
		case SDLK_F7: controlador->eventos_teclado(TECLA_F7, estado); break;
		case SDLK_F8: controlador->eventos_teclado(TECLA_F8, estado); break;
		case SDLK_F9: controlador->eventos_teclado(TECLA_F9, estado); break;
		case SDLK_F10: controlador->eventos_teclado(TECLA_F10, estado); break;
		case SDLK_F11: controlador->eventos_teclado(TECLA_F11, estado); break;
		case SDLK_F12: controlador->eventos_teclado(TECLA_F12, estado); break;
		case SDLK_INSERT: controlador->eventos_teclado(TECLA_INSERTAR, estado); break;
		case SDLK_DELETE: controlador->eventos_teclado(TECLA_SUPRIMIR, estado); break;
		case SDLK_HOME: controlador->eventos_teclado(TECLA_INICIO, estado); break;
		case SDLK_END: controlador->eventos_teclado(TECLA_FIN, estado); break;
		case SDLK_PAGEUP: controlador->eventos_teclado(TECLA_REPAG, estado); break;
		case SDLK_PAGEDOWN: controlador->eventos_teclado(TECLA_AVPAG, estado); break;
		default: return;
	}
}
