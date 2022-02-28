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
#include "Command.h"

// ---------------------------------------------------------------------------
// <anonymous>::lp
// ---------------------------------------------------------------------------

namespace {

struct lp
{
    static constexpr uint8_t ROWS = 8;
    static constexpr uint8_t COLS = 8;

    static constexpr uint8_t BUFFER0  = 0;
    static constexpr uint8_t BUFFER1  = 1;
    static constexpr bool    DO_FLASH = true;
    static constexpr bool    NO_FLASH = false;
    static constexpr bool    DO_COPY  = true;
    static constexpr bool    NO_COPY  = false;

    enum {
        kANY   = 0,
        kSET   = 1,
        kUNSET = 2,
    };

    static uint64_t check_delay(const uint64_t delay, const uint64_t default_delay)
    {
        if(delay == 0UL) {
            return default_delay;
        }
        return delay;
    }

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

    static void sleep(Launchpad& launchpad, const uint64_t delay)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(delay));
    }

    static void clear(Launchpad& launchpad)
    {
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                launchpad.clearPad(row, col);
            }
        }
    }

    static void print(Launchpad& launchpad, const std::string& string, const uint8_t foreground, const uint8_t background, const uint64_t delay, bool& stop)
    {
        for(auto character : string) {
            if(stop != false) {
                break;
            }
            launchpad.setBuffer(BUFFER0, BUFFER1, NO_FLASH, NO_COPY);
            const uint8_t index = (character < Font8x8::MAX_CHARS ? character : 0);
            for(uint8_t row = 0; row < ROWS; ++row) {
                const uint8_t val = Font8x8::data[index][row];
                uint8_t       bit = 0x01;
                for(uint8_t col = 0; col < COLS; ++col) {
                    if((val & bit) != 0) {
                        launchpad.setPad(row, col, foreground);
                    }
                    else {
                        launchpad.setPad(row, col, background);
                    }
                    bit <<= 1;
                }
            }
            launchpad.setBuffer(BUFFER1, BUFFER0, NO_FLASH, DO_COPY);
            launchpad.setBuffer(BUFFER0, BUFFER0, NO_FLASH, NO_COPY);
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
            launchpad.setBuffer(BUFFER0, BUFFER1, NO_FLASH, NO_COPY);
            for(uint8_t row = 0; row < ROWS; ++row) {
                for(uint8_t col = 0; col < COLS; ++col) {
                    const uint8_t chr = data[((col + pix) / COLS)];
                    const uint8_t bit = 1 << ((col + pix) % COLS);
                    const uint8_t val = Font8x8::data[chr][row];
                    if((val & bit) != 0) {
                        launchpad.setPad(row, col, foreground);
                    }
                    else {
                        launchpad.setPad(row, col, background);
                    }
                }
            }
            launchpad.setBuffer(BUFFER1, BUFFER0, NO_FLASH, DO_COPY);
            launchpad.setBuffer(BUFFER0, BUFFER0, NO_FLASH, NO_COPY);
            lp::sleep(launchpad, delay);
        }
    }
};

}

// ---------------------------------------------------------------------------
// AbstractCommand
// ---------------------------------------------------------------------------

AbstractCommand::AbstractCommand(const Console& console)
    : _arglist()
    , _console(console)
{
}

AbstractCommand::AbstractCommand(const ArgList& arglist, const Console& console)
    : _arglist(arglist)
    , _console(console)
{
}

AbstractCommand::~AbstractCommand()
{
}

void AbstractCommand::println(const std::string& message)
{
    if(_console.printStream.good()) {
        _console.printStream << message << std::endl;
    }
}

void AbstractCommand::println(const std::string& prefix, const std::string& message)
{
    if(_console.printStream.good()) {
        _console.printStream << prefix << ' ' << message << std::endl;
    }
}

void AbstractCommand::errorln(const std::string& message)
{
    if(_console.errorStream.good()) {
        _console.errorStream << message << std::endl;
    }
}

void AbstractCommand::errorln(const std::string& prefix, const std::string& message)
{
    if(_console.errorStream.good()) {
        _console.errorStream << prefix << ' ' << message << std::endl;
    }
}

// ---------------------------------------------------------------------------
// Command
// ---------------------------------------------------------------------------

Command::Command ( const ArgList&     arglist
                 , const Console&     console
                 , Launchpad&         launchpad
                 , const uint64_t     delay )
    : AbstractCommand(arglist, console)
    , _launchpad(launchpad)
    , _delay(delay)
    , _black(_launchpad.makeColor(0, 0))
    , _red(_launchpad.makeColor(255, 0))
    , _green(_launchpad.makeColor(0, 255))
    , _amber(_launchpad.makeColor(255, 255))
    , _stop(false)
{
    _launchpad.setListener(this);
}

Command::~Command()
{
    _launchpad.setListener(nullptr);
}

void Command::sleep(const uint64_t delay)
{
    std::this_thread::sleep_for(std::chrono::microseconds(delay));
}

// ---------------------------------------------------------------------------
// HelpCmd
// ---------------------------------------------------------------------------

namespace launchpad {

HelpCmd::HelpCmd ( const ArgList&     arglist
                 , const Console&     console
                 , Launchpad&         launchpad
                 , const uint64_t     delay
                 , const std::string& program
                 , const std::string& midiIn
                 , const std::string& midiOut )
    : Command(arglist, console, launchpad, delay)
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
    std::string command;

    if(_arglist.count() > 0) {
        command = _arglist.at(0);
    }
    if(command.empty()) {
        baseUsage(_console.printStream);
        launchpad(_console.printStream);
    }
    else if(command == "help") {
        helpUsage(_console.printStream);
    }
    else if(command == "list") {
        listUsage(_console.printStream);
    }
    else if(command == "reset") {
        resetUsage(_console.printStream);
    }
    else if(command == "cycle") {
        cycleUsage(_console.printStream);
    }
    else if(command == "print") {
        printUsage(_console.printStream);
    }
    else if(command == "scroll") {
        scrollUsage(_console.printStream);
    }
    else if(command == "matrix") {
        matrixUsage(_console.printStream);
    }
    else if(command == "gameoflife") {
        gameoflifeUsage(_console.printStream);
    }
    else {
        throw std::runtime_error(std::string("no help for") + ' ' + '<' + command + '>');
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

void HelpCmd::baseUsage(std::ostream& stream)
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
        stream << "    matrix                              matrix-like rain effect"   << std::endl;
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

void HelpCmd::helpUsage(std::ostream& stream)
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

void HelpCmd::listUsage(std::ostream& stream)
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

void HelpCmd::resetUsage(std::ostream& stream)
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

void HelpCmd::cycleUsage(std::ostream& stream)
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

void HelpCmd::printUsage(std::ostream& stream)
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

void HelpCmd::scrollUsage(std::ostream& stream)
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

void HelpCmd::matrixUsage(std::ostream& stream)
{
    if(stream.good()) {
        stream << "Usage: " << _program << ' ' << "[options] matrix"                  << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Display a Matrix-like rain effect"                                 << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "Arguments:"                                                        << std::endl;
        stream << ""                                                                  << std::endl;
        stream << "    none"                                                          << std::endl;
        stream << ""                                                                  << std::endl;
    }
}

void HelpCmd::gameoflifeUsage(std::ostream& stream)
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

ListCmd::ListCmd ( const ArgList& arglist
                 , const Console& console
                 , Launchpad&     launchpad
                 , const uint64_t delay )
    : Command(arglist, console, launchpad, delay)
{
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

ResetCmd::ResetCmd ( const ArgList& arglist
                   , const Console& console
                   , Launchpad&     launchpad
                   , const uint64_t delay )
    : Command(arglist, console, launchpad, lp::check_delay(delay, DEFAULT_DELAY))
{
}

ResetCmd::~ResetCmd()
{
}

void ResetCmd::execute()
{
    _launchpad.reset();
}

}

// ---------------------------------------------------------------------------
// launchpad::CycleCmd
// ---------------------------------------------------------------------------

namespace launchpad {

CycleCmd::CycleCmd ( const ArgList& arglist
                   , const Console& console
                   , Launchpad&     launchpad
                   , const uint64_t delay )
    : Command(arglist, console, launchpad, lp::check_delay(delay, DEFAULT_DELAY))
{
}

CycleCmd::~CycleCmd()
{
    lp::clear(_launchpad);
}

void CycleCmd::execute()
{
    auto begin = [&]() -> void
    {
        _launchpad.setBuffer(lp::BUFFER0, lp::BUFFER1, lp::NO_FLASH, lp::NO_COPY);
    };

    auto end = [&]() -> void
    {
        _launchpad.setBuffer(lp::BUFFER1, lp::BUFFER0, lp::NO_FLASH, lp::DO_COPY);
        _launchpad.setBuffer(lp::BUFFER0, lp::BUFFER0, lp::NO_FLASH, lp::NO_COPY);
        sleep(_delay);
    };

    auto display = [&](const uint8_t color) -> void
    {
        for(uint8_t row = 0; row < lp::ROWS; ++row) {
            for(uint8_t col = 0; col < lp::COLS; ++col) {
                _launchpad.setPad(row, col, color);
            }
        }
    };

    for(int r = 0; r < 256; r += 85) {
        if(_stop != false) {
            break;
        }
        for(int g = 0; g < 256; g += 85) {
            if(_stop != false) {
                break;
            }
            begin();
            display(_launchpad.makeColor(r, g));
            end();
        }
    }
}

}

// ---------------------------------------------------------------------------
// launchpad::PrintCmd
// ---------------------------------------------------------------------------

namespace launchpad {

PrintCmd::PrintCmd ( const ArgList& arglist
                   , const Console& console
                   , Launchpad&     launchpad
                   , const uint64_t delay )
    : Command(arglist, console, launchpad, lp::check_delay(delay, DEFAULT_DELAY))
    , _text()
{
    if(_arglist.count() != 1) {
        throw std::runtime_error("invalid argument count");
    }
    else {
        _text = _arglist.at(0);
    }
}

PrintCmd::~PrintCmd()
{
    lp::clear(_launchpad);
}

void PrintCmd::execute()
{
    lp::print(_launchpad, _text, _red, _black, _delay, _stop);
}

}

// ---------------------------------------------------------------------------
// launchpad::ScrollCmd
// ---------------------------------------------------------------------------

namespace launchpad {

ScrollCmd::ScrollCmd ( const ArgList& arglist
                     , const Console& console
                     , Launchpad&     launchpad
                     , const uint64_t delay )
    : Command(arglist, console, launchpad, lp::check_delay(delay, DEFAULT_DELAY))
    , _text()
{
    if(_arglist.count() != 1) {
        throw std::runtime_error("invalid argument count");
    }
    else {
        _text = _arglist.at(0);
    }
}

ScrollCmd::~ScrollCmd()
{
    lp::clear(_launchpad);
}

void ScrollCmd::execute()
{
    lp::scroll(_launchpad, _text, _red, _black, _delay, _stop);
}

}

// ---------------------------------------------------------------------------
// launchpad::MatrixCmd
// ---------------------------------------------------------------------------

namespace launchpad {

MatrixCmd::MatrixCmd ( const ArgList& arglist
                     , const Console& console
                     , Launchpad&     launchpad
                     , const uint64_t delay )
    : Command(arglist, console, launchpad, lp::check_delay(delay, DEFAULT_DELAY))
    , _color0(_launchpad.makeColor(0, 0))
    , _color1(_launchpad.makeColor(0, 85))
    , _color2(_launchpad.makeColor(0, 170))
    , _color3(_launchpad.makeColor(0, 255))
    , _color4(_launchpad.makeColor(255, 255))
    , _color5(_launchpad.makeColor(255, 0))
    , _matrix()
{
    if(_arglist.count() != 0) {
        throw std::runtime_error("invalid argument count");
    }
}

MatrixCmd::~MatrixCmd()
{
    lp::clear(_launchpad);
}

void MatrixCmd::execute()
{
    if(_stop == false) {
        init();
        do {
            loop();
            wait();
        } while(_stop == false);
    }
}

void MatrixCmd::onLaunchpadGridKey(const uint8_t key, const uint8_t velocity)
{
    const uint8_t row = key / 16;
    const uint8_t col = key % 16;
    if((row < ROWS) && (col < COLS)) {
        _matrix.data[row % ROWS][col % COLS] = Cell::kLEVEL5;
    }
}

void MatrixCmd::onLaunchpadLiveKey(const uint8_t key, const uint8_t velocity)
{
    static_cast<void>(key);
    static_cast<void>(velocity);
}

void MatrixCmd::init()
{
    ::srand(::time(nullptr));
}

void MatrixCmd::loop()
{
    auto color = [&](const Cell cell) -> uint8_t
    {
        switch(cell) {
            case Cell::kLEVEL0:
                return _color0;
            case Cell::kLEVEL1:
                return _color1;
            case Cell::kLEVEL2:
                return _color2;
            case Cell::kLEVEL3:
                return _color3;
            case Cell::kLEVEL4:
                return _color4;
            case Cell::kLEVEL5:
                return _color5;
            default:
                break;
        }
        return _black;
    };

    auto mutate = [&](const Cell cell) -> Cell
    {
        switch(cell) {
            case Cell::kLEVEL5:
                return Cell::kLEVEL4;
            case Cell::kLEVEL4:
                return Cell::kLEVEL3;
            case Cell::kLEVEL3:
                return Cell::kLEVEL2;
            case Cell::kLEVEL2:
                return Cell::kLEVEL1;
            case Cell::kLEVEL1:
                return Cell::kLEVEL0;
            case Cell::kLEVEL0:
                return Cell::kLEVEL0;
            default:
                break;
        }
        return cell;
    };

    auto display = [&]() -> void
    {
        _launchpad.setBuffer(lp::BUFFER0, lp::BUFFER1, lp::NO_FLASH, lp::NO_COPY);
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                const Cell& cell(_matrix.get(row, col));
                _launchpad.setPad(row, col, color(cell));
            }
        }
        _launchpad.setBuffer(lp::BUFFER1, lp::BUFFER0, lp::NO_FLASH, lp::DO_COPY);
        _launchpad.setBuffer(lp::BUFFER0, lp::BUFFER0, lp::NO_FLASH, lp::NO_COPY);
    };

    auto prepare = [&]() -> void
    {
    };

    auto process = [&]() -> void
    {
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                const Cell& prev(_matrix.get(((ROWS - 1) - row - 1), col));
                Cell&       cell(_matrix.get(((ROWS - 1) - row - 0), col));
                if(prev == Cell::kLEVEL5) {
                    cell = prev;
                }
                else {
                    cell = mutate(cell);
                }
            }
        }
    };

    auto finalize = [&]() -> void
    {
        const uint8_t row = 0;
        const uint8_t col = ::rand() % 31;
        if(col < COLS) {
            _matrix.data[row][col] = Cell::kLEVEL5;
        }
    };

    display();
    prepare();
    process();
    finalize();
}

void MatrixCmd::wait()
{
    if(_stop == false) {
        sleep(_delay);
    }
}

}

// ---------------------------------------------------------------------------
// launchpad::GameOfLifeCmd
// ---------------------------------------------------------------------------

namespace launchpad {

GameOfLifeCmd::GameOfLifeCmd ( const ArgList& arglist
                             , const Console& console
                             , Launchpad&     launchpad
                             , const uint64_t delay )
    : Command(arglist, console, launchpad, lp::check_delay(delay, DEFAULT_DELAY))
    , _variant()
    , _color0(_launchpad.makeColor(0, 0))
    , _color1(_launchpad.makeColor(64, 0))
    , _color2(_launchpad.makeColor(255, 0))
    , _color3(_launchpad.makeColor(255, 255))
    , _color4(_launchpad.makeColor(0, 255))
    , _world()
    , _cache()
{
    if(_arglist.count() == 0) {
        /* default */
    }
    else if(_arglist.count() == 1) {
        _variant = _arglist.at(0);
    }
    else {
        throw std::runtime_error("invalid argument count");
    }
}

GameOfLifeCmd::~GameOfLifeCmd()
{
    lp::clear(_launchpad);
}

void GameOfLifeCmd::execute()
{
    if(_stop == false) {
        init();
        do {
            loop();
            wait();
        } while(_stop == false);
    }
}

void GameOfLifeCmd::onLaunchpadGridKey(const uint8_t key, const uint8_t velocity)
{
    const uint8_t row = key / 16;
    const uint8_t col = key % 16;

    if((row < ROWS) && (col < COLS)) {
        _world.data[row % ROWS][col % COLS] = Cell::kALIVE;
    }
}

void GameOfLifeCmd::onLaunchpadLiveKey(const uint8_t key, const uint8_t velocity)
{
    static_cast<void>(key);
    static_cast<void>(velocity);
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

    if(_variant.empty()) {
        init_random();
    }
    else if(_variant == "random") {
        init_random();
    }
    else if(_variant == "glider") {
        init_glider(1, 1);
    }
    else {
        throw std::runtime_error(std::string("invalid pattern") + ' ' + '<' + _variant + '>');
    }
}

void GameOfLifeCmd::loop()
{
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
        _launchpad.setBuffer(lp::BUFFER0, lp::BUFFER1, lp::NO_FLASH, lp::NO_COPY);
        for(uint8_t row = 0; row < ROWS; ++row) {
            for(uint8_t col = 0; col < COLS; ++col) {
                const Cell& cell(_world.get(row, col));
                _launchpad.setPad(row, col, color(cell));
            }
        }
        _launchpad.setBuffer(lp::BUFFER1, lp::BUFFER0, lp::NO_FLASH, lp::DO_COPY);
        _launchpad.setBuffer(lp::BUFFER0, lp::BUFFER0, lp::NO_FLASH, lp::NO_COPY);
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
            }
        }
    };

    auto finalize = [&]() -> void
    {
    };

    display();
    prepare();
    process();
    finalize();
}

void GameOfLifeCmd::wait()
{
    if(_stop == false) {
        sleep(_delay);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
