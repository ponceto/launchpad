/*
 * Command.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __Command_h__
#define __Command_h__

#include <base/Console.h>
#include <novation/Launchpad.h>
#include <novation/Font5x7.h>
#include <novation/Font8x8.h>

// ---------------------------------------------------------------------------
// some aliases
// ---------------------------------------------------------------------------

using Console            = base::Console;
using Launchpad          = novation::Launchpad;
using LaunchpadListener  = novation::LaunchpadListener;
using LaunchpadUniquePtr = std::unique_ptr<Launchpad>;
using Font5x7            = novation::Font5x7;
using Font8x8            = novation::Font8x8;

// ---------------------------------------------------------------------------
// CommandType
// ---------------------------------------------------------------------------

enum class CommandType
{
    kNONE       = 0,
    kHELP       = 1,
    kLIST       = 2,
    kRESET      = 3,
    kCYCLE      = 4,
    kPRINT      = 5,
    kSCROLL     = 6,
    kMATRIX     = 7,
    kGAMEOFLIFE = 8,
};

// ---------------------------------------------------------------------------
// Command
// ---------------------------------------------------------------------------

class Command
    : public LaunchpadListener
{
public: // public interface
    Command ( const Console&     console
            , Launchpad&         launchpad
            , const std::string& argument1
            , const std::string& argument2
            , const std::string& argument3
            , const std::string& argument4
            , const uint64_t     delay );

    virtual ~Command();

    virtual void execute() = 0;

    void stop()
    {
        _stop = true;
    }

protected: // protected interface
    void println(const std::string& message = std::string());

    void println(const std::string& prefix, const std::string& message);

    void errorln(const std::string& message = std::string());

    void errorln(const std::string& prefix, const std::string& message);

    void sleep(const uint64_t delay);

    static uint64_t checkDelay(const uint64_t delay, const uint64_t default_delay)
    {
        if(delay == 0UL) {
            return default_delay;
        }
        return delay;
    }

protected: // protected data
    const Console&    _console;
    Launchpad&        _launchpad;
    const std::string _argument1;
    const std::string _argument2;
    const std::string _argument3;
    const std::string _argument4;
    const uint64_t    _delay;
    const uint8_t     _black;
    const uint8_t     _red;
    const uint8_t     _green;
    const uint8_t     _amber;
    bool              _stop;
};

// ---------------------------------------------------------------------------
// CommandUniquePtr
// ---------------------------------------------------------------------------

using CommandUniquePtr = std::unique_ptr<Command>;

// ---------------------------------------------------------------------------
// launchpad::HelpCmd
// ---------------------------------------------------------------------------

namespace launchpad {

class HelpCmd final
    : public Command
{
public: // public interface
    HelpCmd ( const Console&     console
            , Launchpad&         launchpad
            , const std::string& argument1
            , const std::string& argument2
            , const std::string& argument3
            , const std::string& argument4
            , const uint64_t     delay
            , const std::string& program
            , const std::string& midiIn
            , const std::string& midiOut );

    virtual ~HelpCmd();

    virtual void execute() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 0UL;

private: // private interface
    void launchpad(std::ostream&);
    void baseHelp(std::ostream&);
    void helpHelp(std::ostream&);
    void listHelp(std::ostream&);
    void resetHelp(std::ostream&);
    void cycleHelp(std::ostream&);
    void printHelp(std::ostream&);
    void scrollHelp(std::ostream&);
    void matrixHelp(std::ostream&);
    void gameoflifeHelp(std::ostream&);

private: // private data
    const std::string _program;
    const std::string _midiIn;
    const std::string _midiOut;
};

}

// ---------------------------------------------------------------------------
// launchpad::ListCmd
// ---------------------------------------------------------------------------

namespace launchpad {

class ListCmd final
    : public Command
{
public: // public interface
    ListCmd ( const Console&     console
            , Launchpad&         launchpad
            , const std::string& argument1
            , const std::string& argument2
            , const std::string& argument3
            , const std::string& argument4
            , const uint64_t     delay );

    virtual ~ListCmd();

    virtual void execute() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 0UL;

private: // private interface
    virtual void listInputs();
    virtual void listOutputs();
};

}

// ---------------------------------------------------------------------------
// launchpad::ResetCmd
// ---------------------------------------------------------------------------

namespace launchpad {

class ResetCmd final
    : public Command
{
public: // public interface
    ResetCmd ( const Console&     console
             , Launchpad&         launchpad
             , const std::string& argument1
             , const std::string& argument2
             , const std::string& argument3
             , const std::string& argument4
             , const uint64_t     delay );

    virtual ~ResetCmd();

    virtual void execute() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 0UL;
};

}

// ---------------------------------------------------------------------------
// launchpad::CycleCmd
// ---------------------------------------------------------------------------

namespace launchpad {

class CycleCmd final
    : public Command
{
public: // public interface
    CycleCmd ( const Console&     console
             , Launchpad&         launchpad
             , const std::string& argument1
             , const std::string& argument2
             , const std::string& argument3
             , const std::string& argument4
             , const uint64_t     delay );

    virtual ~CycleCmd();

    virtual void execute() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 500UL * 1000UL;
};

}

// ---------------------------------------------------------------------------
// launchpad::PrintCmd
// ---------------------------------------------------------------------------

namespace launchpad {

class PrintCmd final
    : public Command
{
public: // public interface
    PrintCmd ( const Console&     console
             , Launchpad&         launchpad
             , const std::string& argument1
             , const std::string& argument2
             , const std::string& argument3
             , const std::string& argument4
             , const uint64_t     delay );

    virtual ~PrintCmd();

    virtual void execute() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 250UL * 1000UL;
};

}

// ---------------------------------------------------------------------------
// launchpad::ScrollCmd
// ---------------------------------------------------------------------------

namespace launchpad {

class ScrollCmd final
    : public Command
{
public: // public interface
    ScrollCmd ( const Console&     console
              , Launchpad&         launchpad
              , const std::string& argument1
              , const std::string& argument2
              , const std::string& argument3
              , const std::string& argument4
              , const uint64_t     delay );

    virtual ~ScrollCmd();

    virtual void execute() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 125UL * 1000UL;
};

}

// ---------------------------------------------------------------------------
// launchpad::MatrixCmd
// ---------------------------------------------------------------------------

namespace launchpad {

class MatrixCmd final
    : public Command
{
public: // public interface
    MatrixCmd ( const Console&     console
              , Launchpad&         launchpad
              , const std::string& argument1
              , const std::string& argument2
              , const std::string& argument3
              , const std::string& argument4
              , const uint64_t     delay );

    virtual ~MatrixCmd();

    virtual void execute() override;

    virtual void onLaunchpadGridKey(const uint8_t key, const uint8_t velocity) override;

    virtual void onLaunchpadLiveKey(const uint8_t key, const uint8_t velocity) override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 150UL * 1000UL;
    static constexpr uint8_t  ROWS          = 8;
    static constexpr uint8_t  COLS          = 8;

private: // private interface
    enum class Cell : uint8_t
    {
        kLEVEL0 = 0,
        kLEVEL1 = 1,
        kLEVEL2 = 2,
        kLEVEL3 = 3,
        kLEVEL4 = 4,
        kLEVEL5 = 5,
    };

    struct Matrix
    {
        Cell data[ROWS][COLS];
        Cell cell;

        Cell& get(const uint8_t row, const uint8_t col)
        {
            if((row < ROWS) && (col < COLS)) {
                return data[row][col];
            }
            return cell;
        }

        const Cell& get(const uint8_t row, const uint8_t col) const
        {
            if((row < ROWS) && (col < COLS)) {
                return data[row][col];
            }
            return cell;
        }
    };

    void init();
    void loop();
    void wait();

private: // private data
    const uint8_t _color0;
    const uint8_t _color1;
    const uint8_t _color2;
    const uint8_t _color3;
    const uint8_t _color4;
    const uint8_t _color5;
    Matrix        _matrix;
};

}

// ---------------------------------------------------------------------------
// launchpad::GameOfLifeCmd
// ---------------------------------------------------------------------------

namespace launchpad {

class GameOfLifeCmd final
    : public Command
{
public: // public interface
    GameOfLifeCmd ( const Console&     console
                  , Launchpad&         launchpad
                  , const std::string& argument1
                  , const std::string& argument2
                  , const std::string& argument3
                  , const std::string& argument4
                  , const uint64_t     delay );

    virtual ~GameOfLifeCmd();

    virtual void execute() override;

    virtual void onLaunchpadGridKey(const uint8_t key, const uint8_t velocity) override;

    virtual void onLaunchpadLiveKey(const uint8_t key, const uint8_t velocity) override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 750UL * 1000UL;
    static constexpr uint8_t  ROWS          = 8;
    static constexpr uint8_t  COLS          = 8;

private: // private interface
    enum class Cell : uint8_t
    {
        kEMPTY = 0,
        kASHES = 1,
        kDEAD  = 2,
        kDYING = 3,
        kALIVE = 4,
    };

    struct State
    {
        Cell data[ROWS][COLS];

        Cell& get(const uint8_t row, const uint8_t col)
        {
            return data[row % ROWS][col % COLS];
        }

        const Cell& get(const uint8_t row, const uint8_t col) const
        {
            return data[row % ROWS][col % COLS];
        }
    };

    void init();
    void loop();
    void wait();

private: // private data
    const uint8_t _color0;
    const uint8_t _color1;
    const uint8_t _color2;
    const uint8_t _color3;
    const uint8_t _color4;
    State         _world;
    State         _cache;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __Command_h__ */
