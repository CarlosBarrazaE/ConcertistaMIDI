// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#include "MidiTrack.h++"

MidiTrack MidiTrack::ReadFromStream(std::istream &stream)
{
	// Verify the track header
	const static std::string MidiTrackHeader = "MTrk";

	// I could use (MidiTrackHeader.length() + 1), but then this has to be
	// dynamically allocated.  More hassle than it's worth.  MIDI is well
	// defined and will always have a 4-byte header.  We use 5 so we get
	// free null termination.
	char header_id[5] = { 0, 0, 0, 0, 0 };
	uint32_t track_length;

	stream.read(header_id, static_cast<std::streamsize>(MidiTrackHeader.length()));
	stream.read(reinterpret_cast<char*>(&track_length), sizeof(uint32_t));

	if (stream.fail())
		throw MidiError(MidiError_TrackHeaderTooShort);

	std::string header(header_id);
	if (header != MidiTrackHeader)
		throw MidiError_BadTrackHeaderType;

	// Pull the full track out of the file all at once -- there is an
	// End-Of-Track event, but this allows us handle malformed MIDI a
	// little more gracefully.
	track_length = BigToSystem32(track_length);
	char *buffer = new char[track_length + 1];
	buffer[track_length] = 0;

	stream.read(buffer, track_length);
	if (stream.fail())
	{
		delete[] buffer;
		throw MidiError(MidiError_TrackTooShort);
	}

	// We have to jump through a couple hoops because istringstream
	// can't handle binary data unless constructed through an std::string.
	std::string buffer_string(buffer, track_length);
	std::istringstream event_stream(buffer_string, std::ios::binary);
	delete[] buffer;

	MidiTrack t;

	// Read events until we run out of track
	unsigned char last_status = 0;
	unsigned long current_pulse_count = 0;
	while (event_stream.peek() != std::char_traits<char>::eof())
	{
		MidiEvent ev = MidiEvent::ReadFromStream(event_stream, last_status);
		last_status = ev.StatusCode();

		t.m_events.push_back(ev);

		current_pulse_count += ev.GetDeltaPulses();
		t.m_event_pulses.push_back(current_pulse_count);
	}

	t.BuildNoteSet();
	t.DiscoverInstrument();

	return t;
}

std::vector<MidiTrack> MidiTrack::DividirPistas(MidiTrack pista_dividir)
{
	std::vector<MidiTrack> pistas_nuevas;
	std::map<unsigned int, unsigned int> id_pistas;
	std::map<unsigned char, int> programa_canal;//Almacena el ultimo programa por canal

	unsigned int contador_pistas = 0;
	unsigned int programaycanal = 0;
	unsigned char canal_actual;

	for(unsigned int x = 0; x<pista_dividir.m_events.size(); x++)
	{
		//Se dividen las pistas por canal y por programa
		//Pueden haber varios cambios de programa en el mismo canal
		canal_actual = pista_dividir.m_events[x].Channel();
		if(pista_dividir.m_events[x].Type() == MidiEventType_ProgramChange && pista_dividir.m_events[x].ProgramNumber() != programa_canal[canal_actual])
			programa_canal[canal_actual] = pista_dividir.m_events[x].ProgramNumber();

		//programaycanal se almacena de la siguiente manera ppcc donde p es el programa y c el canal
		//por ese motivo se multiplica por 100, para dejar los primeros 2 digitos para el canal
		programaycanal = (static_cast<unsigned int>(programa_canal[canal_actual]) * 100) + canal_actual;
		if(id_pistas.count(programaycanal) == 0)
		{
			//Se guarda el id de la pista nueva
			id_pistas[programaycanal] = contador_pistas;

			//Se crea una nueva pista
			MidiTrack p;

			p.m_events.push_back(pista_dividir.m_events[x]);
			p.m_event_pulses.push_back(pista_dividir.m_event_pulses[x]);

			pistas_nuevas.push_back(p);

			contador_pistas++;
		}
		else
		{
			//Se inserta en la pista existente
			unsigned int numero_pista = id_pistas[programaycanal];
			pistas_nuevas[numero_pista].m_events.push_back(pista_dividir.m_events[x]);
			pistas_nuevas[numero_pista].m_event_pulses.push_back(pista_dividir.m_event_pulses[x]);
		}
	}

	for(unsigned int x = 0; x < pistas_nuevas.size(); x++)
	{
		pistas_nuevas[x].BuildNoteSet();
		pistas_nuevas[x].DiscoverInstrument();
	}

	return pistas_nuevas;
}

MidiTrack MidiTrack::CreateBlankTrack()
{
	return MidiTrack();
}

MidiEventList &MidiTrack::Events()
{
	return m_events;
}

MidiEventPulsesList &MidiTrack::EventPulses()
{
	return m_event_pulses;
}

MidiEventMicrosecondList &MidiTrack::EventUsecs()
{
	return m_event_usecs;
}

const MidiEventList &MidiTrack::Events() const
{
	return m_events;
}

const MidiEventPulsesList &MidiTrack::EventPulses() const
{
	return m_event_pulses;
}

const MidiEventMicrosecondList &MidiTrack::EventUsecs() const
{
	return m_event_usecs;
}

void MidiTrack::SetEventUsecs(const MidiEventMicrosecondList &event_usecs)
{
	m_event_usecs = event_usecs;
}

const std::string MidiTrack::InstrumentName() const
{
	return InstrumentNames[m_instrument_id];
}

bool MidiTrack::IsPercussion() const
{
	return m_instrument_id == InstrumentIdPercussion;
}

bool MidiTrack::esVarios() const
{
	return m_instrument_id == InstrumentIdVarious;
}

const NoteSet &MidiTrack::Notes() const
{
	return m_note_set;
}

bool MidiTrack::hasNotes() const
{
	return (m_note_set.size() > 0);
}

unsigned int MidiTrack::AggregateEventsRemain() const
{
	return static_cast<unsigned int>(m_events.size() - static_cast<unsigned int>(m_last_event + 1));
}

unsigned int MidiTrack::AggregateEventCount() const
{
	return static_cast<unsigned int>(m_events.size());
}

unsigned int MidiTrack::AggregateNotesRemain() const
{
	return m_notes_remaining;
}

unsigned int MidiTrack::AggregateNoteCount() const
{
	return static_cast<unsigned int>(m_note_set.size());
}

MidiTrack::MidiTrack() : m_instrument_id(0)
{
	Reset();
}

struct NoteInfo
{
   int velocity;
   unsigned char channel;
   unsigned long pulses;
};

void MidiTrack::BuildNoteSet()
{
	m_note_set.clear();

	// Keep a list of all the notes currently "on" (and the pulse that
	// it was started).  On a note_on event, we create an element.  On
	// a note_off event we check that an element exists, make a "Note",
	// and remove the element from the list.  If there is already an
	// element on a note_on we both cap off the previous "Note" and
	// begin a new one.
	//
	// A note_on with velocity 0 is a note_off
	std::map<NoteId, NoteInfo> active_notes;

	for (size_t i = 0; i < m_events.size(); ++i)
	{
		const MidiEvent &ev = m_events[i];
		if (ev.Type() != MidiEventType_NoteOn && ev.Type() != MidiEventType_NoteOff)
			continue;

		bool on = (ev.Type() == MidiEventType_NoteOn && ev.NoteVelocity() > 0);
		NoteId id = ev.NoteNumber();

		// Check for an active note
		std::map<NoteId, NoteInfo>::iterator find_ret = active_notes.find(id);
		bool active_event = (find_ret !=  active_notes.end());

		// Close off the last event if there was one
		if (active_event)
		{
			Note n;
			n.start = find_ret->second.pulses;
			n.end = m_event_pulses[i];
			n.note_id = id;
			n.channel = find_ret->second.channel;
			n.velocity = find_ret->second.velocity;

			// NOTE: This must be set at the next level up.  The track
			// itself has no idea what its index is.
			n.track_id = 0;

			// Add a note and remove this NoteId from the active list
			m_note_set.insert(n);
			active_notes.erase(find_ret);
		}

		// We've handled any active events.  If this was a note_off we're done.
		if (!on)
			continue;

		// Add a new active event
		NoteInfo info;
		info.channel = ev.Channel();
		info.velocity = ev.NoteVelocity();
		info.pulses = m_event_pulses[i];

		active_notes[id] = info;
	}

	if (active_notes.size() > 0)
	{
		// LOGTODO!

		// This is mostly non-critical.
		//
		// Erroring out would be needlessly restrictive against
		// promiscuous MIDI files.  As-is, a note just won't be
		// inserted if it isn't closed properly.
	}
}

void MidiTrack::DiscoverInstrument()
{
	// Default to Program 0 per the MIDI Standard
	m_instrument_id = 0;
	bool instrument_found = false;

	// These are actually 10 and 16 in the MIDI standard.  However, MIDI
	// channels are 1-based facing the user.  They're stored 0-based.
	const static int PercussionChannel1 = 9;
	const static int PercussionChannel2 = 15;

	// Check to see if any/all of the notes
	// in this track use Channel 10.
	bool any_note_uses_percussion = false;
	bool any_note_does_not_use_percussion = false;

	bool canal_encontrado = false;
	for (size_t i = 0; i < m_events.size(); ++i)
	{
		const MidiEvent &ev = m_events[i];

		if (ev.Type() != MidiEventType_NoteOn)
			continue;

		if (ev.Channel() == PercussionChannel1 || ev.Channel() == PercussionChannel2)
			any_note_uses_percussion = true;

		if (ev.Channel() != PercussionChannel1 && ev.Channel() != PercussionChannel2)
			any_note_does_not_use_percussion = true;

		if(!canal_encontrado)
		{
			canal_encontrado = true;
			m_canal = static_cast<unsigned char>(ev.Channel()+1);
		}
	}

	if (any_note_uses_percussion && !any_note_does_not_use_percussion)
	{
		m_instrument_id = InstrumentIdPercussion;
		return;
	}

	if (any_note_uses_percussion && any_note_does_not_use_percussion)
	{
		m_instrument_id = InstrumentIdVarious;
		return;
	}

	for (size_t i = 0; i < m_events.size(); ++i)
	{
		const MidiEvent &ev = m_events[i];

		if (ev.Type() != MidiEventType_ProgramChange)
			continue;

		// If we've already hit a different instrument in this
		// same track, just tag it as "various" and exit early
		//
		// Also check that the same instrument isn't just set
		// multiple times in the same track
		if (instrument_found && m_instrument_id != ev.ProgramNumber())
		{
			m_instrument_id = InstrumentIdVarious;
			return;
		}

		m_instrument_id = ev.ProgramNumber();
		instrument_found = true;
	}
}

void MidiTrack::SetTrackId(size_t track_id)
{
	NoteSet old = m_note_set;

	m_note_set.clear();
	for (NoteSet::const_iterator i = old.begin(); i != old.end(); ++i)
	{
		Note n = *i;
		n.track_id = track_id;

		m_note_set.insert(n);
	}
}

unsigned char MidiTrack::canal()
{
	return m_canal;
}

void MidiTrack::Reset()
{
	m_running_microseconds = 0;
	m_last_event = -1;

	m_notes_remaining = static_cast<unsigned int>(m_note_set.size());
}

MidiEventList MidiTrack::Update(microseconds_t delta_microseconds)
{
	m_running_microseconds += delta_microseconds;

	MidiEventList evs;
	for (size_t i = static_cast<size_t>(m_last_event + 1); i < m_events.size(); ++i)
	{
		if (m_event_usecs[i] <= m_running_microseconds)
		{
			evs.push_back(m_events[i]);
			m_last_event = static_cast<long>(i);

			if (m_events[i].Type() == MidiEventType_NoteOn && m_events[i].NoteVelocity() > 0)
				m_notes_remaining--;
		}
		else
			break;
	}

	return evs;
}

void MidiTrack::GoTo(microseconds_t microsecond_song_position)
{
	m_running_microseconds = microsecond_song_position;
	m_last_event = -1;
	m_notes_remaining = static_cast<unsigned int>(m_note_set.size());

	for (size_t i = 0; i < m_events.size(); ++i)
	{
		if (m_event_usecs[i] <= m_running_microseconds)
		{
			m_last_event = static_cast<long>(i);

			if (m_events[i].Type() == MidiEventType_NoteOn && m_events[i].NoteVelocity() > 0)
				m_notes_remaining--;
		}
		else
			break;
	}
}
