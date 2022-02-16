/*
 * Launchpad.cc - Copyright (c) 2001-2022 - Olivier Poncet
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
#include "Launchpad.h"

// ---------------------------------------------------------------------------
// <anonymous>::helper
// ---------------------------------------------------------------------------

namespace {

struct helper
{
    using Layout = novation::Launchpad::Layout;

    static uint8_t layout_byte(uint8_t layout)
    {
        switch(layout) {
            case Layout::GRID_LAYOUT:
            case Layout::DRUM_LAYOUT:
                return layout;
            default:
                break;
        }
        return Layout::GRID_LAYOUT;
    }

    static uint8_t buffer_byte(uint8_t display, uint8_t update, bool flash, bool copy)
    {
        const uint8_t b7 = static_cast<uint8_t>(false)        << 7; // always false
        const uint8_t b6 = static_cast<uint8_t>(false)        << 6; // always false
        const uint8_t b5 = static_cast<uint8_t>(true)         << 5; // always true
        const uint8_t b4 = static_cast<uint8_t>(copy)         << 4; // copy flag
        const uint8_t b3 = static_cast<uint8_t>(flash)        << 3; // flash flag
        const uint8_t b2 = static_cast<uint8_t>(update != 0)  << 2; // update buffer
        const uint8_t b1 = static_cast<uint8_t>(false)        << 1; // always false
        const uint8_t b0 = static_cast<uint8_t>(display != 0) << 0; // display buffer
        const uint8_t rc = (b7 | b6 | b5 | b4 | b3 | b2 | b1 | b0);

        return rc;
    }
};

}

// ---------------------------------------------------------------------------
// novation::Launchpad
// ---------------------------------------------------------------------------

namespace novation {

Launchpad::Launchpad(const std::string& name)
    : _name(name)
    , _midi(name)
{
}

Launchpad::Launchpad(const std::string& name, const std::string& inout)
    : Launchpad(name)
{
    open(inout);
}

Launchpad::Launchpad(const std::string& name, const std::string& in, const std::string& out)
    : Launchpad(name)
{
    open(in, out);
}

Launchpad::~Launchpad()
{
    close();
}

void Launchpad::open(const std::string& inout)
{
    open(inout, inout);
}

void Launchpad::open(const std::string& in, const std::string& out)
{
    if(Midi::open(*(_midi.in), in, _name + ' ' + "client input") == false) {
        throw std::runtime_error("*** unable to open MIDI input ***");
    }
    if(Midi::open(*(_midi.out), out, _name + ' ' + "client output") == false) {
        throw std::runtime_error("*** unable to open MIDI output ***");
    }
}

void Launchpad::close()
{
    static_cast<void>(Midi::close(*(_midi.in)));
    static_cast<void>(Midi::close(*(_midi.out)));
}

void Launchpad::reset()
{
    constexpr uint8_t channel    = Midi::CHANNEL_01_CONTROL_CHANGE;
    constexpr uint8_t controller = Midi::CONTROLLER_BANK_SELECT;
    constexpr uint8_t value      = 0x00;

    Midi::send(*(_midi.out), channel, controller, value);
}

void Launchpad::setLayout(uint8_t layout)
{
    constexpr uint8_t channel    = Midi::CHANNEL_01_CONTROL_CHANGE;
    constexpr uint8_t controller = Midi::CONTROLLER_BANK_SELECT;
    const     uint8_t value      = helper::layout_byte(layout);

    Midi::send(*(_midi.out), channel, controller, value);
}

void Launchpad::setBuffer(uint8_t display, uint8_t update, bool flash, bool copy)
{
    constexpr uint8_t channel    = Midi::CHANNEL_01_CONTROL_CHANGE;
    constexpr uint8_t controller = Midi::CONTROLLER_BANK_SELECT;
    const     uint8_t value      = helper::buffer_byte(display, update, flash, copy);

    Midi::send(*(_midi.out), channel, controller, value);
}

void Launchpad::setPad(uint8_t pad, uint8_t color)
{
    constexpr uint8_t channel  = Midi::CHANNEL_01_NOTE_ON;
    const     uint8_t note     = pad;
    const     uint8_t velocity = color;

    Midi::send(*(_midi.out), channel, note, velocity);
}

void Launchpad::enumerateInputs(std::vector<std::string>& inputs)
{
    Midi::enumerate(*(_midi.in), inputs);
}

void Launchpad::enumerateOutputs(std::vector<std::string>& outputs)
{
    Midi::enumerate(*(_midi.out), outputs);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
