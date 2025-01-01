/*
 * Midi.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Midi.h"

// ---------------------------------------------------------------------------
// countof macro
// ---------------------------------------------------------------------------

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

// ---------------------------------------------------------------------------
// novation::Midi
// ---------------------------------------------------------------------------

namespace novation {

bool Midi::open(RtMidi& midi, const std::string& port, const std::string& name)
{
    const size_t pos = 0;
    const size_t len = port.length();
    const unsigned int count = midi.getPortCount();
    for(unsigned int index = 0; index < count; ++index) {
        const std::string current(midi.getPortName(index));
        if(current.compare(pos, len, port) == 0) {
            midi.openPort(index, name);
            return true;
        }
    }
    return false;
}

bool Midi::close(RtMidi& midi)
{
    if(midi.isPortOpen() != false) {
        midi.closePort();
    }
    return true;
}

void Midi::send(RtMidiOut& midi, uint8_t byte0, uint8_t byte1, uint8_t byte2)
{
    const uint8_t    data[] = { byte0, byte1, byte2 };
    constexpr size_t size   = countof(data); 

    midi.sendMessage(data, size);
}

void Midi::enumerate(RtMidi& midi, std::vector<std::string>& ports)
{
    std::vector<std::string> list;
    const unsigned int count = midi.getPortCount();
    for(unsigned int index = 0; index < count; ++index) {
        const std::string port(midi.getPortName(index));
        list.push_back(port);
    }
    ports.swap(list);
}

}

// ---------------------------------------------------------------------------
// novation::MidiAdapter
// ---------------------------------------------------------------------------

namespace novation {

MidiAdapter::MidiAdapter(const std::string& name)
    : in (new RtMidiIn (RtMidi::UNSPECIFIED, name))
    , out(new RtMidiOut(RtMidi::UNSPECIFIED, name))
{
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
