// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include "MidiUtil.h++"

MidiError::MidiError(MidiErrorCode error) : m_error(error)
{
}

uint32_t BigToSystem32(uint32_t x)
{
	return ntohl(x);
}

unsigned short BigToSystem16(unsigned short x)
{
	return ntohs(x);
}

unsigned long parse_variable_length(std::istream &in)
{
	unsigned long value = static_cast<unsigned int>(in.get());

	if (in.good() && (value & 0x80))
	{
		value &= 0x7F;

		unsigned long c;
		do
		{
			c = static_cast<unsigned int>(in.get());
			value = (value << 7) + (c & 0x7F);
		}
		while (in.good() && (c & 0x80));
	}

	return(value);
}

std::string MidiError::GetErrorDescription() const
{
	switch (m_error)
	{
		case MidiError_UnknownHeaderType:
			return "Se encontró un tipo de encabezado desconocido, es probable que no sea un archivo MIDI.";
		case MidiError_BadFilename:
			return "No se pudo abir el archivo de entrada, compruebe que el archivo existe";
		case MidiError_NoHeader:
			return "No se pudo leer ningún encavezado Midi, archivo muy corto.";
		case MidiError_BadHeaderSize:
			return "Tamaño incorrecto del encabezado.";
		case MidiError_Type2MidiNotSupported:
			return "MIDI tipo 2 no es compatible";
		case MidiError_BadType0Midi:
			return "El MIDI tipo 0 solo debe tener una pista";
		case MidiError_SMTPETimingNotImplemented:
			return "MIDI el uso de la división de tiempo SMTP no está implementada.";

		case MidiError_BadTrackHeaderType:
			return "Se encontró un tipo de encabezado de pista desconocido.";
		case MidiError_TrackHeaderTooShort:
			return "Archivo terminado antes de leer el encabezado de la pista.";
		case MidiError_TrackTooShort:
			return "Flujo de datos demasiado corto para leer la pista completa.";
		case MidiError_BadTrackEnd:
			return "La pista MIDI no terminó con el evento End-Of-Track (fin de pista).";

		case MidiError_EventTooShort:
			return "El flujo de datos finalizó antes del final del evento MIDI informado.";
		case MidiError_UnknownEventType:
			return "Se encontró un tipo de evento MIDI desconocido.";
		case MidiError_UnknownMetaEventType:
			return "Se encontró un tipo de evento de meta MIDI desconocido.";

		case MidiError_MM_NoDevice:
			return "No se pudo abrir el dispositivo MIDI espesificado.";
		case MidiError_MM_NotEnabled:
			return "El dispositivo MIDI no se pudo habilitar.";
		case MidiError_MM_AlreadyAllocated:
			return "El dispositivo MIDI espesificado ya está en uso.";
		case MidiError_MM_BadDeviceID:
			return "El ID del dispositivo MIDI espesificado está fuera de rango.";
		case MidiError_MM_InvalidParameter:
			return "Se usó un parárametro inválido con el dispositovo MIDI.";
		case MidiError_MM_NoDriver:
			return "El controlador MIDI espesificado no está instalado.";
		case MidiError_MM_NoMemory:
			return "No se puede asignar o bloquear la memoria para el dispositivo MIDI.";
		case MidiError_MM_Unknown:
			return "Se ha producido un error de E/S MIDI desconocido.";

		case MidiError_NoInputAvailable:
			return "Se intentó leer un evento MIDI desde un archivo de entrada vacío.";
		case MidiError_MetaEventOnInput:
			return "Un dispositivo de entrada MIDI envió un mata evento.";

		case MidiError_InputError:
			return "El controlador de entrada MIDI informó de un error.";
		case MidiError_InvalidInputErrorBehavior:
			return "Valor de InputError no válido. Las opciones son 'report', 'ignore', and 'use'";

		case MidiError_RequestedTempoFromNonTempoEvent:
			return "Los datos de tempo se solicitaron desde un evento MIDI diferente.";
		case MidiError_RequiereCompasDesdeEventoNoCompas:
			return "Los datos del compas fueron requerido desde un evento MIDI diferente.";

		default:
			return "Codigo de error desconocido: " + std::to_string(static_cast<unsigned int>(m_error)) + ".";
	}
}

std::string GetMidiEventTypeDescription(MidiEventType type)
{
	switch (type)
	{
		case MidiEventType_Meta:             return "Meta";
		case MidiEventType_SysEx:            return "System Exclusive";

		case MidiEventType_NoteOff:          return "Note-Off";
		case MidiEventType_NoteOn:           return "Note-On";
		case MidiEventType_Aftertouch:       return "Aftertouch";
		case MidiEventType_Controller:       return "Controller";
		case MidiEventType_ProgramChange:    return "Program Change";
		case MidiEventType_ChannelPressure:  return "Channel Pressure";
		case MidiEventType_PitchWheel:       return "Pitch Wheel";

		case MidiEventType_Unknown:          return "Unknown";
		default:                             return "BAD EVENT TYPE";
	}
}

std::string GetMidiMetaEventTypeDescription(MidiMetaEventType type)
{
  switch (type)
  {
	case MidiMetaEvent_SequenceNumber:   return "Sequence Number";

	case MidiMetaEvent_Text:             return "Text";
	case MidiMetaEvent_Copyright:        return "Copyright";
	case MidiMetaEvent_TrackName:        return "Track Name";
	case MidiMetaEvent_Instrument:       return "Instrument";
	case MidiMetaEvent_Lyric:            return "Lyric";
	case MidiMetaEvent_Marker:           return "Marker";
	case MidiMetaEvent_Cue:              return "Cue Point";
	case MidiMetaEvent_PatchName:        return "Patch Name";
	case MidiMetaEvent_DeviceName:       return "Device Name";

	case MidiMetaEvent_EndOfTrack:       return "End Of Track";
	case MidiMetaEvent_TempoChange:      return "Tempo Change";
	case MidiMetaEvent_SMPTEOffset:      return "SMPTE Offset";
	case MidiMetaEvent_TimeSignature:    return "Time Signature";
	case MidiMetaEvent_KeySignature:     return "Key Signature";

	case MidiMetaEvent_Proprietary:      return "Proprietary";

	case MidiMetaEvent_ChannelPrefix:    return "(Deprecated) Channel Prefix";
	case MidiMetaEvent_MidiPort:         return "(Deprecated) MIDI Port";

	case MidiMetaEvent_Unknown:          return "Unknown Meta Event Type";
	default:                             return "BAD META EVENT TYPE";
  }
}

std::string const InstrumentNames[InstrumentCount] = {
	"Piano de cola acústico",
	"Piano brillante acústico",
	"Piano de cola eléctrico",
	"Piano Honky tonk",
	"Piano Rhodes",
	"Piano con coro",
	"Clavicordio",
	"Clavecín",
	"Celesta",
	"Glockenspiel",
	"Caja de música",
	"Vibráfono",
	"Marimba",
	"Xilófono",
	"Campanas tubulares",
	"Dulcémele",
	"Órgano Hammond",
	"Órgano percusivo",
	"Órgano de rock",
	"Órgano de iglesia",
	"Armonio",
	"Acordeón",
	"Armónica",
	"Bandoneón",
	"Guitarra española",
	"Guitarra acústica",
	"Guitarra eléctrica (jazz)",
	"Guitarra eléctrica (limpia)",
	"Guitarra eléctrica (tapada)",
	"Guitarra saturada",
	"Guitarra distorsionada",
	"Armónicos de guitarra",
	"Bajo acústico",
	"Bajo eléctrico pulsado",
	"Bajo eléctrico con plectro",
	"Bajo sin trastes",
	"Bajo en slap 1",
	"Bajo en slap 2",
	"Bajo sintetizado 1",
	"Bajo sintetizado 2",
	"Violín",
	"Viola",
	"Violonchelo",
	"Contrabajo",
	"Cuerdas en trémolo",
	"Cuerdas en pizzicato",
	"Arpa",
	"Timbales",
	"Conjunto de cuerda 1",
	"Conjunto de cuerda 2",
	"Cuerdas sintetizadas 1",
	"Cuerdas sintetizadas 2",
	"Coro Aahs",
	"Coro Oohs",
	"Coro sintetizado",
	"Tutti de orquesta",
	"Trompeta",
	"Trombón",
	"Tuba",
	"Trompeta con sordina",
	"Corno francés",
	"Sección de metales",
	"Metales sintetizados 1",
	"Metales sintetizados 2",
	"Saxofón soprano",
	"Saxofón alto",
	"Saxofón tenor",
	"Saxofón barítono",
	"Oboe",
	"Corno inglés",
	"Fagot",
	"Clarinete",
	"Flautín",
	"Flauta travesera",
	"Flauta dulce",
	"Flauta de pan",
	"Botella",
	"Shakuhachi",
	"Silbato",
	"Ocarina",
	"Solo 1 (onda cuadrada)",
	"Solo 2 (diente de sierra)",
	"Solo 3 (órgano de vapor)",
	"Solo 4 (siseo)",
	"Solo 5 (charanga)",
	"Solo 6 (voz)",
	"Solo 7 (quintas)",
	"Solo 8 (metal y solo)",
	"Fondo 1 (nueva era)",
	"Fondo 2 (cálido)",
	"Fondo 3 (polisintetizador)",
	"Fondo 4 (coro)",
	"Fondo 5 (de arco)",
	"Fondo 6 (metálico)",
	"Fondo 7 (celestial)",
	"Fondo 8 (escobillas)",
	"Efecto 1 (lluvia)",
	"Efecto 2 (banda sonora)",
	"Efecto 3 (cristales)",
	"Efecto 4 (atmósfera)",
	"Efecto 5 (brillo)",
	"Efecto 6 (duendes)",
	"Efecto 7 (ecos)",
	"Efecto 8 (ciencia ficción)",
	"Sitar",
	"Banjo",
	"Shamisen",
	"Koto",
	"Kalimba",
	"Gaita",
	"Fídula",
	"Shannai",
	"Campanillas",
	"Agogô",
	"Tambores metálicos",
	"Caja china",
	"Taiko",
	"Tom melódico",
	"Caja sintetizada",
	"Plato invertido",
	"rasteo de guitarra",
	"Respiración",
	"Costa de mar",
	"Pájaros",
	"Timbre de teléfono",
	"Helicóptero",
	"Aplauso",
	"Disparo",

	//
	// NOTE: These aren't actually General MIDI instruments!
	//
	"Percusión", // for Tracks that use Channel 10 or 16
	"Varios"     // for Tracks that use more than one
};
