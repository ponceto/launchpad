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
#include <random>
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

    static void set(Launchpad& launchpad, const uint8_t row, const uint8_t col, const uint8_t color)
    {
        if((row < ROWS) && (col < COLS)) {
            launchpad.setPad(((row * STRIDE) + col), color);
        }
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

    static void cycle(Launchpad& launchpad, const uint64_t delay, bool& shutdown)
    {
        for(int r = 0; r < 256; r += 64) {
            if(shutdown != false) {
                break;
            }
            for(int g = 0; g < 256; g += 64) {
                if(shutdown != false) {
                    break;
                }
                lp::clear(launchpad, lp::color(r, g));
                lp::sleep(launchpad, delay);
            }
        }
    }

    static void print(Launchpad& launchpad, const std::string& string, const uint8_t foreground, const uint8_t background, const uint64_t delay, bool& shutdown)
    {
        for(auto character : string) {
            if(shutdown != false) {
                break;
            }
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
        }
    }

    static void scroll(Launchpad& launchpad, const std::string& string, const uint8_t foreground, const uint8_t background, const uint64_t delay, bool& shutdown)
    {
        const char*  data = string.c_str();
        const size_t size = string.size();
        const int    pixs = 8 * (size > 0 ? size - 1 : 0);

        for(uint8_t pix = 0; pix < pixs; ++pix) {
            if(shutdown != false) {
                break;
            }
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
// <anonymous>::GameOfLife
// ---------------------------------------------------------------------------

namespace {

class GameOfLife
{
public: // public interface
    GameOfLife(Launchpad& launchpad)
        : _launchpad(launchpad)
        , _none(lp::color(0, 0))
        , _dead(lp::color(255, 0))
        , _fate(lp::color(255, 255))
        , _live(lp::color(0, 255))
        , _world()
        , _state()
    {
        lp::reset(_launchpad);
    }

   ~GameOfLife()
    {
        lp::clear(_launchpad, _none);
    }

    void run(const std::string& type, const uint64_t delay, const bool& shutdown)
    {
        if(type == "glider") {
            init_glider(1, 1);
        }
        else {
            init_random();
        }
        while(shutdown == false) {
            show();
            copy();
            next();
            lp::sleep(_launchpad, delay);
        }
    }

private: // private interface
    void set(const uint8_t row, const uint8_t col, const uint8_t state)
    {
        if((row < ROWS) && (col < COLS)) {
            _world[row][col] = state;
        }
    }

    uint8_t get(const uint8_t row, const uint8_t col)
    {
        if((row < ROWS) && (col < COLS)) {
            return _state[row][col];
        }
        return _none;
    }

    uint8_t count(const uint8_t row, const uint8_t col)
    {
        uint8_t count = 0;

        if(get(row - 1, col - 1) == _live) ++count;
        if(get(row - 1, col    ) == _live) ++count;
        if(get(row - 1, col + 1) == _live) ++count;

        if(get(row    , col - 1) == _live) ++count;
        if(get(row    , col + 1) == _live) ++count;

        if(get(row + 1, col - 1) == _live) ++count;
        if(get(row + 1, col    ) == _live) ++count;
        if(get(row + 1, col + 1) == _live) ++count;

        return count;
    }

    void init_random()
    {
        std::random_device entropy;
        std::mt19937 generator(entropy());
        std::uniform_int_distribution<uint8_t> distribute(0, 1);

        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                set(row, col, (distribute(generator) != 0 ? _live : _none));
            }
        }
    }

    void init_glider(const uint8_t row, const uint8_t col)
    {
        set((row - 1), (col - 1), _none);
        set((row - 1), (col    ), _live);
        set((row - 1), (col + 1), _none);
        set((row    ), (col - 1), _none);
        set((row    ), (col    ), _none);
        set((row    ), (col + 1), _live);
        set((row + 1), (col - 1), _live);
        set((row + 1), (col    ), _live);
        set((row + 1), (col + 1), _live);
    }

    void copy()
    {
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                _state[row][col] = _world[row][col];
            }
        }
    }

    void next()
    {
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                uint8_t& cell = _world[row][col];
                const uint8_t neighbors = count(row, col);
                if(cell == _live) {
                    if((neighbors <= 1) || (neighbors >= 4)) {
                        cell = _fate;
                    }
                }
                else if(neighbors == 3) {
                    cell = _live;
                }
                else if(cell == _fate) {
                    cell = _dead;
                }
                else if(cell == _dead) {
                    cell = _none;
                }
                else {
                    cell = _none;
                }
            }
        }
    }

    void show()
    {
        _launchpad.setBuffer(0, 1, false, false);
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                const uint8_t& cell = _world[row][col];
                lp::set(_launchpad, row, col, cell);
            }
        }
        _launchpad.setBuffer(1, 0, false, true );
        _launchpad.setBuffer(0, 0, false, false);
    }

private: // private static data
    static constexpr uint8_t ROWS = 8;
    static constexpr uint8_t COLS = 8;

private: // private data
    Launchpad&    _launchpad;
    const uint8_t _none;
    const uint8_t _dead;
    const uint8_t _fate;
    const uint8_t _live;
    uint8_t       _world[ROWS][COLS];
    uint8_t       _state[ROWS][COLS];
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
    , _delay(delay)
    , _black(lp::color(0, 0))
    , _red(lp::color(255, 0))
    , _green(lp::color(0, 255))
    , _yellow(lp::color(255, 255))
    , _shutdown(false)
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
    lp::list_inputs(_launchpad, _console.printStream);
    lp::list_outputs(_launchpad, _console.printStream);
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
    lp::reset(_launchpad);
    lp::cycle(_launchpad, _delay, _shutdown);
    lp::clear(_launchpad, _black);
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
    lp::reset(_launchpad);
    lp::print(_launchpad, _string, _red, _black, _delay, _shutdown);
    lp::clear(_launchpad, _black);
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
    lp::reset(_launchpad);
    lp::scroll(_launchpad, _string, _red, _black, _delay, _shutdown);
    lp::clear(_launchpad, _black);
}

// ---------------------------------------------------------------------------
// LaunchpadGameOfLifeApp
// ---------------------------------------------------------------------------

LaunchpadGameOfLifeApp::LaunchpadGameOfLifeApp ( const ArgList&     arglist
                                               , const Console&     console
                                               , Launchpad&         launchpad
                                               , const std::string& string
                                               , const uint64_t     delay )
    : LaunchpadApp(arglist, console, launchpad, delay)
    , _string(string)
{
}

LaunchpadGameOfLifeApp::~LaunchpadGameOfLifeApp()
{
}

void LaunchpadGameOfLifeApp::main()
{
    GameOfLife(_launchpad).run(_string, _delay, _shutdown);
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
