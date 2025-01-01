/*
 * Launchpad.cc - Copyright (c) 2001-2025 - Olivier Poncet
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
// <anonymous>::lp
// ---------------------------------------------------------------------------

namespace {

struct lp
{
    enum Layout {
        CMD_RESET_BOARD = 0x00,
        CMD_GRID_LAYOUT = 0x01,
        CMD_DRUM_LAYOUT = 0x02,
    };

    static uint8_t buffer_byte(uint8_t display, uint8_t update, bool flash, bool copy)
    {
        const uint8_t b7 = static_cast<uint8_t>(false)        << 7; /* always false   */
        const uint8_t b6 = static_cast<uint8_t>(false)        << 6; /* always false   */
        const uint8_t b5 = static_cast<uint8_t>(true)         << 5; /* always true    */
        const uint8_t b4 = static_cast<uint8_t>(copy)         << 4; /* copy flag      */
        const uint8_t b3 = static_cast<uint8_t>(flash)        << 3; /* flash flag     */
        const uint8_t b2 = static_cast<uint8_t>(update != 0)  << 2; /* update buffer  */
        const uint8_t b1 = static_cast<uint8_t>(false)        << 1; /* always false   */
        const uint8_t b0 = static_cast<uint8_t>(display != 0) << 0; /* display buffer */
        const uint8_t rc = (b7 | b6 | b5 | b4 | b3 | b2 | b1 | b0);

        return rc;
    }

    static uint8_t color_byte(uint8_t r, uint8_t g, bool copy, bool clear)
    {
        const uint8_t flags = static_cast<uint8_t>(false) << 7 /* must be unset                             */
                            | static_cast<uint8_t>(false) << 6 /* must be unset                             */
                            | static_cast<uint8_t>(false) << 5 /* green brightness (most significant bit)   */
                            | static_cast<uint8_t>(false) << 4 /* green brightness (least significant bit)  */
                            | static_cast<uint8_t>(clear) << 3 /* clear the other buffer’s copy of this led */
                            | static_cast<uint8_t>(copy)  << 2 /* write this led data to both buffers       */
                            | static_cast<uint8_t>(false) << 1 /* red brightness (most significant bit)     */
                            | static_cast<uint8_t>(false) << 0 /* red brightness (least significant bit)    */
                            ;
        const uint8_t color = flags
                            | (((g >> 6) & 0x03) << 4)
                            | (((r >> 6) & 0x03) << 0)
                            ;
        return color;
    }

    static void errorCallback(RtMidiError::Type type, const std::string& message, void* userData)
    {
        novation::LaunchpadListener* listener(reinterpret_cast<novation::LaunchpadListener*>(userData));
        if(listener != nullptr) {
            listener->onLaunchpadError(message);
        }
    }

    static void inputCallback(double deltatime, std::vector<unsigned char>* message, void* userData)
    {
        novation::LaunchpadListener* listener(reinterpret_cast<novation::LaunchpadListener*>(userData));
        if(listener != nullptr) {
            listener->onLaunchpadInput(std::string(message->begin(), message->end()));
        }
    }
};

}

// ---------------------------------------------------------------------------
// novation::Launchpad
// ---------------------------------------------------------------------------

namespace novation {

Launchpad::Launchpad(const std::string& name)
    : _listener(nullptr)
    , _name(name)
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
    constexpr uint8_t value      = lp::CMD_RESET_BOARD;

    Midi::send(*(_midi.out), channel, controller, value);
}

void Launchpad::setGridLayout()
{
    constexpr uint8_t channel    = Midi::CHANNEL_01_CONTROL_CHANGE;
    constexpr uint8_t controller = Midi::CONTROLLER_BANK_SELECT;
    const     uint8_t value      = lp::CMD_GRID_LAYOUT;

    Midi::send(*(_midi.out), channel, controller, value);
}

void Launchpad::setDrumLayout()
{
    constexpr uint8_t channel    = Midi::CHANNEL_01_CONTROL_CHANGE;
    constexpr uint8_t controller = Midi::CONTROLLER_BANK_SELECT;
    const     uint8_t value      = lp::CMD_DRUM_LAYOUT;

    Midi::send(*(_midi.out), channel, controller, value);
}

void Launchpad::setBuffer(uint8_t display, uint8_t update, bool flash, bool copy)
{
    constexpr uint8_t channel    = Midi::CHANNEL_01_CONTROL_CHANGE;
    constexpr uint8_t controller = Midi::CONTROLLER_BANK_SELECT;
    const     uint8_t value      = lp::buffer_byte(display, update, flash, copy);

    Midi::send(*(_midi.out), channel, controller, value);
}

void Launchpad::setPad(uint8_t pad, uint8_t color)
{
    constexpr uint8_t channel  = Midi::CHANNEL_01_NOTE_ON;
    const     uint8_t note     = pad;
    const     uint8_t velocity = color;

    Midi::send(*(_midi.out), channel, note, velocity);
}

void Launchpad::setPad(uint8_t row, uint8_t col, uint8_t color)
{
    constexpr uint8_t channel  = Midi::CHANNEL_01_NOTE_ON;
    const     uint8_t note     = ((16 * row) + col);
    const     uint8_t velocity = color;

    Midi::send(*(_midi.out), channel, note, velocity);
}

void Launchpad::clearPad(uint8_t pad)
{
    constexpr uint8_t channel  = Midi::CHANNEL_01_NOTE_OFF;
    const     uint8_t note     = pad;
    const     uint8_t velocity = 0x00;

    Midi::send(*(_midi.out), channel, note, velocity);
}

void Launchpad::clearPad(uint8_t row, uint8_t col)
{
    constexpr uint8_t channel  = Midi::CHANNEL_01_NOTE_OFF;
    const     uint8_t note     = ((16 * row) + col);
    const     uint8_t velocity = 0x00;

    Midi::send(*(_midi.out), channel, note, velocity);
}

uint8_t Launchpad::makeColor(uint8_t red, uint8_t green, bool copy, bool clear)
{
    return lp::color_byte(red, green, copy, clear);
}

int Launchpad::enumerateInputs(std::vector<std::string>& inputs)
{
    Midi::enumerate(*(_midi.in), inputs);

    return inputs.size();
}

int Launchpad::enumerateOutputs(std::vector<std::string>& outputs)
{
    Midi::enumerate(*(_midi.out), outputs);

    return outputs.size();
}

void Launchpad::setListener(LaunchpadListener* listener)
{
    auto getInputMessage = [&]() -> bool
    {
        std::vector<unsigned char> message;
        static_cast<void>(_midi.in->getMessage(&message));
        return message.size() != 0;
    };

    auto flushInputMessages = [&]() -> void
    {
        while(getInputMessage() != false) {
            /* do nothing */
        }
    };

    auto setInputCallback = [&]() -> void
    {
        if((_listener == nullptr) && (listener != nullptr)) {
            _midi.in->setCallback(&lp::inputCallback, listener);
            _listener = listener;
        }
    };

    auto unsetInputCallback = [&]() -> void
    {
        if(_listener != nullptr) {
            _midi.in->cancelCallback();
            _listener = nullptr;
        }
    };

    unsetInputCallback();
    flushInputMessages();
    setInputCallback();
}

}

// ---------------------------------------------------------------------------
// novation::LaunchpadDecorator
// ---------------------------------------------------------------------------

namespace novation {

LaunchpadDecorator::LaunchpadDecorator(Launchpad& launchpad)
    : _launchpad(launchpad)
{
}

LaunchpadDecorator::~LaunchpadDecorator()
{
}

}

// ---------------------------------------------------------------------------
// novation::LaunchpadListener
// ---------------------------------------------------------------------------

namespace novation {

void LaunchpadListener::onLaunchpadError(const std::string& message)
{
}

void LaunchpadListener::onLaunchpadInput(const std::string& message)
{
    constexpr uint8_t GRID_KEY = Midi::CHANNEL_01_NOTE_ON;
    constexpr uint8_t LIVE_KEY = Midi::CHANNEL_01_CONTROL_CHANGE;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(message.data());
    const size_t   size = message.size();

    if(size == 3) {
        const uint8_t msg = data[0];
        const uint8_t key = data[1];
        const uint8_t val = data[2];
        if(msg == GRID_KEY) {
            onLaunchpadGridKey(key, val);
        }
        else if(msg == LIVE_KEY) {
            onLaunchpadLiveKey(key, val);
        }
    }
}

void LaunchpadListener::onLaunchpadGridKey(const uint8_t key, const uint8_t velocity)
{
    static_cast<void>(key);
    static_cast<void>(velocity);
}

void LaunchpadListener::onLaunchpadLiveKey(const uint8_t key, const uint8_t velocity)
{
    static_cast<void>(key);
    static_cast<void>(velocity);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
