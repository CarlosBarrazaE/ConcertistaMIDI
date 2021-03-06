#ifndef DEFINICIONES_MIDI
#define DEFINICIONES_MIDI

enum EventoMidi
{
	EventoMidi_Nulo,
	EventoMidi_NotaApagada = 0x80,//Incluye el canal 0
	EventoMidi_NotaEncendida = 0x90,//Incluye el canal 0
	EventoMidi_DespuesDeTocarNota = 0xA0,//Incluye el canal 0
	EventoMidi_Controlador = 0xB0,//Incluye el canal 0
	EventoMidi_CambioPrograma = 0xC0,//Incluye el canal 0
	EventoMidi_DespuesDeTocarCanal = 0xD0,//Incluye el canal 0
	EventoMidi_InflexionDeTono = 0xE0,//Incluye el canal 0
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

enum MensajeControlador
{
	MensajeControlador_SelccionDeBanco_BEI = 0x00,
	MensajeControlador_RuedaDeModulacion_BEI = 0x01,
	MensajeControlador_ControlDeRespiracion_BEI = 0x02,
	MensajeControlador_Indefinido = 0x03,
	MensajeControlador_ControlDePie_BEI = 0x04,
	MensajeControlador_TiempoDePortamento_BEI = 0x05,
	MensajeControlador_EntradaDeDatos_BEI = 0x06,
	MensajeControlador_VolumenDelCanal_BEI = 0x07,
	MensajeControlador_Balance_BEI = 0x08,
	//MensajeControlador_Indefinido = 0x09,
	MensajeControlador_Pan_BEI = 0x0A,
	MensajeControlador_ControlDeExpresion_BEI = 0x0B,
	MensajeControlador_Efecto1_BEI = 0x0C,
	MensajeControlador_Efecto2_BEI = 0x0D,
	//MensajeControlador_Indefinido = 0x0E,
	//MensajeControlador_Indefinido = 0x0F,
	MensajeControlador_PropositoGeneral1_BEI = 0x10,
	MensajeControlador_PropositoGeneral2_BEI = 0x11,
	MensajeControlador_PropositoGeneral3_BEI = 0x12,
	MensajeControlador_PropositoGeneral4_BEI = 0x13,
	//MensajeControlador_Indefinido = 0x14,
	//MensajeControlador_Indefinido = 0x15,
	//MensajeControlador_Indefinido = 0x16,
	//MensajeControlador_Indefinido = 0x17,
	//MensajeControlador_Indefinido = 0x18,
	//MensajeControlador_Indefinido = 0x19,
	//MensajeControlador_Indefinido = 0x1A,
	//MensajeControlador_Indefinido = 0x1B,
	//MensajeControlador_Indefinido = 0x1C,
	//MensajeControlador_Indefinido = 0x1D,
	//MensajeControlador_Indefinido = 0x1E,
	//MensajeControlador_Indefinido = 0x1F,
	MensajeControlador_SeleccionDeBanco_BED = 0x20,
	MensajeControlador_RuedaDeModulacion_BED = 0x21,
	MensajeControlador_ControlDeRespiracion_BED = 0x22,
	//MensajeControlador_Indefinido = 0x23,
	MensajeControlador_ControlDePie_BED = 0x24,
	MensajeControlador_TiempoDePortamento_BED = 0x25,
	MensajeControlador_EntradaDeDatos_BED = 0x26,
	MensajeControlador_VolumenDelCanal_BED = 0x27,
	MensajeControlador_Balance_BED = 0x28,
	MensajeControlador_Indefinido_BED = 0x29,
	MensajeControlador_Pan_BED = 0x2A,
	MensajeControlador_ControlDeExpresion_BED = 0x2B,
	MensajeControlador_Efecto1_BED = 0x2C,
	MensajeControlador_Efecto2_BED = 0x2D,
	//MensajeControlador_Indefinido = 0x2E,
	//MensajeControlador_Indefinido = 0x2F,
	MensajeControlador_PropositoGeneral1_BED = 0x30,
	MensajeControlador_PropositoGeneral2_BED = 0x31,
	MensajeControlador_PropositoGeneral3_BED = 0x32,
	MensajeControlador_PropositoGeneral4_BED = 0x33,
	//MensajeControlador_Indefinido = 0x34,
	//MensajeControlador_Indefinido = 0x35,
	//MensajeControlador_Indefinido = 0x36,
	//MensajeControlador_Indefinido = 0x37,
	//MensajeControlador_Indefinido = 0x38,
	//MensajeControlador_Indefinido = 0x39,
	//MensajeControlador_Indefinido = 0x3A,
	//MensajeControlador_Indefinido = 0x3B,
	//MensajeControlador_Indefinido = 0x3C,
	//MensajeControlador_Indefinido = 0x3D,
	//MensajeControlador_Indefinido = 0x3E,
	//MensajeControlador_Indefinido = 0x3F,
	MensajeControlador_PedalDeAmortiguador = 0x40,
	MensajeControlador_Portamento = 0x41,
	MensajeControlador_Sostenuto = 0x42,
	MensajeControlador_PedalSuave = 0x43,
	MensajeControlador_PedalLegato = 0x44,
	MensajeControlador_Sostenido = 0x45,
	MensajeControlador_Sonido1 = 0x46,
	MensajeControlador_Sonido2 = 0x47,
	MensajeControlador_Sonido3 = 0x48,
	MensajeControlador_Sonido4 = 0x49,
	MensajeControlador_Sonido5 = 0x4A,
	MensajeControlador_Sonido6 = 0x4B,
	MensajeControlador_Sonido7 = 0x4C,
	MensajeControlador_Sonido8 = 0x4D,
	MensajeControlador_Sonido9 = 0x4E,
	MensajeControlador_Sonido10 = 0x4F,
	MensajeControlador_PropositoGeneral5 = 0x50,
	MensajeControlador_PropositoGeneral6 = 0x51,
	MensajeControlador_PropositoGeneral7 = 0x52,
	MensajeControlador_PropositoGeneral8 = 0x53,
	MensajeControlador_ControlDePortamento = 0x54,
	//MensajeControlador_Indefinido = 0x55,
	//MensajeControlador_Indefinido = 0x56,
	//MensajeControlador_Indefinido = 0x57,
	//MensajeControlador_Indefinido = 0x58,
	//MensajeControlador_Indefinido = 0x59,
	//MensajeControlador_Indefinido = 0x5A,
	MensajeControlador_EfectoProfundida1 = 0x5B,
	MensajeControlador_EfectoProfundida2 = 0x5C,
	MensajeControlador_EfectoProfundida3 = 0x5D,
	MensajeControlador_EfectoProfundida4 = 0x5E,
	MensajeControlador_EfectoProfundida5 = 0x5F,
	MensajeControlador_EntradaDeDatosMas1 = 0x60,
	MensajeControlador_EntradaDeDatosMenos1 = 0x61,
	MensajeControlador_NoRegistrado_BED = 0x62,
	MensajeControlador_NoRegistrado_BEI = 0x63,
	MensajeControlador_Registrado_BED = 0x64,
	MensajeControlador_Registrado_BEI = 0x65,
	//MensajeControlador_Indefinido = 0x66,
	//MensajeControlador_Indefinido = 0x67,
	//MensajeControlador_Indefinido = 0x68,
	//MensajeControlador_Indefinido = 0x69,
	//MensajeControlador_Indefinido = 0x6A,
	//MensajeControlador_Indefinido = 0x6B,
	//MensajeControlador_Indefinido = 0x6C,
	//MensajeControlador_Indefinido = 0x6D,
	//MensajeControlador_Indefinido = 0x6E,
	//MensajeControlador_Indefinido = 0x6F,
	//MensajeControlador_Indefinido = 0x70,
	//MensajeControlador_Indefinido = 0x71,
	//MensajeControlador_Indefinido = 0x72,
	//MensajeControlador_Indefinido = 0x73,
	//MensajeControlador_Indefinido = 0x74,
	//MensajeControlador_Indefinido = 0x75,
	//MensajeControlador_Indefinido = 0x76,
	//MensajeControlador_Indefinido = 0x77,
	MensajeControlador_TodosLosSonidosApagados = 0x78,
	MensajeControlador_RestablecerTodo = 0x79,
	MensajeControlador_ControlLocal = 0x7A,
	MensajeControlador_TodasLasNotasApagadas = 0x7B,
	MensajeControlador_ModoOmniApagados = 0x7C,
	MensajeControlador_ModoOmniActivado = 0x7D,
	MensajeControlador_ModoPoli = 0x7E,
	MensajeControlador_ModoPoliActivado = 0x7F,
};

#endif
