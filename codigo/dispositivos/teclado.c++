#include "teclado.h++"

unsigned char Teclado::Tecla_a_nota(int tecla)
{
	const unsigned char octava = 4;
	const unsigned char do_primera_octava = octava * 12;
	const unsigned char do_segunda_octava = (octava+1) * 12;

	switch(tecla)
	{
		case TECLA_TABULADOR:			return do_primera_octava;
		case TECLA_1:					return do_primera_octava + 1;
		case TECLA_Q:					return do_primera_octava + 2;
		case TECLA_2:					return do_primera_octava + 3;
		case TECLA_W:					return do_primera_octava + 4;
		case TECLA_E:					return do_primera_octava + 5;
		case TECLA_4:					return do_primera_octava + 6;
		case TECLA_R:					return do_primera_octava + 7;
		case TECLA_5:					return do_primera_octava + 8;
		case TECLA_T:					return do_primera_octava + 9;
		case TECLA_6:					return do_primera_octava + 10;
		case TECLA_Y:					return do_primera_octava + 11;

		case TECLA_U:					return do_segunda_octava;
		case TECLA_8:					return do_segunda_octava + 1;
		case TECLA_I:					return do_segunda_octava + 2;
		case TECLA_9:					return do_segunda_octava + 3;
		case TECLA_O:					return do_segunda_octava + 4;
		case TECLA_P:					return do_segunda_octava + 5;
		case TECLA_APOSTRPFE:			return do_segunda_octava + 6;
		case TECLA_ACENTOGRAVE:			return do_segunda_octava + 7;
		case TECLA_EXCLAMACIONINICIO:	return do_segunda_octava + 8;
		case TECLA_SUMA:				return do_segunda_octava + 9;
		case TECLA_BORRAR:				return do_segunda_octava + 10;
		case TECLA_ENTRAR:				return do_segunda_octava + 11;
		default:						return 255;
	}
}
