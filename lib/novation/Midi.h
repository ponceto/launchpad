/*
 * Midi.h - Copyright (c) 2001-2023 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __NOVATION_Midi_h__
#define __NOVATION_Midi_h__

#include <RtMidi.h>

// ---------------------------------------------------------------------------
// novation::Midi
// ---------------------------------------------------------------------------

namespace novation {

using MidiInUniquePtr  = std::unique_ptr<RtMidiIn>;
using MidiOutUniquePtr = std::unique_ptr<RtMidiOut>;

}

// ---------------------------------------------------------------------------
// novation::Midi
// ---------------------------------------------------------------------------

namespace novation {

struct Midi
{
    static constexpr uint8_t CHANNEL_01_NOTE_OFF          = 0x80;
    static constexpr uint8_t CHANNEL_02_NOTE_OFF          = 0x81;
    static constexpr uint8_t CHANNEL_03_NOTE_OFF          = 0x82;
    static constexpr uint8_t CHANNEL_04_NOTE_OFF          = 0x83;
    static constexpr uint8_t CHANNEL_05_NOTE_OFF          = 0x84;
    static constexpr uint8_t CHANNEL_06_NOTE_OFF          = 0x85;
    static constexpr uint8_t CHANNEL_07_NOTE_OFF          = 0x86;
    static constexpr uint8_t CHANNEL_08_NOTE_OFF          = 0x87;
    static constexpr uint8_t CHANNEL_09_NOTE_OFF          = 0x88;
    static constexpr uint8_t CHANNEL_10_NOTE_OFF          = 0x89;
    static constexpr uint8_t CHANNEL_11_NOTE_OFF          = 0x8a;
    static constexpr uint8_t CHANNEL_12_NOTE_OFF          = 0x8b;
    static constexpr uint8_t CHANNEL_13_NOTE_OFF          = 0x8c;
    static constexpr uint8_t CHANNEL_14_NOTE_OFF          = 0x8d;
    static constexpr uint8_t CHANNEL_15_NOTE_OFF          = 0x8e;
    static constexpr uint8_t CHANNEL_16_NOTE_OFF          = 0x8f;

    static constexpr uint8_t CHANNEL_01_NOTE_ON           = 0x90;
    static constexpr uint8_t CHANNEL_02_NOTE_ON           = 0x91;
    static constexpr uint8_t CHANNEL_03_NOTE_ON           = 0x92;
    static constexpr uint8_t CHANNEL_04_NOTE_ON           = 0x93;
    static constexpr uint8_t CHANNEL_05_NOTE_ON           = 0x94;
    static constexpr uint8_t CHANNEL_06_NOTE_ON           = 0x95;
    static constexpr uint8_t CHANNEL_07_NOTE_ON           = 0x96;
    static constexpr uint8_t CHANNEL_08_NOTE_ON           = 0x97;
    static constexpr uint8_t CHANNEL_09_NOTE_ON           = 0x98;
    static constexpr uint8_t CHANNEL_10_NOTE_ON           = 0x99;
    static constexpr uint8_t CHANNEL_11_NOTE_ON           = 0x9a;
    static constexpr uint8_t CHANNEL_12_NOTE_ON           = 0x9b;
    static constexpr uint8_t CHANNEL_13_NOTE_ON           = 0x9c;
    static constexpr uint8_t CHANNEL_14_NOTE_ON           = 0x9d;
    static constexpr uint8_t CHANNEL_15_NOTE_ON           = 0x9e;
    static constexpr uint8_t CHANNEL_16_NOTE_ON           = 0x9f;

    static constexpr uint8_t CHANNEL_01_CONTROL_CHANGE    = 0xb0;
    static constexpr uint8_t CHANNEL_02_CONTROL_CHANGE    = 0xb1;
    static constexpr uint8_t CHANNEL_03_CONTROL_CHANGE    = 0xb2;
    static constexpr uint8_t CHANNEL_04_CONTROL_CHANGE    = 0xb3;
    static constexpr uint8_t CHANNEL_05_CONTROL_CHANGE    = 0xb4;
    static constexpr uint8_t CHANNEL_06_CONTROL_CHANGE    = 0xb5;
    static constexpr uint8_t CHANNEL_07_CONTROL_CHANGE    = 0xb6;
    static constexpr uint8_t CHANNEL_08_CONTROL_CHANGE    = 0xb7;
    static constexpr uint8_t CHANNEL_09_CONTROL_CHANGE    = 0xb8;
    static constexpr uint8_t CHANNEL_10_CONTROL_CHANGE    = 0xb9;
    static constexpr uint8_t CHANNEL_11_CONTROL_CHANGE    = 0xba;
    static constexpr uint8_t CHANNEL_12_CONTROL_CHANGE    = 0xbb;
    static constexpr uint8_t CHANNEL_13_CONTROL_CHANGE    = 0xbc;
    static constexpr uint8_t CHANNEL_14_CONTROL_CHANGE    = 0xbd;
    static constexpr uint8_t CHANNEL_15_CONTROL_CHANGE    = 0xbe;
    static constexpr uint8_t CHANNEL_16_CONTROL_CHANGE    = 0xbf;

    static constexpr uint8_t CONTROLLER_BANK_SELECT       = 0x00;
    static constexpr uint8_t CONTROLLER_MODULATION_WHEEL  = 0x01;
    static constexpr uint8_t CONTROLLER_BREATH_CONTROLLER = 0x02;
    static constexpr uint8_t CONTROLLER_FOOT_CONTROLLER   = 0x04;
    static constexpr uint8_t CONTROLLER_PORTAMENTO_TIME   = 0x05;
    static constexpr uint8_t CONTROLLER_DATA_ENTRY_SLIDER = 0x06;
    static constexpr uint8_t CONTROLLER_MAIN_VOLUME       = 0x07;
    static constexpr uint8_t CONTROLLER_BALANCE           = 0x08;
    static constexpr uint8_t CONTROLLER_PAN               = 0x0a;
    static constexpr uint8_t CONTROLLER_EXPRESSION        = 0x0b;
    static constexpr uint8_t CONTROLLER_EFFECT1           = 0x0c;
    static constexpr uint8_t CONTROLLER_EFFECT2           = 0x0d;

    static bool open(RtMidi& midi, const std::string& port, const std::string& name);

    static bool close(RtMidi& midi);

    static void send(RtMidiOut& midi, uint8_t byte0, uint8_t byte1, uint8_t byte2);

    static void enumerate(RtMidi& midi, std::vector<std::string>& ports);
};

}

// ---------------------------------------------------------------------------
// novation::MidiAdapter
// ---------------------------------------------------------------------------

namespace novation {

struct MidiAdapter
{
    MidiAdapter(const std::string& name);

    MidiInUniquePtr  in;
    MidiOutUniquePtr out;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __NOVATION_Midi_h__ */
