/*
 * LaunchpadApp.cc - Copyright (c) 2001-2022 - Olivier Poncet
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
#include <chrono>
#include <thread>
#include "Font8x8.h"
#include "LaunchpadApp.h"

// ---------------------------------------------------------------------------
// <anonymous>::lp
// ---------------------------------------------------------------------------

namespace {

struct lp
{
    static constexpr uint8_t ROWS   = 8;
    static constexpr uint8_t COLS   = 8;
    static constexpr uint8_t STRIDE = 16;

    static uint8_t color(uint8_t r, uint8_t g, bool copy = false, bool clear = false)
    {
        const uint8_t flags = static_cast<uint8_t>(false) << 7 // must be 0
                            | static_cast<uint8_t>(false) << 6 // must be 0
                            | static_cast<uint8_t>(false) << 5 // green msb
                            | static_cast<uint8_t>(false) << 4 // green lsb
                            | static_cast<uint8_t>(clear) << 3 // clear
                            | static_cast<uint8_t>(copy)  << 2 // copy
                            | static_cast<uint8_t>(false) << 1 // red msb
                            | static_cast<uint8_t>(false) << 0 // red lsb
                            ;
        return flags
             | (((g >> 6) & 0x03) << 4)
             | (((r >> 6) & 0x03) << 0)
             ;
    }

    static void sleep(Launchpad& launchpad, const uint64_t delay)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(delay));
    }

    static void reset(Launchpad& launchpad)
    {
        launchpad.reset();
    }

    static void clear(Launchpad& launchpad, const uint8_t color)
    {
        launchpad.setBuffer(0, 1, false, false);
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                launchpad.setPad(((row * STRIDE) + col), color);
            }
        }
        launchpad.setBuffer(1, 0, false, true );
        launchpad.setBuffer(0, 0, false, false);
    }

    static void cycle(Launchpad& launchpad, const uint64_t delay, bool& terminate)
    {
        for(int r = 0; r < 256; r += 64) {
            for(int g = 0; g < 256; g += 64) {
                lp::clear(launchpad, lp::color(r, g));
                lp::sleep(launchpad, delay);
                if(terminate != false) {
                    break;
                }
            }
            if(terminate != false) {
                break;
            }
        }
    }

    static void print(Launchpad& launchpad, const std::string& string, const uint8_t foreground, const uint8_t background, const uint64_t delay, bool& terminate)
    {
        for(auto character : string) {
            launchpad.setBuffer(0, 1, false, false);
            const uint8_t index = (character < Font8x8::MAX_CHARS ? character : 0);
            for(uint8_t row = 0; row < ROWS; ++row) {
                const uint8_t val = Font8x8::data[index][row];
                uint8_t       bit = 0x01;
                for(uint8_t col = 0; col < COLS; ++col) {
                    if((val & bit) != 0) {
                        launchpad.setPad(((row * STRIDE) + col), foreground);
                    }
                    else {
                        launchpad.setPad(((row * STRIDE) + col), background);
                    }
                    bit <<= 1;
                }
            }
            launchpad.setBuffer(1, 0, false, true );
            launchpad.setBuffer(0, 0, false, false);
            lp::sleep(launchpad, delay);
            if(terminate != false) {
                break;
            }
        }
    }

    static void scroll(Launchpad& launchpad, const std::string& string, const uint8_t foreground, const uint8_t background, const uint64_t delay, bool& terminate)
    {
        const char*  data = string.c_str();
        const size_t size = string.size();
        const int    pixs = 8 * (size > 0 ? size - 1 : 0);

        for(uint8_t pix = 0; pix < pixs; ++pix) {
            launchpad.setBuffer(0, 1, false, false);
            for(uint8_t row = 0; row < ROWS; ++row) {
                for(uint8_t col = 0; col < COLS; ++col) {
                    const uint8_t chr = data[((col + pix) / COLS)];
                    const uint8_t bit = 1 << ((col + pix) % COLS);
                    const uint8_t val = Font8x8::data[chr][row];
                    if((val & bit) != 0) {
                        launchpad.setPad(((row * STRIDE) + col), foreground);
                    }
                    else {
                        launchpad.setPad(((row * STRIDE) + col), background);
                    }
                }
            }
            launchpad.setBuffer(1, 0, false, true );
            launchpad.setBuffer(0, 0, false, false);
            lp::sleep(launchpad, delay);
            if(terminate != false) {
                break;
            }
        }
    }

    static void list_inputs(Launchpad& launchpad, std::ostream& stream)
    {
        std::vector<std::string> ports;
        launchpad.enumerateInputs(ports);
        stream << "Available MIDI inputs :" << std::endl;
        stream << std::endl;
        for(const std::string& port : ports) {
            stream << "[I]" << ' ' << port << std::endl;
        }
        stream << std::endl;
    }

    static void list_outputs(Launchpad& launchpad, std::ostream& stream)
    {
        std::vector<std::string> ports;
        launchpad.enumerateOutputs(ports);
        stream << "Available MIDI outputs :" << std::endl;
        stream << std::endl;
        for(const std::string& port : ports) {
            stream << "[O]" << ' ' << port << std::endl;
        }
        stream << std::endl;
    }
};

}

// ---------------------------------------------------------------------------
// LaunchpadApp
// ---------------------------------------------------------------------------

LaunchpadApp::LaunchpadApp ( const ArgList& arglist
                           , const Console& console
                           , Launchpad&     launchpad
                           , uint64_t       delay )
    : _arglist(arglist)
    , _console(console)
    , _launchpad(launchpad)
    , _black(lp::color(0, 0))
    , _red(lp::color(255, 0))
    , _green(lp::color(0, 255))
    , _yellow(lp::color(255, 255))
    , _delay(delay)
    , _terminate(false)
{
}

LaunchpadApp::~LaunchpadApp()
{
}

// ---------------------------------------------------------------------------
// LaunchpadListApp
// ---------------------------------------------------------------------------

LaunchpadListApp::LaunchpadListApp ( const ArgList& arglist
                                   , const Console& console
                                   , Launchpad&     launchpad )
    : LaunchpadApp(arglist, console, launchpad, 0)
{
}

LaunchpadListApp::~LaunchpadListApp()
{
}

void LaunchpadListApp::main()
{
    /* list inputs */ {
        lp::list_inputs(_launchpad, _console.printStream);
    }
    /* list outputs */ {
        lp::list_outputs(_launchpad, _console.printStream);
    }
}

// ---------------------------------------------------------------------------
// LaunchpadCycleApp
// ---------------------------------------------------------------------------

LaunchpadCycleApp::LaunchpadCycleApp ( const ArgList& arglist
                                     , const Console& console
                                     , Launchpad&     launchpad
                                     , const uint64_t delay )
    : LaunchpadApp(arglist, console, launchpad, delay)
{
}

LaunchpadCycleApp::~LaunchpadCycleApp()
{
}

void LaunchpadCycleApp::main()
{
    /* cycle */ {
        lp::reset(_launchpad);
        lp::cycle(_launchpad, _delay, _terminate);
        lp::clear(_launchpad, _black);
    }
}

// ---------------------------------------------------------------------------
// LaunchpadPrintApp
// ---------------------------------------------------------------------------

LaunchpadPrintApp::LaunchpadPrintApp ( const ArgList&     arglist
                                     , const Console&     console
                                     , Launchpad&         launchpad
                                     , const std::string& string
                                     , const uint64_t     delay )
    : LaunchpadApp(arglist, console, launchpad, delay)
    , _string(string)
{
}

LaunchpadPrintApp::~LaunchpadPrintApp()
{
}

void LaunchpadPrintApp::main()
{
    /* print */ {
        lp::reset(_launchpad);
        lp::print(_launchpad, _string, _red, _black, _delay, _terminate);
        lp::clear(_launchpad, _black);
    }
}

// ---------------------------------------------------------------------------
// LaunchpadScrollApp
// ---------------------------------------------------------------------------

LaunchpadScrollApp::LaunchpadScrollApp ( const ArgList&     arglist
                                       , const Console&     console
                                       , Launchpad&         launchpad
                                       , const std::string& string
                                       , const uint64_t     delay )
    : LaunchpadApp(arglist, console, launchpad, delay)
    , _string(string)
{
}

LaunchpadScrollApp::~LaunchpadScrollApp()
{
}

void LaunchpadScrollApp::main()
{
    /* scroll */ {
        lp::reset(_launchpad);
        lp::scroll(_launchpad, _string, _red, _black, _delay, _terminate);
        lp::clear(_launchpad, _black);
    }
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
