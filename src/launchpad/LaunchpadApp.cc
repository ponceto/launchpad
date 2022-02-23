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
        const int    pixs = COLS * (size > 0 ? size - 1 : 0);

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
};

}

// ---------------------------------------------------------------------------
// LaunchpadApp
// ---------------------------------------------------------------------------

LaunchpadApp::LaunchpadApp ( const Console&     console
                           , Launchpad&         launchpad
                           , const std::string& param
                           , const uint64_t     delay )
    : _console(console)
    , _launchpad(launchpad)
    , _param(param)
    , _delay(delay)
    , _black(lp::color(0, 0))
    , _red(lp::color(255, 0))
    , _green(lp::color(0, 255))
    , _amber(lp::color(255, 255))
    , _shutdown(false)
{
}

LaunchpadApp::~LaunchpadApp()
{
}

void LaunchpadApp::println(const std::string& message)
{
    if(_console.printStream.good()) {
        _console.printStream << message << std::endl;
    }
}

void LaunchpadApp::println(const std::string& prefix, const std::string& message)
{
    if(_console.printStream.good()) {
        _console.printStream << prefix << ' ' << message << std::endl;
    }
}

void LaunchpadApp::errorln(const std::string& message)
{
    if(_console.errorStream.good()) {
        _console.errorStream << message << std::endl;
    }
}

void LaunchpadApp::errorln(const std::string& prefix, const std::string& message)
{
    if(_console.errorStream.good()) {
        _console.errorStream << prefix << ' ' << message << std::endl;
    }
}

void LaunchpadApp::sleep(const uint64_t delay)
{
    std::this_thread::sleep_for(std::chrono::microseconds(delay));
}

// ---------------------------------------------------------------------------
// HelpApp
// ---------------------------------------------------------------------------

namespace launchpad {

HelpApp::HelpApp ( const Console&     console
                 , Launchpad&         launchpad
                 , const std::string& param
                 , const uint64_t     delay
                 , const std::string& program
                 , const std::string& midiIn
                 , const std::string& midiOut )
    : LaunchpadApp(console, launchpad, param, delay)
    , _program(program)
    , _midiIn(midiIn)
    , _midiOut(midiOut)
{
}

HelpApp::~HelpApp()
{
}

void HelpApp::main()
{
    std::ostream& printStream(_console.printStream);

    if(printStream.good()) {
        printStream << "Usage: " << _program << ' ' << "[OPTIONS]"                 << std::endl;
        printStream << ""                                                          << std::endl;
        printStream << "    -h, --help                  display this help"         << std::endl;
        printStream << "    -l, --list                  list available MIDI ports" << std::endl;
        printStream << ""                                                          << std::endl;
        printStream << "    --cycle                     cycle colors"              << std::endl;
        printStream << "    --print={text}              print a text"              << std::endl;
        printStream << "    --scroll={text}             scroll a text"             << std::endl;
        printStream << "    --game-of-life={pattern}    Conway's game of life"     << std::endl;
        printStream << ""                                                          << std::endl;
        printStream << "    --delay={value[us|ms|s|m]}  delay (ms by default)"     << std::endl;
        printStream << ""                                                          << std::endl;
        printStream << "    --midi={port}               MIDI input/output"         << std::endl;
        printStream << "    --midi-input={port}         MIDI input"                << std::endl;
        printStream << "    --midi-output={port}        MIDI output"               << std::endl;
        printStream << ""                                                          << std::endl;
        printStream << "MIDI input:"                                               << std::endl;
        printStream << ""                                                          << std::endl;
        printStream << "  - " << _midiIn                                           << std::endl;
        printStream << ""                                                          << std::endl;
        printStream << "MIDI output:"                                              << std::endl;
        printStream << ""                                                          << std::endl;
        printStream << "  - " << _midiOut                                          << std::endl;
        printStream << ""                                                          << std::endl;
    }
}

}

// ---------------------------------------------------------------------------
// ListApp
// ---------------------------------------------------------------------------

namespace launchpad {

ListApp::ListApp ( const Console&     console
                 , Launchpad&         launchpad
                 , const std::string& param
                 , const uint64_t     delay )
    : LaunchpadApp(console, launchpad, param, delay)
{
}

ListApp::~ListApp()
{
}

void ListApp::main()
{
    listInputs();
    listOutputs();
}

void ListApp::listInputs()
{
    std::vector<std::string> ports;

    println("Available MIDI inputs :");
    println();
    if(_launchpad.enumerateInputs(ports) > 0) {
        for(const std::string& port : ports) {
            println("[I]", port);
        }
    }
    println();
}

void ListApp::listOutputs()
{
    std::vector<std::string> ports;

    println("Available MIDI outputs :");
    println();
    if(_launchpad.enumerateOutputs(ports) > 0) {
        for(const std::string& port : ports) {
            println("[O]", port);
        }
    }
    println();
}

}

// ---------------------------------------------------------------------------
// launchpad::CycleApp
// ---------------------------------------------------------------------------

namespace launchpad {

CycleApp::CycleApp ( const Console&     console
                   , Launchpad&         launchpad
                   , const std::string& param
                   , const uint64_t     delay )
    : LaunchpadApp(console, launchpad, param, delay)
{
    lp::reset(_launchpad);
}

CycleApp::~CycleApp()
{
    lp::clear(_launchpad, _black);
}

void CycleApp::main()
{
    lp::cycle(_launchpad, _delay, _shutdown);
}

}

// ---------------------------------------------------------------------------
// launchpad::PrintApp
// ---------------------------------------------------------------------------

namespace launchpad {

PrintApp::PrintApp ( const Console&     console
                   , Launchpad&         launchpad
                   , const std::string& param
                   , const uint64_t     delay )
    : LaunchpadApp(console, launchpad, param, delay)
{
    lp::reset(_launchpad);
}

PrintApp::~PrintApp()
{
    lp::clear(_launchpad, _black);
}

void PrintApp::main()
{
    lp::print(_launchpad, _param, _red, _black, _delay, _shutdown);
}

}

// ---------------------------------------------------------------------------
// launchpad::ScrollApp
// ---------------------------------------------------------------------------

namespace launchpad {

ScrollApp::ScrollApp ( const Console&     console
                     , Launchpad&         launchpad
                     , const std::string& param
                     , const uint64_t     delay )
    : LaunchpadApp(console, launchpad, param, delay)
{
    lp::reset(_launchpad);
}

ScrollApp::~ScrollApp()
{
    lp::clear(_launchpad, _black);
}

void ScrollApp::main()
{
    lp::scroll(_launchpad, _param, _red, _black, _delay, _shutdown);
}

}

// ---------------------------------------------------------------------------
// launchpad::GameOfLifeApp
// ---------------------------------------------------------------------------

namespace launchpad {

GameOfLifeApp::GameOfLifeApp ( const Console&     console
                             , Launchpad&         launchpad
                             , const std::string& param
                             , const uint64_t     delay )
    : LaunchpadApp(console, launchpad, param, delay)
    , _color0(lp::color(0, 0))
    , _color1(lp::color(128, 0))
    , _color2(lp::color(255, 0))
    , _color3(lp::color(255, 255))
    , _color4(lp::color(0, 255))
    , _world()
    , _cache()
{
    lp::reset(_launchpad);
}

GameOfLifeApp::~GameOfLifeApp()
{
    lp::clear(_launchpad, _black);
}

void GameOfLifeApp::main()
{
    if(_shutdown == false) {
        init();
        do {
            loop();
        } while(_shutdown == false);
    }
}

void GameOfLifeApp::init()
{
    auto set = [&](const uint8_t row, const uint8_t col, const Cell state) -> void
    {
        _world.data[row % ROWS][col % COLS] = state;
    };

    auto init_random = [&]() -> void
    {
        std::random_device entropy;
        std::mt19937 generator(entropy());
        std::uniform_int_distribution<uint8_t> distribute(0, 1);

        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                const uint8_t value = distribute(generator);
                set(row, col, (value != 0 ? Cell::kLIVE : Cell::kNONE));
            }
        }
    };

    auto init_glider = [&](const uint8_t row, const uint8_t col) -> void
    {
        set((row - 1), (col - 1), Cell::kNONE);
        set((row - 1), (col | 0), Cell::kLIVE);
        set((row - 1), (col + 1), Cell::kNONE);
        set((row | 0), (col - 1), Cell::kNONE);
        set((row | 0), (col | 0), Cell::kNONE);
        set((row | 0), (col + 1), Cell::kLIVE);
        set((row + 1), (col - 1), Cell::kLIVE);
        set((row + 1), (col | 0), Cell::kLIVE);
        set((row + 1), (col + 1), Cell::kLIVE);
    };

    if(_param.empty()) {
        init_random();
    }
    else if(_param == "random") {
        init_random();
    }
    else if(_param == "glider") {
        init_glider(1, 1);
    }
    else {
        throw std::runtime_error("invalid pattern");
    }
}

void GameOfLifeApp::loop()
{
    bool stable = true;

    auto stateOrDeath = [&](const Cell cell, const uint8_t neighbors) -> Cell
    {
        if((neighbors <= 1) || (neighbors >= 4)) {
            return Cell::kKILL;
        }
        return cell;
    };

    auto stateOrBirth = [&](const Cell cell, const uint8_t neighbors) -> Cell
    {
        if(neighbors == 3) {
            return Cell::kLIVE;
        }
        return cell;
    };

    auto mutate = [&](const Cell cell, const uint8_t neighbors) -> Cell
    {
        switch(cell) {
            case Cell::kLIVE:
                return stateOrDeath(Cell::kLIVE, neighbors);
            case Cell::kKILL:
                return stateOrBirth(Cell::kDEAD, neighbors);
            case Cell::kDEAD:
                return stateOrBirth(Cell::kHIDE, neighbors);
            case Cell::kHIDE:
                return stateOrBirth(Cell::kNONE, neighbors);
            case Cell::kNONE:
                return stateOrBirth(Cell::kNONE, neighbors);
            default:
                break;
        }
        return cell;
    };

    auto neighbors = [&](const uint8_t row, const uint8_t col) -> uint8_t
    {
        uint8_t count = 0;

        if(_cache.get((row - 1), (col - 1)) == Cell::kLIVE) ++count;
        if(_cache.get((row - 1), (col | 0)) == Cell::kLIVE) ++count;
        if(_cache.get((row - 1), (col + 1)) == Cell::kLIVE) ++count;
        if(_cache.get((row | 0), (col - 1)) == Cell::kLIVE) ++count;
        if(_cache.get((row | 0), (col + 1)) == Cell::kLIVE) ++count;
        if(_cache.get((row + 1), (col - 1)) == Cell::kLIVE) ++count;
        if(_cache.get((row + 1), (col | 0)) == Cell::kLIVE) ++count;
        if(_cache.get((row + 1), (col + 1)) == Cell::kLIVE) ++count;

        return count;
    };

    auto color = [&](const Cell cell) -> uint8_t
    {
        switch(cell) {
            case Cell::kNONE:
                return _color0;
            case Cell::kHIDE:
                return _color1;
            case Cell::kDEAD:
                return _color2;
            case Cell::kKILL:
                return _color3;
            case Cell::kLIVE:
                return _color4;
            default:
                break;
        }
        return _black;
    };

    auto display = [&]() -> void
    {
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                const Cell& cell(_world.get(row, col));
                lp::set(_launchpad, row, col, color(cell));
            }
        }
    };

    auto prepare = [&]() -> void
    {
        _cache = _world;
    };

    auto process = [&]() -> void
    {
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                const Cell& prev(_cache.get(row, col));
                Cell&       cell(_world.get(row, col));
                cell = mutate(prev, neighbors(row, col));
                if(cell != prev) {
                    stable = false;
                }
            }
        }
    };

    auto finalize = [&]() -> void
    {
        if(stable != false) {
            init();
        }
        if(_shutdown == false) {
            sleep(_delay);
        }
    };

    display();
    prepare();
    process();
    finalize();
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
