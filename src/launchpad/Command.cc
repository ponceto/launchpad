/*
 * Command.cc - Copyright (c) 2001-2022 - Olivier Poncet
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
#include <cerrno>
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
#include "Command.h"

// ---------------------------------------------------------------------------
// <anonymous>::lp
// ---------------------------------------------------------------------------

namespace {

struct lp
{
    static constexpr uint8_t ROWS   = 8;
    static constexpr uint8_t COLS   = 8;
    static constexpr uint8_t STRIDE = 16;

    enum {
        kANY   = 0,
        kSET   = 1,
        kUNSET = 2,
    };

    static void assert_argument(const std::string& argument, const int expected)
    {
        switch(expected) {
            case kANY:
                break;
            case kSET:
                if(argument.empty() != false) {
                    throw std::runtime_error(std::string("missing argument"));
                }
                break;
            case kUNSET:
                if(argument.empty() == false) {
                    throw std::runtime_error(std::string("unexpected argument") + ' ' + '<' + argument + '>');
                }
                break;
            default:
                break;
        }
    };

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

    static void cycle(Launchpad& launchpad, const uint64_t delay, bool& stop)
    {
        for(int r = 0; r < 256; r += 64) {
            if(stop != false) {
                break;
            }
            for(int g = 0; g < 256; g += 64) {
                if(stop != false) {
                    break;
                }
                lp::clear(launchpad, lp::color(r, g));
                lp::sleep(launchpad, delay);
            }
        }
    }

    static void print(Launchpad& launchpad, const std::string& string, const uint8_t foreground, const uint8_t background, const uint64_t delay, bool& stop)
    {
        for(auto character : string) {
            if(stop != false) {
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

    static void scroll(Launchpad& launchpad, const std::string& string, const uint8_t foreground, const uint8_t background, const uint64_t delay, bool& stop)
    {
        const char*  data = string.c_str();
        const size_t size = string.size();
        const int    pixs = COLS * (size > 0 ? size - 1 : 0);

        for(int pix = 0; pix < pixs; ++pix) {
            if(stop != false) {
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
// Command
// ---------------------------------------------------------------------------

Command::Command ( const Console&     console
                 , Launchpad&         launchpad
                 , const std::string& argument1
                 , const std::string& argument2
                 , const std::string& argument3
                 , const std::string& argument4
                 , const uint64_t     delay )
    : _console(console)
    , _launchpad(launchpad)
    , _argument1(argument1)
    , _argument2(argument2)
    , _argument3(argument3)
    , _argument4(argument4)
    , _delay(delay)
    , _black(lp::color(0, 0))
    , _red(lp::color(255, 0))
    , _green(lp::color(0, 255))
    , _amber(lp::color(255, 255))
    , _stop(false)
{
}

Command::~Command()
{
}

void Command::println(const std::string& message)
{
    if(_console.printStream.good()) {
        _console.printStream << message << std::endl;
    }
}

void Command::println(const std::string& prefix, const std::string& message)
{
    if(_console.printStream.good()) {
        _console.printStream << prefix << ' ' << message << std::endl;
    }
}

void Command::errorln(const std::string& message)
{
    if(_console.errorStream.good()) {
        _console.errorStream << message << std::endl;
    }
}

void Command::errorln(const std::string& prefix, const std::string& message)
{
    if(_console.errorStream.good()) {
        _console.errorStream << prefix << ' ' << message << std::endl;
    }
}

void Command::sleep(const uint64_t delay)
{
    std::this_thread::sleep_for(std::chrono::microseconds(delay));
}

// ---------------------------------------------------------------------------
// HelpCmd
// ---------------------------------------------------------------------------

namespace launchpad {

HelpCmd::HelpCmd ( const Console&     console
                 , Launchpad&         launchpad
                 , const std::string& argument1
                 , const std::string& argument2
                 , const std::string& argument3
                 , const std::string& argument4
                 , const uint64_t     delay
                 , const std::string& program
                 , const std::string& midiIn
                 , const std::string& midiOut )
    : Command(console, launchpad, argument1, argument2, argument3, argument4, delay)
    , _program(program)
    , _midiIn(midiIn)
    , _midiOut(midiOut)
{
}

HelpCmd::~HelpCmd()
{
}

void HelpCmd::execute()
{
    if(_argument1.empty()) {
        baseHelp(_console.printStream);
        launchpad(_console.printStream);
    }
    else if(_argument1 == "help") {
        helpHelp(_console.printStream);
    }
    else if(_argument1 == "list") {
        listHelp(_console.printStream);
    }
    else if(_argument1 == "reset") {
        resetHelp(_console.printStream);
    }
    else if(_argument1 == "cycle") {
        cycleHelp(_console.printStream);
    }
    else if(_argument1 == "print") {
        printHelp(_console.printStream);
    }
    else if(_argument1 == "scroll") {
        scrollHelp(_console.printStream);
    }
    else if(_argument1 == "gameoflife") {
        gameoflifeHelp(_console.printStream);
    }
    else {
        throw std::runtime_error(std::string("no help for") + ' ' + '<' + _argument1 + '>');
    }
}

void HelpCmd::launchpad(std::ostream& stream)
{
    const char* const indent = "    ";

    if(stream.good()) {
        stream << indent << "+---------------------------------------+" << std::endl;
        stream << indent << "|  1   2   3   4   5   6   7   8        |" << std::endl;
        stream << indent << "| ( ) ( ) ( ) ( ) ( ) ( ) ( ) ( )       |" << std::endl;
        stream << indent << "|                                       |" << std::endl;
        stream << indent << "| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) A |" << std::endl;
        stream << indent << "| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) B |" << std::endl;
        stream << indent << "| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) C |" << std::endl;
        stream << indent << "| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) D |" << std::endl;
        stream << indent << "| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) E |" << std::endl;
        stream << indent << "| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) F |" << std::endl;
        stream << indent << "| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) G |" << std::endl;
        stream << indent << "| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) H |" << std::endl;
        stream << indent << "|                                       |" << std::endl;
        stream << indent << "| Novation               Launchpad MINI |" << std::endl;
        stream << indent << "+---------------------------------------+" << std::endl;
        stream << ""                                                    << std::endl;
    }
}

void HelpCmd::baseHelp(std::ostream& stream)
{
    if(stream.good()) {
        stream << "Usage: " << _program << ' ' << "[options] <command> [<args>]"      << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Commands:"                                                         << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    help [{command}]                    display help"              << std::endl;
        stream << "    list                                list available MIDI ports" << std::endl;
        stream << "    reset                               reset the Launchpad"       << std::endl;
        stream << "    cycle                               cycle colors"              << std::endl;
        stream << "    print {message}                     print a message"           << std::endl;
        stream << "    scroll {message}                    scroll a message"          << std::endl;
        stream << "    gameoflife [{pattern}]              display the game of life"  << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Options:"                                                          << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    -h, --help                          display this help"         << std::endl;
        stream << "    -l, --list                          list available MIDI ports" << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    --delay={value[us|ms|s|m]}          delay (ms by default)"     << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    --midi={port}                       MIDI input/output"         << std::endl;
        stream << "    --midi-input={port}                 MIDI input"                << std::endl;
        stream << "    --midi-output={port}                MIDI output"               << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "MIDI input/output:"                                                << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    [I] " << _midiIn                                               << std::endl;
        stream << "    [O] " << _midiOut                                              << std::endl;
        stream << ""                                                                  << std::endl;
    }
}

void HelpCmd::helpHelp(std::ostream& stream)
{
    if(stream.good()) {
        stream << "Usage: " << _program << ' ' << "[options] help [{command}]"        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Display help information about the program or a command"           << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Arguments:"                                                        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    command             specifies the command"                     << std::endl;
        stream << ""                                                                  << std::endl;
    }
}

void HelpCmd::listHelp(std::ostream& stream)
{
    if(stream.good()) {
        stream << "Usage: " << _program << ' ' << "[options] list"                    << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "List all available MIDI ports"                                     << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Arguments:"                                                        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    none"                                                          << std::endl;
        stream << ""                                                                  << std::endl;
    }
}

void HelpCmd::resetHelp(std::ostream& stream)
{
    if(stream.good()) {
        stream << "Usage: " << _program << ' ' << "[options] reset"                   << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Reset the Launchpad"                                               << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Arguments:"                                                        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    none"                                                          << std::endl;
        stream << ""                                                                  << std::endl;
    }
}

void HelpCmd::cycleHelp(std::ostream& stream)
{
    if(stream.good()) {
        stream << "Usage: " << _program << ' ' << "[options] cycle"                   << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Display a color cycle on the Launchpad"                            << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Arguments:"                                                        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    none"                                                          << std::endl;
        stream << ""                                                                  << std::endl;
    }
}

void HelpCmd::printHelp(std::ostream& stream)
{
    if(stream.good()) {
        stream << "Usage: " << _program << ' ' << "[options] print {message}"         << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Print a message on the Launchpad"                                  << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Arguments:"                                                        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    message             specifies the message to print"            << std::endl;
        stream << ""                                                                  << std::endl;
    }
}

void HelpCmd::scrollHelp(std::ostream& stream)
{
    if(stream.good()) {
        stream << "Usage: " << _program << ' ' << "[options] scroll {message}"        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Scroll a message on the Launchpad"                                 << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Arguments:"                                                        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    message             specifies the message to scroll"           << std::endl;
        stream << ""                                                                  << std::endl;
    }
}

void HelpCmd::gameoflifeHelp(std::ostream& stream)
{
    if(stream.good()) {
        stream << "Usage: " << _program << ' ' << "[options] gameoflife {pattern}"    << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Display a Game of Life on the Launchpad"                           << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Arguments:"                                                        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    pattern             specifies the pattern to simulate:"        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "                        'random' display a random pattern"         << std::endl;
        stream << "                        'glider' display a glider pattern"         << std::endl;
        stream << ""                                                                  << std::endl;
    }
}

}

// ---------------------------------------------------------------------------
// ListCmd
// ---------------------------------------------------------------------------

namespace launchpad {

ListCmd::ListCmd ( const Console&     console
                 , Launchpad&         launchpad
                 , const std::string& argument1
                 , const std::string& argument2
                 , const std::string& argument3
                 , const std::string& argument4
                 , const uint64_t     delay )
    : Command(console, launchpad, argument1, argument2, argument3, argument4, delay)
{
    lp::assert_argument(argument1, lp::kUNSET);
    lp::assert_argument(argument2, lp::kUNSET);
    lp::assert_argument(argument3, lp::kUNSET);
    lp::assert_argument(argument4, lp::kUNSET);
}

ListCmd::~ListCmd()
{
}

void ListCmd::execute()
{
    listInputs();
    listOutputs();
}

void ListCmd::listInputs()
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

void ListCmd::listOutputs()
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
// launchpad::ResetCmd
// ---------------------------------------------------------------------------

namespace launchpad {

ResetCmd::ResetCmd ( const Console&     console
                   , Launchpad&         launchpad
                   , const std::string& argument1
                   , const std::string& argument2
                   , const std::string& argument3
                   , const std::string& argument4
                   , const uint64_t     delay )
    : Command(console, launchpad, argument1, argument2, argument3, argument4, checkDelay(delay, DEFAULT_DELAY))
{
    lp::assert_argument(argument1, lp::kUNSET);
    lp::assert_argument(argument2, lp::kUNSET);
    lp::assert_argument(argument3, lp::kUNSET);
    lp::assert_argument(argument4, lp::kUNSET);
}

ResetCmd::~ResetCmd()
{
}

void ResetCmd::execute()
{
    lp::reset(_launchpad);
}

}

// ---------------------------------------------------------------------------
// launchpad::CycleCmd
// ---------------------------------------------------------------------------

namespace launchpad {

CycleCmd::CycleCmd ( const Console&     console
                   , Launchpad&         launchpad
                   , const std::string& argument1
                   , const std::string& argument2
                   , const std::string& argument3
                   , const std::string& argument4
                   , const uint64_t     delay )
    : Command(console, launchpad, argument1, argument2, argument3, argument4, checkDelay(delay, DEFAULT_DELAY))
{
    lp::assert_argument(argument1, lp::kUNSET);
    lp::assert_argument(argument2, lp::kUNSET);
    lp::assert_argument(argument3, lp::kUNSET);
    lp::assert_argument(argument4, lp::kUNSET);
}

CycleCmd::~CycleCmd()
{
    lp::clear(_launchpad, _black);
}

void CycleCmd::execute()
{
    lp::cycle(_launchpad, _delay, _stop);
}

}

// ---------------------------------------------------------------------------
// launchpad::PrintCmd
// ---------------------------------------------------------------------------

namespace launchpad {

PrintCmd::PrintCmd ( const Console&     console
                   , Launchpad&         launchpad
                   , const std::string& argument1
                   , const std::string& argument2
                   , const std::string& argument3
                   , const std::string& argument4
                   , const uint64_t     delay )
    : Command(console, launchpad, argument1, argument2, argument3, argument4, checkDelay(delay, DEFAULT_DELAY))
{
    lp::assert_argument(argument1, lp::kSET);
    lp::assert_argument(argument2, lp::kUNSET);
    lp::assert_argument(argument3, lp::kUNSET);
    lp::assert_argument(argument4, lp::kUNSET);
}

PrintCmd::~PrintCmd()
{
    lp::clear(_launchpad, _black);
}

void PrintCmd::execute()
{
    lp::print(_launchpad, _argument1, _red, _black, _delay, _stop);
}

}

// ---------------------------------------------------------------------------
// launchpad::ScrollCmd
// ---------------------------------------------------------------------------

namespace launchpad {

ScrollCmd::ScrollCmd ( const Console&     console
                     , Launchpad&         launchpad
                     , const std::string& argument1
                     , const std::string& argument2
                     , const std::string& argument3
                     , const std::string& argument4
                     , const uint64_t     delay )
    : Command(console, launchpad, argument1, argument2, argument3, argument4, checkDelay(delay, DEFAULT_DELAY))
{
    lp::assert_argument(argument1, lp::kSET);
    lp::assert_argument(argument2, lp::kUNSET);
    lp::assert_argument(argument3, lp::kUNSET);
    lp::assert_argument(argument4, lp::kUNSET);
}

ScrollCmd::~ScrollCmd()
{
    lp::clear(_launchpad, _black);
}

void ScrollCmd::execute()
{
    lp::scroll(_launchpad, _argument1, _red, _black, _delay, _stop);
}

}

// ---------------------------------------------------------------------------
// launchpad::GameOfLifeCmd
// ---------------------------------------------------------------------------

namespace launchpad {

GameOfLifeCmd::GameOfLifeCmd ( const Console&     console
                             , Launchpad&         launchpad
                             , const std::string& argument1
                             , const std::string& argument2
                             , const std::string& argument3
                             , const std::string& argument4
                             , const uint64_t     delay )
    : Command(console, launchpad, argument1, argument2, argument3, argument4, checkDelay(delay, DEFAULT_DELAY))
    , _color0(lp::color(0, 0))
    , _color1(lp::color(64, 0))
    , _color2(lp::color(255, 0))
    , _color3(lp::color(255, 255))
    , _color4(lp::color(0, 255))
    , _world()
    , _cache()
{
    lp::assert_argument(argument1, lp::kANY);
    lp::assert_argument(argument2, lp::kUNSET);
    lp::assert_argument(argument3, lp::kUNSET);
    lp::assert_argument(argument4, lp::kUNSET);
}

GameOfLifeCmd::~GameOfLifeCmd()
{
    lp::clear(_launchpad, _black);
}

void GameOfLifeCmd::execute()
{
    if(_stop == false) {
        init();
        do {
            loop();
        } while(_stop == false);
    }
}

void GameOfLifeCmd::init()
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
                set(row, col, (value != 0 ? Cell::kALIVE : Cell::kEMPTY));
            }
        }
    };

    auto init_glider = [&](const uint8_t row, const uint8_t col) -> void
    {
        set((row - 1), (col - 1), Cell::kEMPTY);
        set((row - 1), (col | 0), Cell::kALIVE);
        set((row - 1), (col + 1), Cell::kEMPTY);
        set((row | 0), (col - 1), Cell::kEMPTY);
        set((row | 0), (col | 0), Cell::kEMPTY);
        set((row | 0), (col + 1), Cell::kALIVE);
        set((row + 1), (col - 1), Cell::kALIVE);
        set((row + 1), (col | 0), Cell::kALIVE);
        set((row + 1), (col + 1), Cell::kALIVE);
    };

    if(_argument1.empty()) {
        init_random();
    }
    else if(_argument1 == "random") {
        init_random();
    }
    else if(_argument1 == "glider") {
        init_glider(1, 1);
    }
    else {
        throw std::runtime_error(std::string("invalid pattern") + ' ' + '<' + _argument1 + '>');
    }
}

void GameOfLifeCmd::loop()
{
    bool stable = true;

    auto stateOrDeath = [&](const Cell cell, const uint8_t neighbors) -> Cell
    {
        if((neighbors <= 1) || (neighbors >= 4)) {
            return Cell::kDYING;
        }
        return cell;
    };

    auto stateOrBirth = [&](const Cell cell, const uint8_t neighbors) -> Cell
    {
        if(neighbors == 3) {
            return Cell::kALIVE;
        }
        return cell;
    };

    auto mutate = [&](const Cell cell, const uint8_t neighbors) -> Cell
    {
        switch(cell) {
            case Cell::kALIVE:
                return stateOrDeath(Cell::kALIVE, neighbors);
            case Cell::kDYING:
                return stateOrBirth(Cell::kDEAD, neighbors);
            case Cell::kDEAD:
                return stateOrBirth(Cell::kASHES, neighbors);
            case Cell::kASHES:
                return stateOrBirth(Cell::kEMPTY, neighbors);
            case Cell::kEMPTY:
                return stateOrBirth(Cell::kEMPTY, neighbors);
            default:
                break;
        }
        return cell;
    };

    auto neighbors = [&](const uint8_t row, const uint8_t col) -> uint8_t
    {
        uint8_t count = 0;

        if(_cache.get((row - 1), (col - 1)) == Cell::kALIVE) ++count;
        if(_cache.get((row - 1), (col | 0)) == Cell::kALIVE) ++count;
        if(_cache.get((row - 1), (col + 1)) == Cell::kALIVE) ++count;
        if(_cache.get((row | 0), (col - 1)) == Cell::kALIVE) ++count;
        if(_cache.get((row | 0), (col + 1)) == Cell::kALIVE) ++count;
        if(_cache.get((row + 1), (col - 1)) == Cell::kALIVE) ++count;
        if(_cache.get((row + 1), (col | 0)) == Cell::kALIVE) ++count;
        if(_cache.get((row + 1), (col + 1)) == Cell::kALIVE) ++count;

        return count;
    };

    auto color = [&](const Cell cell) -> uint8_t
    {
        switch(cell) {
            case Cell::kEMPTY:
                return _color0;
            case Cell::kASHES:
                return _color1;
            case Cell::kDEAD:
                return _color2;
            case Cell::kDYING:
                return _color3;
            case Cell::kALIVE:
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
        if(_stop == false) {
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
