// Linthesia

// Copyright (c) 2007 Nicholas Piegdon
// Adaptation to GNU/Linux by Oscar Aceña
// See COPYING for license information

#ifndef __MIDI_COMM_H
#define __MIDI_COMM_H

#include <iostream>
#include <string>
#include <vector>
#include <alsa/asoundlib.h>

#include "MidiEvent.h++"
#include "../elementos_graficos/notificacion.h++"
#include "../registro.h++"

struct MidiCommDescription
{
	unsigned int id;
	std::string name;
	int client;
	int port;
};

typedef std::vector<MidiCommDescription> MidiCommDescriptionList;

// Start/Stop midi services (i.e. open/close sequencer)
void midiInit();
void midiStop();

// Emulate MIDI keyboard using PC keyboard
void sendNote(const unsigned int note, bool on);
void doRetrieveDevices(unsigned int perms, MidiCommDescriptionList& devices);

// Once you create a MidiCommIn object.  Use the Read() function
// to grab one event at a time from the buffer.
class MidiCommIn
{
public:
	static MidiCommDescriptionList GetDeviceList();
	static void UpdateDeviceList();

	// device_id is obtained from GetDeviceList()
	MidiCommIn(unsigned int device_id);
	~MidiCommIn();

	MidiCommDescription GetDeviceDescription() const
	{
		return m_description;
	}

	// Returns the next buffered input event.  Use KeepReading() (usually in
	// a while loop) to see if you should call this function.  If called when
	// KeepReading() is false, this will throw MidiError_NoInputAvailable.
	MidiEvent Read();

	// Discard events from the input buffer
	void Reset();

	// Returns whether the input device has more buffered events.
	bool KeepReading() const;
	bool ShouldReconnect() const;
	void Reconnect();

	private:
	MidiCommDescription m_description;
	bool m_should_reconnect;
};

class MidiCommOut
{
public:
	static MidiCommDescriptionList GetDeviceList();
	static void UpdateDeviceList();

	// device_id is obtained from GetDeviceList()
	MidiCommOut(unsigned int device_id);
	~MidiCommOut();

	MidiCommDescription GetDeviceDescription() const
	{
		return m_description;
	}

	// Send a single event out to the device.
	void Write(const MidiEvent &out);
	void evento_sysex(char *datos, unsigned int largo);

	// Turns all notes off and resets all controllers
	void Reset();
	void Reconnect();

private:
	MidiCommDescription m_description;
	std::vector<std::pair<int,int>> m_notes_on;
};

#endif
