// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __MIDI_TRACK_H
#define __MIDI_TRACK_H

#include <vector>
#include <string>
#include <map>

#include "MidiEvent.h++"

class MidiEvent;

typedef std::vector<MidiEvent> MidiEventList;
typedef std::vector<unsigned long> MidiEventPulsesList;
typedef std::vector<microseconds_t> MidiEventMicrosecondList;

class MidiTrack
{
public:
	static MidiTrack ReadFromStream(std::istream &stream);
	static std::vector<MidiTrack> DividirPistas(MidiTrack pista);
	static MidiTrack CreateBlankTrack();
	MidiEventList &Events();
	MidiEventPulsesList &EventPulses();
	MidiEventMicrosecondList &EventUsecs();
	const MidiEventList &Events() const;
	const MidiEventPulsesList &EventPulses() const;
	const MidiEventMicrosecondList &EventUsecs() const;
	void SetEventUsecs(const MidiEventMicrosecondList &event_usecs);
	const std::string InstrumentName() const;
	bool IsPercussion() const;
	bool esVarios() const;
	const NoteSet &Notes() const;
	void SetTrackId(size_t track_id);
	unsigned char canal();

	// Reports whether this track contains any Note-On MIDI events
	// (vs. just being an information track with a title or copyright)
	bool hasNotes() const;
	void Reset();
	MidiEventList Update(microseconds_t delta_microseconds);
	void GoTo(microseconds_t microsecond_song_position);

	unsigned int AggregateEventsRemain() const;
	unsigned int AggregateEventCount() const;
	unsigned int AggregateNotesRemain() const;
	unsigned int AggregateNoteCount() const;

private:
	MidiTrack();

	void BuildNoteSet();
	void DiscoverInstrument();

	MidiEventList m_events;
	MidiEventPulsesList m_event_pulses;
	MidiEventMicrosecondList m_event_usecs;

	NoteSet m_note_set;

	int m_instrument_id;
	unsigned char m_canal;

	microseconds_t m_running_microseconds;
	long m_last_event;

	unsigned int m_notes_remaining;
};

#endif
