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

#include <base/ArgList.h>
#include <base/Console.h>
#include <novation/Launchpad.h>
#include <novation/Font5x7.h>
#include <novation/Font8x8.h>

// ---------------------------------------------------------------------------
// some aliases
// ---------------------------------------------------------------------------

using ArgList            = base::ArgList;
using Console            = base::Console;
using Launchpad          = novation::Launchpad;
using LaunchpadDecorator = novation::LaunchpadDecorator;
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
// AbstractCommand
// ---------------------------------------------------------------------------

class AbstractCommand
{
public: // public interface
    AbstractCommand(const Console& console);

    AbstractCommand(const ArgList& arglist, const Console& console);

    virtual ~AbstractCommand();

    virtual void execute() = 0;

protected: // protected interface
    void println(const std::string& message = std::string());

    void println(const std::string& prefix, const std::string& message);

    void errorln(const std::string& message = std::string());

    void errorln(const std::string& prefix, const std::string& message);

protected: // protected data
    const ArgList  _arglist;
    const Console& _console;
};

// ---------------------------------------------------------------------------
// Command
// ---------------------------------------------------------------------------

class Command
    : public AbstractCommand
    , public LaunchpadDecorator
    , public LaunchpadListener
{
public: // public interface
    Command ( Launchpad&     launchpad
            , const ArgList& arglist
            , const Console& console
            , const uint64_t delay );

    virtual ~Command();

    void stop()
    {
        _stop = true;
    }

    void sleep(const uint64_t delay);

protected: // protected data
    const uint64_t _delay;
    const uint8_t  _black;
    const uint8_t  _red;
    const uint8_t  _green;
    const uint8_t  _amber;
    bool           _stop;
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
    HelpCmd ( Launchpad&         launchpad
            , const ArgList&     arglist
            , const Console&     console
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
    void baseUsage(std::ostream&);
    void helpUsage(std::ostream&);
    void listUsage(std::ostream&);
    void resetUsage(std::ostream&);
    void cycleUsage(std::ostream&);
    void printUsage(std::ostream&);
    void scrollUsage(std::ostream&);
    void matrixUsage(std::ostream&);
    void gameoflifeUsage(std::ostream&);

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
    ListCmd ( Launchpad&     launchpad
            , const ArgList& arglist
            , const Console& console
            , const uint64_t delay );

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
    ResetCmd ( Launchpad&     launchpad
             , const ArgList& arglist
             , const Console& console
             , const uint64_t delay );

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
    CycleCmd ( Launchpad&     launchpad
             , const ArgList& arglist
             , const Console& console
             , const uint64_t delay );

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
    PrintCmd ( Launchpad&     launchpad
             , const ArgList& arglist
             , const Console& console
             , const uint64_t delay );

    virtual ~PrintCmd();

    virtual void execute() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 250UL * 1000UL;

private: // private data
    std::string _text;
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
    ScrollCmd ( Launchpad&     launchpad
              , const ArgList& arglist
              , const Console& console
              , const uint64_t delay );

    virtual ~ScrollCmd();

    virtual void execute() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 125UL * 1000UL;

private: // private data
    std::string _text;
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
    MatrixCmd ( Launchpad&     launchpad
              , const ArgList& arglist
              , const Console& console
              , const uint64_t delay );

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
    GameOfLifeCmd ( Launchpad&     launchpad
                  , const ArgList& arglist
                  , const Console& console
                  , const uint64_t delay );

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
    std::string   _variant;
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
