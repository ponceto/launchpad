/*
 * LaunchpadApp.h - Copyright (c) 2001-2022 - Olivier Poncet
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
#ifndef __LaunchpadApp_h__
#define __LaunchpadApp_h__

#include <base/Console.h>
#include <novation/Launchpad.h>

// ---------------------------------------------------------------------------
// some aliases
// ---------------------------------------------------------------------------

using Console            = base::Console;
using Launchpad          = novation::Launchpad;
using LaunchpadUniquePtr = std::unique_ptr<Launchpad>;

// ---------------------------------------------------------------------------
// LaunchpadAppType
// ---------------------------------------------------------------------------

enum class LaunchpadAppType
{
    kNONE       = 0,
    kHELP       = 1,
    kLIST       = 2,
    kCYCLE      = 3,
    kPRINT      = 4,
    kSCROLL     = 5,
    kGAMEOFLIFE = 6,
};

// ---------------------------------------------------------------------------
// LaunchpadApp
// ---------------------------------------------------------------------------

class LaunchpadApp
{
public: // public interface
    LaunchpadApp ( const Console&     console
                 , Launchpad&         launchpad
                 , const std::string& param1
                 , const std::string& param2
                 , const std::string& param3
                 , const std::string& param4
                 , const uint64_t     delay );

    virtual ~LaunchpadApp();

    virtual void main() = 0;

    void shutdown()
    {
        _shutdown = true;
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
    const std::string _param1;
    const std::string _param2;
    const std::string _param3;
    const std::string _param4;
    const uint64_t    _delay;
    const uint8_t     _black;
    const uint8_t     _red;
    const uint8_t     _green;
    const uint8_t     _amber;
    bool              _shutdown;
};

// ---------------------------------------------------------------------------
// LaunchpadAppUniquePtr
// ---------------------------------------------------------------------------

using LaunchpadAppUniquePtr = std::unique_ptr<LaunchpadApp>;

// ---------------------------------------------------------------------------
// launchpad::HelpApp
// ---------------------------------------------------------------------------

namespace launchpad {

class HelpApp final
    : public LaunchpadApp
{
public: // public interface
    HelpApp ( const Console&     console
            , Launchpad&         launchpad
            , const std::string& param1
            , const std::string& param2
            , const std::string& param3
            , const std::string& param4
            , const uint64_t     delay
            , const std::string& program
            , const std::string& midiIn
            , const std::string& midiOut );

    virtual ~HelpApp();

    virtual void main() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 0UL;

private: // private data
    const std::string _program;
    const std::string _midiIn;
    const std::string _midiOut;
};

}

// ---------------------------------------------------------------------------
// launchpad::ListApp
// ---------------------------------------------------------------------------

namespace launchpad {

class ListApp final
    : public LaunchpadApp
{
public: // public interface
    ListApp ( const Console&     console
            , Launchpad&         launchpad
            , const std::string& param1
            , const std::string& param2
            , const std::string& param3
            , const std::string& param4
            , const uint64_t     delay );

    virtual ~ListApp();

    virtual void main() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 0UL;

private: // private interface
    virtual void listInputs();
    virtual void listOutputs();
};

}

// ---------------------------------------------------------------------------
// launchpad::CycleApp
// ---------------------------------------------------------------------------

namespace launchpad {

class CycleApp final
    : public LaunchpadApp
{
public: // public interface
    CycleApp ( const Console&     console
             , Launchpad&         launchpad
             , const std::string& param1
             , const std::string& param2
             , const std::string& param3
             , const std::string& param4
             , const uint64_t     delay );

    virtual ~CycleApp();

    virtual void main() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 500UL * 1000UL;
};

}

// ---------------------------------------------------------------------------
// launchpad::PrintApp
// ---------------------------------------------------------------------------

namespace launchpad {

class PrintApp final
    : public LaunchpadApp
{
public: // public interface
    PrintApp ( const Console&     console
             , Launchpad&         launchpad
             , const std::string& param1
             , const std::string& param2
             , const std::string& param3
             , const std::string& param4
             , const uint64_t     delay );

    virtual ~PrintApp();

    virtual void main() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 250UL * 1000UL;
};

}

// ---------------------------------------------------------------------------
// launchpad::ScrollApp
// ---------------------------------------------------------------------------

namespace launchpad {

class ScrollApp final
    : public LaunchpadApp
{
public: // public interface
    ScrollApp ( const Console&     console
              , Launchpad&         launchpad
              , const std::string& param1
              , const std::string& param2
              , const std::string& param3
              , const std::string& param4
              , const uint64_t     delay );

    virtual ~ScrollApp();

    virtual void main() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 125UL * 1000UL;
};

}

// ---------------------------------------------------------------------------
// launchpad::GameOfLifeApp
// ---------------------------------------------------------------------------

namespace launchpad {

class GameOfLifeApp final
    : public LaunchpadApp
{
public: // public interface
    GameOfLifeApp ( const Console&     console
                  , Launchpad&         launchpad
                  , const std::string& param1
                  , const std::string& param2
                  , const std::string& param3
                  , const std::string& param4
                  , const uint64_t     delay );

    virtual ~GameOfLifeApp();

    virtual void main() override;

private: // private static data
    static constexpr uint64_t DEFAULT_DELAY = 750UL * 1000UL;
    static constexpr uint8_t  ROWS          = 8;
    static constexpr uint8_t  COLS          = 8;

private: // private interface
    enum class Cell : uint8_t
    {
        kNONE = 0,
        kHIDE = 1,
        kDEAD = 2,
        kKILL = 3,
        kLIVE = 4,
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

#endif /* __LaunchpadApp_h__ */
