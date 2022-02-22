#ifndef DEFINICIONES_MIDI
#define DEFINICIONES_MIDI

enum EventoMidi
{
	EventoMidi_Nulo,
	EventoMidi_NotaApagada = 0x80,//Incluye el canal 0
	EventoMidi_NotaEncendida = 0x90,//Incluye el canal 0
	EventoMidi_DespuesDeTocarTecla = 0xA0,//Incluye el canal 0
	EventoMidi_CambioControl = 0xB0,//Incluye el canal 0
	EventoMidi_CambioPrograma = 0xC0,//Incluye el canal 0
	EventoMidi_DespuesDeTocarCanal = 0xD0,//Incluye el canal 0
	EventoMidi_CambioRuedaDeTono = 0xE0,//Incluye el canal 0
	EventoMidi_ExclusivoDelSistema = 0xF0,

	EventoMidi_SecuenciaDeEscape = 0xF7,

	EventoMidi_Metaevento = 0xFF,

	EventoMidi_ClienteConectado,
	EventoMidi_ClienteDesconectado,
	EventoMidi_PuertoConectado,
	EventoMidi_PuertoDesconectado,
	EventoMidi_PuertoSuscrito,
};

enum MetaEventoMidi
{
	MetaEventoMidi_SecuenciaNumerica = 0x00,
	MetaEventoMidi_Texto = 0x01,
	MetaEventoMidi_DerechosDeAutor = 0x02,
	MetaEventoMidi_NombreDePista = 0x03,
	MetaEventoMidi_NombreDeInstrumento = 0x04,
	MetaEventoMidi_Letra = 0x05,
	MetaEventoMidi_Marcador = 0x06,
	MetaEventoMidi_Apunte = 0x07,
	MetaEventoMidi_NombreDelPrograma = 0x08,
	MetaEventoMidi_NombreDelDispositivo = 0x09,

	MetaEventoMidi_PrefijoCanal = 0x20,//Evento obsoleto
	MetaEventoMidi_PuertoMidi = 0x21,//Evento obsoleto

	MetaEventoMidi_FinDePista = 0x2F,

	MetaEventoMidi_CambioDeTempo = 0x51,

	MetaEventoMidi_AjusteDeSMPTE = 0x54,

	MetaEventoMidi_Compas = 0x58,
	MetaEventoMidi_Armadura = 0x59,

	MetaEventoMidi_EspecificoDelSecuenciador = 0x7F,

	MetaEventoMidi_NoImplementado,
	MetaEventoMidi_Nulo,
};

#endif
