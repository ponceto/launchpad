/*
 * Launchpad.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __NOVATION_Launchpad_h__
#define __NOVATION_Launchpad_h__

#include <novation/Midi.h>

// ---------------------------------------------------------------------------
// novation::Launchpad
// ---------------------------------------------------------------------------

namespace novation {

class Launchpad
{
public: // public interface
    Launchpad(const std::string& name);

    Launchpad(const std::string& name, const std::string& inout);

    Launchpad(const std::string& name, const std::string& in, const std::string& out);

    virtual ~Launchpad();

    virtual void open(const std::string& inout);

    virtual void open(const std::string& in, const std::string& out);

    virtual void close();

    virtual void reset();

    virtual void setGridLayout();

    virtual void setDrumLayout();

    virtual void setBuffer(uint8_t display, uint8_t update, bool flash, bool copy);

    virtual void setPad(uint8_t pad, uint8_t color);

    virtual void setPad(uint8_t row, uint8_t col, uint8_t color);

    virtual void clearPad(uint8_t pad);

    virtual void clearPad(uint8_t row, uint8_t col);

    virtual uint8_t makeColor(uint8_t red, uint8_t green, bool copy = false, bool clear = false);

    virtual int enumerateInputs(std::vector<std::string>& inputs);

    virtual int enumerateOutputs(std::vector<std::string>& outputs);

public: // public static data
    static const uint8_t ROWS = 8;
    static const uint8_t COLS = 8;

    static const uint8_t BRIGHTNESS_OFF    = 0b00000000;
    static const uint8_t BRIGHTNESS_LOW    = 0b01010101;
    static const uint8_t BRIGHTNESS_MEDIUM = 0b10101010;
    static const uint8_t BRIGHTNESS_FULL   = 0b11111111;

protected: // protected data
    const std::string _name;
    const MidiAdapter _midi;

private: // disable copy and assignment
    Launchpad(const Launchpad&) = delete;
    Launchpad& operator=(const Launchpad&) = delete;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __NOVATION_Launchpad_h__ */
